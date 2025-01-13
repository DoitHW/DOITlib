#pragma once
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Frame_DMS/Frame_DMS.h>

// Definición de pines I2C
#define SDA_PIN 47
#define SCL_PIN 48

// Clase para manejar el acelerómetro ADXL345
class ADXL345Handler {
public:
    ADXL345Handler();                  // Constructor
    void init();                      // Inicializar el acelerómetro
    void readInclination(char axis);           // Leer la inclinación eje x
    void sendSensorValue(const SENSOR_VALUE_T &sensorValue);
    long convertInclinationToValue(float inclination);
    SENSOR_VALUE_T createSensorValue(long finalValue);
    bool isInitialized() const;       // Verificar si está inicializado
    void setThreshold(float newThreshold);  // Configurar un nuevo umbral

private:
    Adafruit_ADXL345_Unified accel;
    float lastInclination;  // Última inclinación leída
    float threshold;        // Umbral de cambio significativo
    bool initialized;       // Estado de inicialización
};

// Declaración de variable global para el control del ADXL345
extern ADXL345Handler adxl345Handler;
extern bool adxl;  // Variable global para habilitar/deshabilitar lecturas
