#ifndef TOKEN_DMS_H
#define TOKEN_DMS_H

#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

// Si no vas a incluir PN532 en el .h (sólo en .cpp), no lo pongas aquí.
// Pero si necesitas usar tipos de PN532 en la declaración de la clase, sí debes incluirlo.
// #include <PN532.h>

// ----------------------------------------------------------------------------
// Clase TOKEN_
// ----------------------------------------------------------------------------
class TOKEN_ {
public:
    // Constructor (opcional si no necesitas lógica especial)
    TOKEN_() : lastReadAttempt(0), readInterval(200) {}

    // Inicializa el bus I2C y el PN532
    void begin();

    // Lee una tarjeta y devuelve true si se detectó, false si no
    bool readCard(uint8_t *uid, uint8_t &uidLength);

private:
    // Variables internas para controlar el intervalo entre lecturas
    unsigned long lastReadAttempt;
    unsigned long readInterval;
};

#endif // TOKEN_DMS_H