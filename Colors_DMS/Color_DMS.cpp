#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <map>
//testing MARC 2
//testing 2 3 4
//testing 3



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



COLORHANDLER_::COLORHANDLER_() : leds(nullptr), numLeds(0),startColor(CRGB::Black), currentColor(CRGB::Black), 
                                  targetColor(CRGB::Black), targetBrightness(255),
                                  currentBrightness(255), targetFade(1000), 
                                  lastUpdateTime(0), transitionStartTime(0), transitioning(false) {}

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
    //this->leds = new (std::nothrow) CRGB[numLeds];
    this->leds = new CRGB[numLeds];
    FastLED.addLeds<WS2811, dataPin, RGB>(leds, numLeds);
    FastLED.setBrightness(255);
}

CRGB COLORHANDLER_::get_CRGB_from_colorList(int index) {
    if (index < 0 || index >= sizeof(listaColores) / sizeof(listaColores[0])) {
                                                                #ifdef DEBUG
                                                                  Serial.println("Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes...");
                                                                #endif
        return CRGB::Black;   
    }
    return CRGB(listaColores[index]);
}


void COLORHANDLER_::action_frank(){

  if(!transitioning) return;
  unsigned long currentTime= millis();
  unsigned long elapsedTime= currentTime - transitionStartTime;

  if(elapsedTime >= targetFade){
    fill_solid(colorHandler.leds, 1, targetColor);
    FastLED.show();
    currentColor= targetColor;
    startColor= currentColor;
    transitioning= false;
    return;
  }
  else{
    Serial.println(targetFade);
    byte progress= map(elapsedTime, 0, targetFade, 0, 255);
    currentColor= blend(startColor, targetColor, progress);
    fill_solid(colorHandler.leds, 1, currentColor);
    FastLED.show();
  }

}


void COLORHANDLER_::action() {
    unsigned long currentTime = millis();

    // Limit updates to every ~20ms for smoother transitions
    if (currentTime - lastUpdateTime < 20) {
        return;
    }

    lastUpdateTime = currentTime;

    // Check if a transition is needed
    if (currentColor != targetColor) {
        if (!transitioning) {
            // Start a new transition
            transitionStartTime = currentTime;
            transitioning = true;
        }

        // Calculate progress of the transition
        unsigned long elapsedTime = currentTime - transitionStartTime;
        float progress = (float)elapsedTime / (float)targetFade;

        if (progress >= 1.0f) {
            // Transition complete
            currentColor = targetColor;
            transitioning = false;
            progress = 1.0f;
        }

        // Calculate the intermediate color
        currentColor = blend(currentColor, targetColor, (uint8_t)(progress * 255));

        // Apply the color to all LEDs
        for (int i = 0; i < numLeds; i++) {
            leds[i] = currentColor;
        }

        FastLED.show();
    } else {
        // No transition needed; ensure LEDs reflect the current color
        if (transitioning) {
            for (int i = 0; i < numLeds; i++) {
                leds[i] = currentColor;
            }
            FastLED.show();
            transitioning = false;
        }
    }
}



void COLORHANDLER_::set_targetColor(CRGB color){

  targetColor= color;
}

void COLORHANDLER_::set_targetFade(uint16_t fade){

    if (fade > 0 && fade <= UINT16_MAX) targetFade = fade;
    else {
      #ifdef DEBUG
        Serial.println("Algo pasa al hacer set_targetFade");
      #endif  
    }
}

void COLORHANDLER_::set_targetBrightness(byte brightness){

    if (brightness <= 255) targetBrightness = brightness;
        else {
      #ifdef DEBUG
        Serial.println("Algo pasa al hacer set_braitness");
      #endif  
    }

}