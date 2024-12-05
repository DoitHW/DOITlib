#include "Color_DMS.h"
#include "defines_DMS.h"
#include <Arduino.h>
#include <stdint.h>
#include <map>


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


COLORHANDLER_::COLORHANDLER_() : leds(nullptr), numLeds(0), fadeCompleted(true) {}

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
    this->leds = new CRGB[numLeds];
    FastLED.addLeds<WS2811, dataPin, RGB>(leds, numLeds);
    FastLED.setBrightness(255);
}

void COLORHANDLER_::do_color_crossFade(CRGB color1, CRGB color2, uint16_t fadeTime) {
    colorStart = color1;
    colorTarget = color2;
    colorCurrent = colorStart;
    this->fadeTime = fadeTime;
    startTime = millis();
    fadeCompleted = false;
}

CRGB COLORHANDLER_::getCurrentColor() {
    return colorCurrent;
}

bool COLORHANDLER_::update() {
    if (fadeCompleted) return true;

    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (elapsedTime >= fadeTime) {
        showColor(colorTarget);
        colorCurrent = colorTarget;
        fadeCompleted = true;
        return true;
    } else {
        uint8_t progress = map(elapsedTime, 0, fadeTime, 0, 255);
        colorCurrent = blend(colorStart, colorTarget, progress);
        showColor(colorCurrent);
        return false;
    }
}

void COLORHANDLER_::showColor(CRGB color) {
    fill_solid(leds, numLeds, color);
    FastLED.show();
}

void COLORHANDLER_::hacer_color(byte fadein, byte brightnessin, bool colorChangein, byte colorin) {
    static byte lastBrightness = 255;
    static byte baseR = 0, baseG = 0, baseB = 0; // Valores base del color actual
    Serial.println("Test 1");
    CRGB currentColor = getCurrentColor();
    Serial.println("Test 2");
    CRGB targetColor;
    
    if (colorChangein) {
        get_color(&baseR, &baseG, &baseB, colorin);
        lastBrightness = 255; // Resetear el brillo al cambiar el color
    }
    Serial.println("Test 3");
    // Siempre recalculamos el color objetivo basado en el brillo actual
    targetColor.r = map(brightnessin, 0, 255, 0, baseR);
    targetColor.g = map(brightnessin, 0, 255, 0, baseG);
    targetColor.b = map(brightnessin, 0, 255, 0, baseB);
    Serial.println("Test 4");
    if (fadein < 201) {
        fadeTime = map(fadein, 0, 200, 0, 20000);
    }
    Serial.println("Test 5");
    do_color_crossFade(currentColor, targetColor, fadeTime);
    Serial.println("Test 6");
    lastBrightness = brightnessin;
}

void COLORHANDLER_::get_color(byte* nextRin, byte* nextGin, byte* nextBin, byte colorin) {
    if (colorin < sizeof(listaColores) / sizeof(listaColores[0])) {
        unsigned int color = listaColores[colorin];
        *nextRin = (color >> 16) & 0xFF;

        *nextGin = (color >> 8) & 0xFF;
        *nextBin = color & 0xFF;
    } else {
        // Manejar el caso de un índice de color inválido
        *nextRin = *nextGin = *nextBin = 0;
    }
}