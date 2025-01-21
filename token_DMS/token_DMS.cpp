#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

    
void TOKEN_::begin() {
    I2C2.begin(40, 41); // Pines SDA y SCL para el bus I2C
    Serial.println("Escaneando dispositivos I2C...");

    bool dispositivoEncontrado = false;
    for (uint8_t address = 1; address < 127; address++) {
        I2C2.beginTransmission(address);
        if (I2C2.endTransmission() == 0) {
            Serial.print("Dispositivo I2C encontrado en dirección: 0x");
            Serial.println(address, HEX);
            dispositivoEncontrado = true;
        }
    }
    if (!dispositivoEncontrado) {
        Serial.println("Error: No se encontraron dispositivos en el bus I2C. Verifica las conexiones y el cableado.");
    }

    // Inicializa el lector NFC
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("Error: No se encontró el lector PN53x. Verifica las conexiones o el módulo.");
        while (1); // Detener el programa
    }

    // Muestra información sobre el firmware del PN532
    Serial.print("Encontrado chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware: v");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print(".");
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // Configura el módulo NFC
    Serial.println("Configurando SAM...");
    nfc.SAMConfig();
    Serial.println("Configuración SAM completada.");
    Serial.println("El lector NFC está listo. Esperando un tag NFC...");
}

bool TOKEN_::readCard(uint8_t *uid, uint8_t &uidLength) {
    // Limitar la frecuencia de intentos de lectura
    if (millis() - lastReadAttempt < readInterval) {
        return false; // No realizar lectura todavía
    }
    lastReadAttempt = millis(); // Actualizar el tiempo del último intento

    // Intentar leer una tarjeta
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.println("¡Tarjeta detectada!");
        Serial.print("UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");
        Serial.print("UID Value: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(" 0x");
            Serial.print(uid[i], HEX);
        }
        Serial.println("");
        return true; // Se detectó una tarjeta
    }

    // No se detectó ninguna tarjeta
    return false;
}
