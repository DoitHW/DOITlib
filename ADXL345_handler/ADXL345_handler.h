#pragma once
#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Frame_DMS/Frame_DMS.h>

// Definición de pines I2C
#define SDA_ADXL_PIN 47
#define SCL_ADXL_PIN 48

// Clase para manejar el acelerómetro ADXL345
class ADXL345Handler {
public:
    ADXL345Handler();                  // Constructor
    void init();                      // Inicializar el acelerómetro
    void sendSensorValue(const SENSOR_VALUE_T &sensorValue);
    long convertInclinationToValue(float inclination);
    SENSOR_VALUE_T createSensorValue(long finalValue);
    bool isInitialized() const;       // Verificar si está inicializado
    void setThreshold(float newThreshold);  // Configurar un nuevo umbral
    void end();
    void readInclinations();
    SENSOR_DOUBLE_T createSensorDoubleValue(long finalValueX, long finalValueY);
    void sendSensorValueDouble(const SENSOR_DOUBLE_T &sensorValue);

private:
    Adafruit_ADXL345_Unified accel;
    float lastInclination;  // Última inclinación leída
    float lastInclinationX;
    float lastInclinationY;
    float threshold;        // Umbral de cambio significativo
    float thresholdBinary;  // Umbral para sensor binario
    bool initialized;       // Estado de inicialización
    int errorCount;
    static const unsigned long movementSampleInterval = 50;  // muestreo rápido
    static const unsigned long inactivityTimeout     = 1000; // 1 s sin movimiento

    unsigned long lastSampleTime    = 0;
    unsigned long lastMovementTime  = 0;
    bool         movementDetectedLast = false;
    float        lastSampleX = 0, lastSampleY = 0;

};

// Declaración de variable global para el control del ADXL345
extern ADXL345Handler adxl345Handler;
extern bool adxl;  // Variable global para habilitar/deshabilitar lecturas
