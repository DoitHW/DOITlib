#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Wire.h>


extern DOITSOUNDS_ doitPlayer;

void TOKEN_::begin() {
    // Inicialización del objeto
}

bool TOKEN_::isCardPresent() {
    // Verifica si hay una tarjeta presente
    return true;
}

// Función para proponer un nuevo token aleatorio
void TOKEN_::proponer_token() {
    proposedToken.fileAddr.bank = random(1, 10); // Genera un número aleatorio para "bank"
    proposedToken.fileAddr.file = random(1, 10); // Genera un número aleatorio para "file"
    doitPlayer.play_file(proposedToken.fileAddr.bank, proposedToken.fileAddr.file);
    proposed = true; // Marca que ya se ha propuesto un token
}


void TOKEN_::token_action(std::vector<byte> targets, TOKEN_INFO tokenin, byte LANG, byte tokenMode) {
    static TOKEN_FILE_ADDR lastToken = {0, 0}; 

    doitPlayer.play_file(tokenin.fileAddr.bank, tokenin.fileAddr.file);
    send_frame(frameMaker_SEND_COLOR(globalID, targets, tokenin.color));
    while(doitPlayer.is_playing()){}
    send_frame(frameMaker_SEND_COLOR(globalID, targets, BLACK));

    if (tokenMode == GUESS_TOKEN_MODE) {
        if (!proposed) {
            proponer_token();
            proposed= true;
        } else {
            if ((tokenin.fileAddr.bank == proposedToken.fileAddr.bank) && (tokenin.fileAddr.file == proposedToken.fileAddr.file)) {
                byte num_rand = random(1, 5); // Genera una respuesta aleatoria buena
                if     (doitPlayer.VOICE_TYPE == WOMAN_VOICE) doitPlayer.play_file(WIN_RESP_M_BANK, num_rand + LANG);
                else if(doitPlayer.VOICE_TYPE == MAN_VOICE)   doitPlayer.play_file(WIN_RESP_H_BANK, num_rand + LANG);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, GREEN));
                delay(RESPONSE_TIME);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, BLACK));
                proposed= false;
            } else {
                byte num_rand = random(1, 5); 
                doitPlayer.play_file(FAIL_RESP_M_BANK, num_rand + LANG);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, RED));
                delay(RESPONSE_TIME);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, BLACK));
                delay(500);
                doitPlayer.play_file(proposedToken.fileAddr.bank, proposedToken.fileAddr.file);
                
            }
        }
    } else if (tokenMode == PARTNER_TOKEN_MODE) {
        if(buscandoPareja){
            bool bankCoincidence = false;
            bool fileCoincidence = false;
            for (int i = 0; i < 8; i++) {
                if (tokenin.fileAddr.bank == proposedToken.partner[i].bank) bankCoincidence = true;
                if (tokenin.fileAddr.file == proposedToken.partner[i].file) fileCoincidence = true;
            }
            if (bankCoincidence && fileCoincidence) {
                byte num_rand = random(1, 5);
                doitPlayer.play_file(WIN_RESP_M_BANK, num_rand + LANG);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, GREEN));
                delay(RESPONSE_TIME);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, BLACK));
                buscandoPareja= false;
            } else {
                byte num_rand = random(1, 5);
                doitPlayer.play_file(FAIL_RESP_M_BANK, num_rand + LANG);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, RED));
                delay(RESPONSE_TIME);
                send_frame(frameMaker_SEND_COLOR(globalID, targets, BLACK));
                buscandoPareja= true;
            }
        }
        else{
            proposedToken.fileAddr.bank= tokenin.fileAddr.bank;
            proposedToken.fileAddr.file= tokenin.fileAddr.file;
            buscandoPareja= true;
        }
    }

    //lastToken = tokenin.fileAddr; // Actualiza el último token procesado OJO
}
