#include <ADXL345_handler/ADXL345_handler.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <encoder_handler/encoder_handler.h>


ADXL345Handler adxl345Handler;

// Constructor
ADXL345Handler::ADXL345Handler() 
    : accel(Adafruit_ADXL345_Unified(12345)), /*lastInclination(0.0),*/ threshold(0.4), thresholdBinary(0.7), initialized(false) {}

// Inicialización del ADXL345
void ADXL345Handler::init()
{
    Wire.begin(SDA_ADXL_PIN, SCL_ADXL_PIN);
    Wire.setClock(100000); // Reducir la velocidad I2C para mayor estabilidad
    const int maxAttempts = 3;
    int attempt = 0;
    bool detected = false;
    while (attempt < maxAttempts)
    {
        if (accel.begin())
        {
            detected = true;
            break;
        }
        else
        {
            DEBUG__________ln("ADXL345 no detectado, reintentando...");
            delay(500); // Espera antes del siguiente intento
        }
        attempt++;
    }

    if (!detected)
    {
        DEBUG__________ln("ADXL345 no detectado");
        initialized = false;
        return;
    }

    accel.setRange(ADXL345_RANGE_16_G);
    initialized = true;
    DEBUG__________ln("ADXL345 inicializado correctamente");
}

// Función para convertir la inclinación en un valor escalado
long ADXL345Handler::convertInclinationToValue(float inclination) {
    long scaledInclination = inclination * 100;
    return map(scaledInclination, -1000, 1000, 0, 2000);
}

// Función para empaquetar el valor en SENSOR_VALUE_T
// SENSOR_VALUE_T ADXL345Handler::createSensorValue(long finalValue) {
//     SENSOR_VALUE_T sensorin;
//     sensorin.lsb_min = 0x00;
//     sensorin.msb_min = 0x00;
//     sensorin.msb_max = 0x07;
//     sensorin.lsb_max = 0xD0;
//     sensorin.msb_val = (byte)(finalValue >> 8);
//     sensorin.lsb_val = (byte)(finalValue & 0xFF);
//     return sensorin;
// }


// Verificar si el ADXL345 está inicializado
// bool ADXL345Handler::isInitialized() const {
//     return initialized;
// }

// Establecer un nuevo umbral
// void ADXL345Handler::setThreshold(float newThreshold) {
//     threshold = newThreshold;
// }

void ADXL345Handler::end()
{
    if (!initialized)
    {
                                                                    #ifdef DEBUG
                                                                    DEBUG__________ln("ADXL345Handler::end() llamado, pero ya estaba desactivado.");
                                                                    #endif 
        return;
    }

    Wire.end();
    initialized = false;
                                                                    #ifdef DEBUG 
                                                                    DEBUG__________ln("I2C desactivado (acelerómetro end).");
                                                                    #endif
}

void ADXL345Handler::readInclinations() {
    if (!initialized || !adxl) return;

    // 1) Leemos siempre, pero procesamos sólo cada movementSampleInterval
    unsigned long now = millis();
    if (now - lastSampleTime < movementSampleInterval) return;
    lastSampleTime = now;

    sensors_event_t event;
    if (!accel.getEvent(&event)) return;

    float currX = event.acceleration.x;
    float currY = event.acceleration.y;

    // 2) Obtenemos el flag binario
    byte modeConfig[2] = {0};
    getModeConfig(elementFiles[currentIndex], currentModeIndex, modeConfig);
    bool isBinary = getModeFlag(modeConfig, HAS_BINARY_SENSORS);

    // 3) Calculamos la diferencia
    float diffX = abs(currX - lastSampleX);
    float diffY = abs(currY - lastSampleY);

    // DEBUG__________printf(
    //   "currX=%.2f, lastSampleX=%.2f, diffX=%.2f | currY=%.2f, lastSampleY=%.2f, diffY=%.2f\n",
    //   currX, lastSampleX, diffX,
    //   currY, lastSampleY, diffY
    // );

    // 4) Si es binario, gestionamos ON inmediato y OFF tras inactivityTimeout
    if (isBinary) {
        bool movementDetected = (diffX >= thresholdBinary ||
                                 diffY >= thresholdBinary);

        // Inicio de movimiento → enviar ‘1’ de inmediato
        if (movementDetected && !movementDetectedLast) {
        TARGETNS ns = getCurrentElementNS();
        SENSOR_DOUBLE_T binVal = {0,0, 0,1, 0,1, 0,0, 0,0, 0,0};;
        send_frame(frameMaker_SEND_SENSOR_VALUE(
        DEFAULT_BOTONERA,     // origin = la botonera
        DEFAULT_DEVICE,       // targetType = un dispositivo normal
        ns,                  // el número de serie del elemento actual
        binVal
        ));

            movementDetectedLast = true;
        }

        // Cada vez que haya movimiento, actualizamos el timestamp
        if (movementDetected) {
            lastMovementTime = now;
        }

        // Si llevamos inactivityTimeout sin movimiento → enviar ‘0’
        if (movementDetectedLast &&
            (now - lastMovementTime >= inactivityTimeout)) {
            TARGETNS ns = getCurrentElementNS();
            SENSOR_DOUBLE_T binVal = {0,0, 0,1, 0,0, 0,0, 0,0, 0,0};  // min=0,max=1,val=0
            send_frame(frameMaker_SEND_SENSOR_VALUE(
            DEFAULT_BOTONERA,     // origin = la botonera
            DEFAULT_DEVICE,       // targetType = un dispositivo normal
            ns,                  // el número de serie del elemento actual
            binVal
            ));
            movementDetectedLast = false;
        }
    }
    else {
        // Comportamiento analógico original: sólo cuando supera umbral
        bool movementDetected = (diffX >= threshold || diffY >= threshold);
        if (movementDetected) {
            // Limita y convierte igual que antes
            currX = constrain(currX, -10, 10);
            currY = constrain(currY, -10, 10);
            long valX = convertInclinationToValue(currX);
            long valY = convertInclinationToValue(currY);
            SENSOR_DOUBLE_T sensorVal = createSensorDoubleValue(valX, valY);
            sendSensorValueDouble(sensorVal);
            //lastInclinationX = currX;
            //lastInclinationY = currY;
        }
    }
    // Actualizar referencia para siguiente comparación
    lastSampleX = currX;
    lastSampleY = currY;
}

SENSOR_DOUBLE_T ADXL345Handler::createSensorDoubleValue(long finalValueX, long finalValueY) {
    SENSOR_DOUBLE_T s;
    // Primer eje
    s.msb_min  = 0x00; s.lsb_min  = 0x00;
    s.msb_max  = 0x07; s.lsb_max  = 0xD0;
    s.msb_val  = (byte)(finalValueX >> 8);
    s.lsb_val  = (byte)(finalValueX & 0xFF);
    // Segundo eje
    s.msb_min2 = 0x00;   s.lsb_min2 = 0x00;
    s.msb_max2 = 0x07;   s.lsb_max2 = 0xD0;
    s.msb_val2 = (byte)(finalValueY >> 8);
    s.lsb_val2 = (byte)(finalValueY & 0xFF);
    return s;
}

void ADXL345Handler::sendSensorValueDouble(const SENSOR_DOUBLE_T &sensorValue) {
    TARGETNS ns = getCurrentElementNS();

    // Verificar que el número de serie no sea nulo (ej. {0,0,0,0,0})
    bool isValid = false;
    for (int i = 0; i < 5; i++) {
        if (((uint8_t*)&ns)[i] != 0) { 
            isValid = true; 
            break; 
        }
    }

    if (!isValid) {
        #ifdef DEBUG
                DEBUG__________ln("⚠️ No se pudo obtener el número de serie del elemento. Trama no enviada.");
        #endif
        return;
    }

    send_frame(frameMaker_SEND_SENSOR_VALUE(
        DEFAULT_BOTONERA,   // origen = botonera
        DEFAULT_DEVICE,     // destino = un dispositivo normal
        ns,                // número de serie del elemento
        sensorValue
    ));
}

// ====== Configuración de INT de ACTIVIDAD para wake (INT1) ======
static inline void adxl_write_u8(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(0x53); // ADXL345 por defecto
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

static inline uint8_t adxl_read_u8(uint8_t reg) {
  Wire.beginTransmission(0x53);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)0x53, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0;
}

// Registros
#define ADXL_REG_THRESH_ACT     0x24
#define ADXL_REG_ACT_INACT_CTL  0x27
#define ADXL_REG_BW_RATE        0x2C
#define ADXL_REG_POWER_CTL      0x2D
#define ADXL_REG_INT_ENABLE     0x2E
#define ADXL_REG_INT_MAP        0x2F
#define ADXL_REG_INT_SOURCE     0x30
#define ADXL_REG_DATA_FORMAT    0x31

void ADXL345Handler::enableActivityInterrupt(uint16_t threshold_mg, bool enX, bool enY, bool enZ) {
  if (!initialized) init();         // asegura I2C y detección
  // Rango ±2g, INT activa en alto (por defecto). Full-res off.
  adxl_write_u8(ADXL_REG_DATA_FORMAT, 0x00);
  // Frecuencia moderada (25–50 Hz) para menos ruido y consumo. 0x08 = 25 Hz, 0x0A = 100 Hz.
  adxl_write_u8(ADXL_REG_BW_RATE, 0x08); // 25 Hz

  // Umbral de actividad (62.5 mg/LSB): satura 1..255
  uint16_t lsbs = (threshold_mg + 31) / 62; // redondeo aproximado
  if (lsbs < 1) lsbs = 1;
  if (lsbs > 255) lsbs = 255;
  adxl_write_u8(ADXL_REG_THRESH_ACT, (uint8_t)lsbs);

  // ACT_INACT_CTL: ACT_AC=1 (acoplamiento AC para “movimiento”), X/Y/Z según flags
  uint8_t act = 0;
  if (enX) act |= (1 << 6);
  if (enY) act |= (1 << 5);
  if (enZ) act |= (1 << 4);
  act |= (1 << 7); // ACT_AC=1
  adxl_write_u8(ADXL_REG_ACT_INACT_CTL, act);

  // Mapear ACTIVIDAD (bit4) a INT1 (bit=0 → INT1)
  uint8_t int_map = adxl_read_u8(ADXL_REG_INT_MAP);
  int_map &= ~(1 << 4);
  adxl_write_u8(ADXL_REG_INT_MAP, int_map);

  // Habilitar INT de ACTIVIDAD (bit4)
  uint8_t int_en = adxl_read_u8(ADXL_REG_INT_ENABLE);
  int_en |= (1 << 4);
  adxl_write_u8(ADXL_REG_INT_ENABLE, int_en);

  // Modo Measure
  adxl_write_u8(ADXL_REG_POWER_CTL, 0x08);
}

void ADXL345Handler::clearInterrupts() {
  (void)adxl_read_u8(ADXL_REG_INT_SOURCE);  // leer limpia el latch
}




