#ifndef TOKEN_DMS_H
#define TOKEN_DMS_H

#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <NfcAdapter.h>
#include <NdefMessage.h>
#include <NdefRecord.h>
#include <Frame_DMS/Frame_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Translations_handler/translations.h>
#include <vector>

// Enumeración de modos de operación del token
enum TOKEN_MODE_ {
    TOKEN_BASIC_MODE = 0,
    TOKEN_PARTNER_MODE,
    TOKEN_GUESS_MODE
};

class TOKEN_ {
public:
    // Estructuras para almacenar la información del token
    struct TOKEN_ADDR {
        byte bank;
        byte file;
    };

    struct TOKEN_COLOR {
        byte r;
        byte g;
        byte b;
    };

    struct TOKEN_DATA {
        byte cmd;
        byte cmd2;
        TOKEN_ADDR addr;
        TOKEN_COLOR color;
        TOKEN_ADDR partner[8];
    };

    // Constructor
    TOKEN_();

    // Inicializa el bus I2C y configura el PN532 (a través de NfcAdapter)
    void begin();

    // Lee el UID de la tarjeta. Retorna true si se detecta tarjeta y asigna el UID a 'uid'
    bool readCard(String &uid);

    // Lee y procesa el mensaje NDEF de la tarjeta y retorna el token (cadena delimitada por '#')
    // Si no se encuentra un mensaje válido, retorna cadena vacía.
    bool leerMensajeNFC(String& mensaje);

    // Función de compatibilidad: lectura de página (no utilizada en la nueva lógica)
    bool leerPagina(uint8_t pagina, uint8_t *buffer);

    // Funciones ya existentes para la lógica de la aplicación (se adaptan según se requiera)
    void set_mode(TOKEN_MODE_ modein) { tokenCurrentMode = modein; }
    void set_reset_partner(bool state) { reset_partner = state; }
    void token_handler(TOKEN_DATA token, uint8_t lang, bool genre, uint8_t myid, std::vector<uint8_t> targets);
    void proponer_token(byte guessbank);
    void printFicha(const TOKEN_::TOKEN_DATA &f);
    // Función auxiliar para convertir dos caracteres ASCII hex a un byte
    byte asciiHexToByte(char high, char low) ;
    // Función auxiliar para parsear la cadena del token y convertirla en TOKEN_DATA
    TOKEN_DATA parseTokenString(const String &tokenStr);
    // Variables públicas para facilitar el control en el loop
    String currentUID;
    String lastProcessedUID;

    // Parámetros de audio u otros (a adaptar según tu lógica)
    byte genre;
    byte lang;

    // Datos del token actual y el token propuesto (para modos de pareja/guess)
    TOKEN_DATA currentToken;
    TOKEN_DATA propossedToken;
    TOKEN_MODE_ tokenCurrentMode = TOKEN_BASIC_MODE;

private:
    // Variables internas para controlar el estado
    
    bool waitingForPartner = false;
    bool reset_partner = false;
    unsigned long lastReadAttempt;
    unsigned long readInterval;

    // Función auxiliar para decodificar el texto de un registro NDEF (tipo Text)
    String decodeNdefText(const byte* payload, int payloadLength);
};

extern DOITSOUNDS_ doitPlayer;

#endif // TOKEN_DMS_H
