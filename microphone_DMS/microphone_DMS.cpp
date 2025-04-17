
#include <defines_DMS/defines_DMS.h>
#include <microphone_DMS/microphone_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <driver/i2s.h>




extern bool BCframe;
extern float varaux;
extern CRGB* leds;
extern byte numColorRec;

// Declaración de variable interna para rastrear el estado del driver I2S
static bool micInitialized = false;

void MICROPHONE_::begin(){
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };

    if (!micInitialized) {
        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
        i2s_set_pin(I2S_NUM_0, &pin_config);
        micInitialized = true;
        //Serial.println("Micrófono activado.");
    }
    else {
        Serial.println("Micrófono ya estaba activado.");
    }
}



// byte MICROPHONE_::get_mic_value_BYTE(){
//     int32_t sample = 0;
//     size_t bytes_read = 0;

//     // Leer una muestra del micrófono
//     i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, portMAX_DELAY);

//     if (bytes_read > 0) {
//         sample >>= 14; // Ajusta el valor a 16 bits
//         sample = abs(sample); // Tomar el valor absoluto
//         sample = map(sample, 0, 5000, 0, 255); // Mapear a un rango de 0 a 255
//         sample = constrain(sample, 0, 255); // Asegurarse de que esté en el rango 0-255
//     }

//     return (byte)sample; // Devolver el valor mapeado
// }

int MICROPHONE_::readMicRaw() {
    int32_t sample = 0;
    size_t bytes_read = 0;
    // Leer una muestra del micrófono
    i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, portMAX_DELAY);
    if (bytes_read > 0) {
        sample >>= 14;      // Ajusta el valor (según el hardware, aquí se adapta a 16 bits)
        sample = abs(sample); // Valor absoluto
        return sample;
    }
    return 0;
}

void MICROPHONE_::calibracionInicial(unsigned long duracionCalibracion) {
    unsigned long startTime = millis();
    int rawMin = 4097;  // Valor inicial mayor al máximo posible del ADC (4096)
    int rawMax = 0;
    long suma = 0;
    int lectura = 0;
    int contador = 0;
    
    Serial.println("Iniciando calibración...");
    
    while (millis() - startTime < duracionCalibracion) {
        lectura = readMicRaw();
        suma += lectura;
        contador++;
        
        if (lectura < rawMin) {
            rawMin = lectura;
        }
        if (lectura > rawMax) {
            rawMax = lectura;
        }
        delay(10); // Intervalo entre lecturas (ajusta según la frecuencia deseada)
    }
    
    float promedio = (contador > 0) ? (float)suma / contador : 0;
    
    Serial.println("Calibración finalizada:");
    Serial.print("Valor mínimo: ");
    Serial.println(rawMin);
    Serial.print("Valor máximo: ");
    Serial.println(rawMax);
    Serial.print("Valor promedio: ");
    Serial.println(promedio);
}

byte MICROPHONE_::get_mic_value_BYTE(int sens){
    // Inicializamos raw_min con un valor superior al máximo posible del ADC (4096) 
    // y raw_max en 0. 
    // Si reinicias el sistema (o llamas a una función de reinicio) entre pruebas, 
    // estos valores se reiniciarán.
    static int raw_min = 4097;  
    static int raw_max = 0;      

    int32_t sample = 0;
    size_t bytes_read = 0;

    // Leer una muestra del micrófono
    i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, portMAX_DELAY);

    if (bytes_read > 0) {
        sample >>= 14;           // Ajusta el valor (según tu hardware)
        sample = abs(sample);      // Se toma el valor absoluto
        
        // Actualizar el mínimo y el máximo "crudos"
        if(sample < raw_min){
            raw_min = sample;
        }
        if(sample > raw_max){
            raw_max = sample;
        }

        // Mapear el valor del ADC (de 0 a 4096) a un rango de 0 a 255
        sample = map(sample, 0, 5000 - sens, 0, 255);
        sample = constrain(sample, 0, 255);  // Asegurarse que esté en el rango 0-255
    }
    Serial.println("raw_min: " + String(raw_min) + ", raw_max: " + String(raw_max) + ", sample: " + String(sample));
    return (byte)sample; // Devolver el valor mapeado
}


byte MICROPHONE_::get_mic_value_BYTE_voice() {
    static double vReal[SAMPLES];
    static double vImag[SAMPLES];
    static unsigned long lastSampleTime = 0;
    static int sampleIndex = 0;

    unsigned long currentTime = micros();
    if (currentTime - lastSampleTime >= 1000000 / SAMPLING_FREQUENCY) {
        lastSampleTime = currentTime;

        int32_t sample = 0;
        size_t bytes_read = 0;
        i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, 0);

        if (bytes_read > 0) {
            sample >>= 14; // Ajusta el valor a 16 bits
            vReal[sampleIndex] = (double)sample;
            vImag[sampleIndex] = 0;
            sampleIndex++;

            if (sampleIndex == SAMPLES) {
                // Realizar FFT
                FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
                FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
                FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

                // Calcular la energía en la banda de voz
                double voiceEnergy = 0;
                int lowerBin = (300 * SAMPLES) / SAMPLING_FREQUENCY;
                int upperBin = (3400 * SAMPLES) / SAMPLING_FREQUENCY;
                for (int i = lowerBin; i <= upperBin; i++) {
                    voiceEnergy += vReal[i];
                }

                // Normalizar y mapear a byte
                voiceEnergy /= (upperBin - lowerBin + 1);
                int mappedEnergy = map(voiceEnergy, 0, 5000, 0, 255);
                mappedEnergy = constrain(mappedEnergy, 0, 255);

                sampleIndex = 0;
                return (byte)mappedEnergy;
            }
        }
    }

    return 0; // Retornar 0 si no hay nuevo valor
}

bool MICROPHONE_::detect_sound_threshold() {
    const int THRESHOLD = 320; // Ajusta este valor según tus necesidades
    int32_t sample = 0;
    size_t bytes_read = 0;

    // Leer una muestra del micrófono
    i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, portMAX_DELAY);

    if (bytes_read > 0) {
        sample >>= 14; // Ajusta el valor a 16 bits
        sample = abs(sample); // Tomar el valor absoluto
                                                                #ifdef DEBUG
                                                                  //  Serial.println("Detectado sonido, Umbral=" +String(THRESHOLD));
                                                                #endif
        return sample > THRESHOLD;
    }

    return false; // Si no se leyó ninguna muestra, devolver false
}


byte MICROPHONE_::detect_musical_note() {
    // Usamos 1024 muestras para mejorar la resolución en frecuencia:
    static const int SAMPLES_det = 1024;
    static const float SAMPLING_FREQUENCY_det = 44100.0;
    
    // Notas base: DO, RE, MI, FA, SOL, LA, SI (en Hz)
    static const float noteFrequencies[7] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88};

    // Buffers para el FFT
    static double vReal[SAMPLES_det];
    static double vImag[SAMPLES_det];

    unsigned long startTime = micros();
    int maxSample = 0;
    int validCount = 0;
    
    // Recolectar muestras desde el I2S
    for (int i = 0; i < SAMPLES_det; i++) {
        int32_t sample = 0;
        size_t bytes_read = 0;
        i2s_read(I2S_NUM_0, &sample, sizeof(int32_t), &bytes_read, 0);
        
        if (bytes_read == sizeof(int32_t)) {
            // Ajustar la resolución según el formato del IMP441:
            sample >>= 14;         // Ajusta el valor a 16 bits
            sample -= 2048;        // Centrar la señal alrededor de 0
            vReal[i] = sample;
            vImag[i] = 0;
            if (abs(sample) > maxSample) {
                maxSample = abs(sample);
            }
            validCount++;
        } else {
            vReal[i] = 0;
            vImag[i] = 0;
        }
    }
    
    unsigned long endTime = micros();
    unsigned long deltaTime = endTime - startTime;
    // Se utiliza la frecuencia nominal (44100 Hz) porque el I2S trabaja con DMA.
    float actualSamplingFreq = SAMPLING_FREQUENCY_det;
    
    // Depuración: datos de adquisición
                                                                                #ifdef DEBUG
                                                                                Serial.printf("DEBUG: validCount=%d, maxSample=%d, deltaTime=%lu, samplingFreq=%.2f\n", 
                                                                                            validCount, maxSample, deltaTime, actualSamplingFreq);
                                                                                #endif
    
    if (maxSample <= 200) {
                                                                                #ifdef DEBUG
                                                                                Serial.println("DEBUG: maxSample demasiado bajo");
                                                                                #endif
        return 0;
    }
    
    // Aplicar ventana y FFT
    FFT.Windowing(vReal, SAMPLES_det, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES_det, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES_det);
    
    // Buscar el pico máximo (ignorando la componente DC)
    double maxPeak = 0;
    int maxIndex = 0;
    for (int i = 2; i < SAMPLES_det / 2; i++) {
        if (vReal[i] > maxPeak) {
            maxPeak = vReal[i];
            maxIndex = i;
        }
    }
                                                                            #ifdef DEBUG
                                                                            Serial.printf("DEBUG: maxPeak=%.2f, maxIndex=%d\n", maxPeak, maxIndex);
                                                                            #endif
    if (maxPeak <= 200) {
                                                                            #ifdef DEBUG
                                                                            Serial.println("DEBUG: maxPeak demasiado bajo");
                                                                            #endif
        return 0;
    }
    
    // --- Nuevo filtro para favorecer el pico fundamental ---
    // Se busca, entre los índices menores que el pico máximo, el primero que tenga
    // al menos el 70% de la amplitud del pico máximo.
    double threshold = 0.7 * maxPeak;
    int candidateIndex = maxIndex;
    for (int i = 2; i < maxIndex; i++) {
        if (vReal[i] > threshold) {
            candidateIndex = i;
            break;
        }
    }
    if (candidateIndex != maxIndex) {
                                                                                                #ifdef DEBUG
                                                                                                Serial.printf("DEBUG: Pico fundamental detectado en index=%d en lugar de index=%d\n", candidateIndex, maxIndex);
                                                                                                #endif
        maxIndex = candidateIndex;
    }
    // ---------------------------------------------------------
    
    // Interpolación parabólica para mejorar la estimación:
    float interpIndex = maxIndex;
    if (maxIndex > 0 && maxIndex < (SAMPLES_det / 2 - 1)) {
        double alpha = vReal[maxIndex - 1];
        double beta  = vReal[maxIndex];
        double gamma = vReal[maxIndex + 1];
        float delta = (alpha - gamma) / (2.0 * (alpha - 2.0 * beta + gamma));
        interpIndex = maxIndex + delta;
                                                                                                #ifdef DEBUG
                                                                                                Serial.printf("DEBUG: Interpolación delta=%.2f, interpIndex=%.2f\n", delta, interpIndex);
                                                                                                #endif
    }
    
    // Calcular la frecuencia usando el índice interpolado:
    float freq = interpIndex * actualSamplingFreq / SAMPLES_det;
                                                                                                #ifdef DEBUG
                                                                                                Serial.printf("DEBUG: Frecuencia detectada: %.2f Hz\n", freq);
                                                                                                #endif

    // Filtro: ignorar frecuencias fuera del rango de interés (100 Hz a 10 kHz)
    if (freq < 100 || freq > 10000) {
                                                                                                #ifdef DEBUG
                                                                                                Serial.println("DEBUG: Frecuencia fuera del rango de interés (100Hz - 10kHz)");
                                                                                                #endif
        return 0;
    }
    
    // Normalizar la frecuencia a la octava base (entre DO y DO*2)
    double normFreq = freq;
    while (normFreq < noteFrequencies[0]) {
        normFreq *= 2;
    }
    while (normFreq >= noteFrequencies[0] * 2) {
        normFreq /= 2;
    }
                                                                                                #ifdef DEBUG
                                                                                                Serial.printf("DEBUG: Frecuencia normalizada: %.2f\n", normFreq);                                                                               
                                                                                                #endif
    
    // Determinar la nota comparando con las frecuencias base:
    int nota = 0;
    double minDiff = fabs(normFreq - noteFrequencies[0]);
    for (int i = 1; i < 7; i++) {
        double diff = fabs(normFreq - noteFrequencies[i]);
        if (diff < minDiff) {
            minDiff = diff;
            nota = i;
        }
    }
                                                                                                #ifdef DEBUG
                                                                                                Serial.printf("DEBUG: Nota determinada (índice): %d, frecuencia base: %.2f Hz\n", nota, noteFrequencies[nota]);                                                                               
                                                                                                #endif
    
    
    // Se retorna un número entre 1 y 7 (1: DO, 2: RE, …, 7: SI)
    return nota + 1;
}

void MICROPHONE_::end(){
    // Desinstalar el driver solo si estaba instalado
    if (micInitialized) {
        i2s_driver_uninstall(I2S_NUM_0);
        micInitialized = false;
        Serial.println("Micrófono desactivado.");
    }
    else {
        Serial.println("Micrófono end() llamado, pero I2S no estaba instalado.");
    }
}