#ifndef TOKEN_DMS_H
#define TOKEN_DMS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>


class TOKEN_ {
private:
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
    TwoWire I2C2;                      // Instancia de I2C personalizada
    Adafruit_PN532 nfc;
    unsigned long lastReadAttempt;     // Último intento de lectura
    unsigned long readInterval;

public:
    TOKEN_() : I2C2(1), nfc(-1, -1, &I2C2) {}

    ~TOKEN_() {}
    void begin();
    bool readCard(uint8_t *uid, uint8_t &uidLength);

};
#endif