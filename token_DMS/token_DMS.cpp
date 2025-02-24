#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>

// Librerías para el PN532 (NO Adafruit)
#include <Wire.h>


extern DOITSOUNDS_ doitPlayer;

#include "PN532_I2C.h"
#include "PN532.h"

// ========== DECLARACIÓN DE OBJETOS Y VARIABLES GLOBALES ==========
// Suponiendo que tu ESP32-S3 usa el bus I2C número 1 en pines SDA=40, SCL=41
TwoWire I2C2(1);

// Instancia de la clase PN532_I2C, usando el bus I2C2
PN532_I2C pn532i2c(I2C2);

// Instancia de la clase PN532 (core), que recibe la instancia anterior
PN532 nfc(pn532i2c);

// ========== IMPLEMENTACIÓN DE LA CLASE TOKEN_ ==========

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
    // 1) Inicializa I2C en pines 40 (SDA) y 41 (SCL)
    I2C2.begin(40, 41);

    bool dispositivoEncontrado = false;
    for (uint8_t address = 1; address < 127; address++) {
        I2C2.beginTransmission(address);
        if (I2C2.endTransmission() == 0) {
            dispositivoEncontrado = true;
                                                                                                        #ifdef DEBUG
                                                                                                        Serial.print("Dispositivo I2C encontrado en dirección: 0x");
                                                                                                        Serial.println(address, HEX);                                                                          
                                                                                                        #endif
        }
    }
    
    if (!dispositivoEncontrado) {
                                                                                                        #ifdef DEBUG
                                                                                                        Serial.println("Error: No se encontraron dispositivos en el bus I2C. Verifica las conexiones y el cableado.");                                                               
                                                                                                        #endif
        // Aquí podrías hacer un return o continuar igualmente;
        // depende de la lógica que quieras manejar si no encuentra nada
    }

    // 2) Inicializa el lector NFC
    nfc.begin();
    delay(100); // Pequeña pausa para la configuración interna

    // 3) Verifica el firmware del PN532
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
                                                                                                            #ifdef DEBUG
                                                                                                                    Serial.println("Error: No se detectó el PN532. Verifica conexiones o módulo.");                                                              
                                                                                                            #endif
        while (1) {
            // Bucle infinito para detener el programa; o bien usa return
        }
    }

                                                                                                            #ifdef DEBUG
                                                                                                            Serial.print("Encontrado chip PN5 ");
                                                                                                            Serial.println((versiondata >> 24) & 0xFF, HEX);
                                                                                                            Serial.print("Firmware: v");
                                                                                                            Serial.print((versiondata >> 16) & 0xFF, DEC);
                                                                                                            Serial.print(".");
                                                                                                            Serial.println((versiondata >> 8) & 0xFF, DEC);                                                                  
                                                                                                            Serial.println("El lector NFC está listo. Esperando un tag NFC...");
                                                                                                            #endif
    // 4) Imprime la versión
    // 5) Configura el PN532 (SAMConfig)
    nfc.SAMConfig();
}

bool TOKEN_::readCard(uint8_t *uid, uint8_t &uidLength) {
    // Limitar la frecuencia de intentos de lectura para no saturar
    if (millis() - lastReadAttempt < readInterval) {
        return false; // No realizar lectura todavía
    }
    lastReadAttempt = millis(); // Actualizar el tiempo del último intento

    // 6) Intentar leer tarjeta
    //    Opcional: se puede especificar un timeout si la librería lo soporta:
    //    nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
    //    Si tu librería no lo admite, la llamada puede ser bloqueante.
    //    Usamos la forma básica:
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1)) {
                                                                                                            #ifdef DEBUG
                                                                                                            Serial.print("UID Length: ");
                                                                                                            Serial.print(uidLength, DEC);
                                                                                                            Serial.println(" bytes");
                                                                                                            Serial.print("UID Value: ");
                                                                                                            for (uint8_t i = 0; i < uidLength; i++) {
                                                                                                                Serial.print(" 0x");
                                                                                                                Serial.print(uid[i], HEX);
                                                                                                            }
                                                                                                            Serial.println("");                                                              
                                                                                                            #endif
        return true; // Se detectó una tarjeta
    }

    //lastToken = tokenin.fileAddr; // Actualiza el último token procesado OJO
    // No se detectó ninguna tarjeta en este intento
    return false;
}