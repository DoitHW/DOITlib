#include <ADXL345_handler/ADXL345_handler.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>

ADXL345Handler adxl345Handler;

// Constructor
ADXL345Handler::ADXL345Handler() 
    : accel(Adafruit_ADXL345_Unified(12345)), lastInclination(0.0), threshold(0.5), initialized(false) {}

// Inicialización del ADXL345
void ADXL345Handler::init() {
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!accel.begin()) {
                                                                                    
        Serial.println("ADXL345 no detectado");
                                                        
        initialized = false;
        return;
    }
    accel.setRange(ADXL345_RANGE_16_G);
    initialized = true;
    //Serial.println("ADXL345 inicializado correctamente");
}

// Leer la inclinación del ADXL345
void ADXL345Handler::readInclination(char axis) {
    if (!initialized || !adxl) return; // Si no está inicializado o deshabilitado, no hace nada.

    sensors_event_t event;
    accel.getEvent(&event);
    float currentInclination;

    switch (axis) {
        case 'x':
            currentInclination = event.acceleration.x;
            break;
        case 'y':
            currentInclination = event.acceleration.y;
            break;
        default:
            return;
    }

    if (abs(currentInclination - lastInclination) >= threshold) {
                                                                                                        #ifdef DEBUG
                                                                                                            Serial.print("Inclinación detectada (");
                                                                                                            Serial.print(axis);
                                                                                                            Serial.print("): ");                                                                         
                                                                                                        #endif
       
        
        currentInclination = constrain(currentInclination, -10, 10);
        Serial.println(currentInclination);

        // Convertir y enviar
        long finalValue = convertInclinationToValue(currentInclination);
        SENSOR_VALUE_T sensorValue = createSensorValue(finalValue);
        sendSensorValue(sensorValue);

        lastInclination = currentInclination;
    }
}

// Función para convertir la inclinación en un valor escalado
long ADXL345Handler::convertInclinationToValue(float inclination) {
    long scaledInclination = inclination * 100;
    return map(scaledInclination, -1000, 1000, 0, 2000);
}

// Función para empaquetar el valor en SENSOR_VALUE_T
SENSOR_VALUE_T ADXL345Handler::createSensorValue(long finalValue) {
    SENSOR_VALUE_T sensorin;
    sensorin.lsb_min = 0x00;
    sensorin.msb_min = 0x00;
    sensorin.msb_max = 0x07;
    sensorin.lsb_max = 0xD0;
    sensorin.msb_val = (byte)(finalValue >> 8);
    sensorin.lsb_val = (byte)(finalValue & 0xFF);
    return sensorin;
}

// Función para enviar la trama
void ADXL345Handler::sendSensorValue(const SENSOR_VALUE_T &sensorValue) {
    std::vector<byte> targets;
    
    // Obtener la ID del elemento actualmente mostrado en pantalla
    byte currentElementID = getCurrentElementID();
    
    if (currentElementID == 0xFF) {
                                                                                        #ifdef DEBUG
                                                                                            Serial.println("⚠️ Advertencia: No se pudo obtener la ID del elemento actual. No se enviará la trama.");                                                                            
                                                                                        #endif
        return;
    }

    // Usar la ID obtenida en lugar de 0xFF
    targets.push_back(currentElementID);

    send_frame(frameMaker_SEND_SENSOR_VALUE(DEFAULT_BOTONERA, targets, sensorValue));
   
}



// Verificar si el ADXL345 está inicializado
bool ADXL345Handler::isInitialized() const {
    return initialized;
}

// Establecer un nuevo umbral
void ADXL345Handler::setThreshold(float newThreshold) {
    threshold = newThreshold;
}

void ADXL345Handler::end() {
    Wire.end();
    initialized = false;
                                                                                        #ifdef DEBUG
                                                                                        Serial.println("I2C desactivado (acelerómetro end).");                                                                               
                                                                                        #endif
}