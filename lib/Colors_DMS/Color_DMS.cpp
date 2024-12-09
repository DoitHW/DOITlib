#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <map>
//testing

extern COLORHANDLER_ colorHandler;

bool color_mix_handler(int color1, int color2, byte *resultado) {
 
  static std::map<std::pair<int, int>, int> colorMixMap = {
    {{WHITE, YELLOW},       CREMA},
    {{YELLOW, WHITE},       CREMA},
    {{WHITE, RED},          ROSA},
    {{RED, WHITE},          ROSA},
    {{WHITE, ORANGE},       SALMON},
    {{ORANGE, WHITE},       SALMON},
    {{WHITE, VIOLET},       LILA},
    {{VIOLET, WHITE},       LILA},
    {{WHITE, BLUE},         CELESTE_CLARO},
    {{BLUE, WHITE},         CELESTE_CLARO},
    {{WHITE, LIGHT_BLUE},   TURQUESA},
    {{LIGHT_BLUE, WHITE},   TURQUESA},
    {{WHITE, GREEN},        VERDE_CLARO},
    {{GREEN, WHITE},        VERDE_CLARO},
    {{YELLOW, RED},         NARANJA},
    {{RED, YELLOW},         NARANJA},
    {{YELLOW, VIOLET},      ROSA_OSCURO},
    {{VIOLET, YELLOW},      ROSA_OSCURO},
    {{YELLOW, BLUE},        VERDE_OLIVA},
    {{BLUE, YELLOW},        VERDE_OLIVA},
    {{YELLOW, LIGHT_BLUE},  VERDE_LIMA},
    {{LIGHT_BLUE, YELLOW},  VERDE_LIMA},
    {{YELLOW, GREEN},       VERDE_AMARILLO},
    {{GREEN, YELLOW},       VERDE_AMARILLO},
    {{RED, VIOLET},         MAGENTA},
    {{VIOLET, RED},         MAGENTA},
    {{RED, BLUE},           MORADO},
    {{BLUE, RED},           MORADO},
    {{RED, LIGHT_BLUE},     VERDE_AZULADO},
    {{LIGHT_BLUE, RED},     VERDE_AZULADO},
    {{RED, GREEN},          MARRON},
    {{GREEN, RED},          MARRON},
    {{VIOLET, BLUE},        INDIGO},
    {{BLUE, VIOLET},        INDIGO},
    {{VIOLET, LIGHT_BLUE},  AZUL_VERDOSO},
    {{LIGHT_BLUE, VIOLET},  AZUL_VERDOSO},
    {{VIOLET, GREEN},       ESMERALDA},
    {{GREEN, VIOLET},       ESMERALDA},
    {{BLUE, LIGHT_BLUE},    CYAN},
    {{LIGHT_BLUE, BLUE},    CYAN},
    {{BLUE, GREEN},         VERDE_AZULADO},
    {{GREEN, BLUE},         VERDE_AZULADO},
    {{LIGHT_BLUE, GREEN},   VERDE_MENTA},
    {{GREEN, LIGHT_BLUE},   VERDE_MENTA},
    {{ORANGE, YELLOW},      AMARILLO_ANARANJADO},
    {{YELLOW, ORANGE},      AMARILLO_ANARANJADO},
    {{ORANGE, RED},         ROJO_ANARANJADO},
    {{RED, ORANGE},         ROJO_ANARANJADO},
    {{ORANGE, VIOLET},      ROSA_FUERTE},
    {{VIOLET, ORANGE},      ROSA_FUERTE},
    {{ORANGE, LIGHT_BLUE},  VERDE_GRISACEO},
    {{LIGHT_BLUE, ORANGE},  VERDE_GRISACEO},
    {{ORANGE, BLUE},        MARRON_OSCURO},
    {{BLUE, ORANGE},        MARRON_OSCURO},
    {{ORANGE, GREEN},       VERDE_OLIVA_CLARO},
    {{GREEN, ORANGE},       VERDE_OLIVA_CLARO}
 
  };

    auto it = colorMixMap.find({color1, color2});
    if (it != colorMixMap.end()) {
        *resultado = it->second;
        return true;
    } else {
        return false;
    }
}



COLORHANDLER_::COLORHANDLER_()
    : leds(nullptr), numLeds(0),
      startColor(CRGB::Black),
      currentColor(CRGB::Black),
      targetColor(CRGB::Black),
      targetBrightness(0),
      target_transition_time(0),
      current_transition_time(0),
      currentBrightness(0),
      startBrightness(0),
      startTime(0),
      fadeCompleted(true) {}

void COLORHANDLER_::begin(int numLeds) {
    #ifdef COLUMNA
    const byte dataPin = COLUMN_LED_DATA_PIN;
    #endif
    #ifdef FIBRAS
    byte dataPin = LIGHTSOURCE_LED_DATA_PIN;
    #endif
    #ifdef WALLWASHER
    byte dataPin = LEDSTRIP_LED_DATA_PIN;
    #endif
    #ifdef ESCALERA
    byte dataPin = ESCALERA_LED_DATA_PIN;
    #endif
    #ifdef LUZNEGRA
    byte dataPin = BLACKLIGHT_LED_DATA_PIN;
    #endif

    this->numLeds = numLeds;
    this->leds = new (std::nothrow) CRGB[numLeds];
    FastLED.addLeds<WS2811, dataPin, RGB>(leds, numLeds);
    FastLED.setBrightness(255);
}


CRGB COLORHANDLER_::get_start_color(){
    return startColor;
}


void COLORHANDLER_::set_start_color(CRGB colorin){
    startColor= colorin;
}


CRGB COLORHANDLER_::get_current_color(){
    return currentColor;
}


void COLORHANDLER_::set_current_color(CRGB colorin){
    currentColor= colorin;
}


CRGB COLORHANDLER_::get_target_color(){
    return targetColor;
}


void COLORHANDLER_::set_target_color(CRGB colorin){
    targetColor= colorin;
}


CRGB COLORHANDLER_::get_color(byte numColorin) {
    if (numColorin >= 36) { // Validación para evitar desbordamiento
        #ifdef DEBUG
        Serial.println("Índice fuera de rango en listaColores");
        #endif
        return CRGB::Black; // Retorna negro por defecto si el índice es inválido
    }

    // Extrae los componentes RGB del valor hexadecimal
    unsigned int colorHex = listaColores[numColorin];
    byte r = (colorHex >> 16) & 0xFF; // Bits 16-23
    byte g = (colorHex >> 8) & 0xFF;  // Bits 8-15
    byte b = colorHex & 0xFF;         // Bits 0-7

    // Crea y retorna el color CRGB
    return CRGB(r, g, b);
}


uint16_t COLORHANDLER_::get_target_transition_time(){
    return target_transition_time;
}

void COLORHANDLER_::set_target_transition_time(uint16_t timein){
    target_transition_time= timein;
}

byte COLORHANDLER_::get_target_brightness(){
    return targetBrightness;
}

void COLORHANDLER_::set_current_brightness(uint16_t brightnessin){
    currentBrightness= brightnessin;
}

uint16_t COLORHANDLER_::get_current_transition_time(){
    return current_transition_time;
}

void COLORHANDLER_::set_current_transition_time(uint16_t timein){
    current_transition_time= timein;
}

void COLORHANDLER_::set_target_brightness(byte brightnessin){
    targetBrightness= brightnessin;
}

void COLORHANDLER_::color_action() {
    // Asegura valores válidos
    if (target_transition_time <= 0) {
        #ifdef DEBUG
        Serial.println("Tiempo de transición inválido, ajustando a 1 ms.");
        #endif
        target_transition_time = 1;
    }

    if (targetBrightness > 255) {
        #ifdef DEBUG
        Serial.println("Brillo objetivo fuera de rango, ajustando a 255.");
        #endif
        targetBrightness = 255;
    }

    unsigned long currentTime = millis();

    // Verifica si se necesita iniciar una nueva transición
    if (!fadeCompleted) {
        unsigned long elapsedTime = currentTime - startTime;

        // Calcula el progreso
        float progress = (float)elapsedTime / target_transition_time;

        // Si la transición está completa
        if (elapsedTime >= target_transition_time) {
            currentColor = targetColor;
            currentBrightness = targetBrightness;
            fadeCompleted = true;
            FastLED.show();
            #ifdef DEBUG
            Serial.println("Transición completada.");
            #endif
            return;
        }

        // Interpolación de color y brillo
        currentColor = blend(startColor, targetColor, progress * 255);
        currentColor.nscale8_video(startBrightness + (targetBrightness - startBrightness) * progress);

        // Actualiza LEDs
        for (int i = 0; i < numLeds; i++) {
            leds[i] = currentColor;
        }

        FastLED.show();
    }
}