#ifndef TOKEN_DMS_H
#define TOKEN_DMS_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <vector>


class TOKEN_ {
private:

    enum TOKEN_MODES{

        BASIC_TOKEN_MODE,
        GUESS_TOKEN_MODE,
        PARTNER_TOKEN_MODE
    };
    struct TOKEN_FILE_ADDR {
        byte file;
        byte bank;
    };

    struct TOKEN_INFO {
        TOKEN_FILE_ADDR fileAddr;
        byte color;
        uint16_t timeColor;
        byte command;
        TOKEN_FILE_ADDR partner[8];
    };

    std::vector<byte> vctToken;
    TOKEN_INFO proposedToken = {0, 0}; // Token propuesto
    bool proposed = false;
    bool buscandoPareja= false;

public:
    TOKEN_() {}
    ~TOKEN_() {}

    void begin();
    bool isCardPresent();
    TOKEN_INFO get_token(std::vector<byte> tkn);
    void proponer_token();
    void token_action(std::vector<byte> targets, TOKEN_INFO tokenin, byte LANG, byte tokenMode);
};


#endif