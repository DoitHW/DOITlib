#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <map>
//testing MARC 2
//testing 2 3 4
//testing 3



bool color_mix_handler(int color1, int color2, byte *resultado) {
 
  static std::map <std::pair<int, int>, int> colorMixMap = {
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



COLORHANDLER_::COLORHANDLER_() : leds(nullptr), numLeds(0), paused(false), passive(false), startColor(CRGB::Black), currentColor(CRGB::Black), 
                                  targetColor(CRGB::Black), targetBrightness(255),
                                  currentBrightness(255), fadeTime(1000), 
                                  lastUpdateTime(0), transitionStartTime(0), transitioning(false) {}

void COLORHANDLER_::begin(int numLeds) {
    #ifdef COLUMNA
    const byte dataPin = COLUMN_LED_DATA_PIN;
    #endif
    #ifdef FIBRAS
    byte dataPin = LIGHTSOURCE_LED_DATA_PIN;
    #endif
    #ifdef WALLWASHER
    const byte dataPin = LEDSTRIP_LED_DATA_PIN;
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
    if (index < 0 || index >= sizeof(listaColoresPasivos) / sizeof(listaColoresPasivos[0])) {
                                                                #ifdef DEBUG
                                                                  Serial.println("Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes...");
                                                                #endif
        return CRGB::Black;   
    }
    return CRGB(listaColores[index]);
}

void COLORHANDLER_::set_passive(bool pas){
  passive= pas;
}

bool COLORHANDLER_::get_passive(){
  return passive;
}

void COLORHANDLER_::set_is_paused(bool pau){
  paused= pau;
}

bool COLORHANDLER_::get_is_paused(){
  return paused;
}

void COLORHANDLER_::action_frank() {

    unsigned long currentTime = millis();
    static CRGB targetColorPrev= 0;
    // Modo pasivo (ciclo automático de colores)
    if (passive) {
        static unsigned long lastColorChangeTime = 0;
        static int currentPassiveColorIndex = 0;
        
        if (!paused && (currentTime - lastColorChangeTime >= 3000)) {
            currentPassiveColorIndex++;
            if (currentPassiveColorIndex >= sizeof(listaColoresPasivos) / sizeof(listaColoresPasivos[0])) {
                currentPassiveColorIndex = 0;
            }
            targetColor = CRGB(listaColoresPasivos[currentPassiveColorIndex]);
            lastColorChangeTime = currentTime;
        }
        if (paused) return;
    }

    // Si hay un nuevo color objetivo diferente al actual
    if (targetColor != currentColor) {
        // Solo iniciamos nueva transición si no estamos en una o si el target cambió
        if (!transitioning || (transitioning && targetColor != targetColorPrev)) {
            startColor = currentColor;  // El color actual real como punto de partida
            transitionStartTime = currentTime;
            transitioning = true;
            targetColorPrev = targetColor;  // Guardamos el target para detectar cambios
            
            #ifdef DEBUG
                Serial.println("Nueva transición iniciada");
                Serial.printf("Start Color: RGB(%d,%d,%d)\n", startColor.r, startColor.g, startColor.b);
                Serial.printf("Target Color: RGB(%d,%d,%d)\n", targetColor.r, targetColor.g, targetColor.b);
            #endif
        }
        
        // Calculamos el progreso de la transición
        unsigned long elapsedTime = currentTime - transitionStartTime;
        
        if (elapsedTime >= fadeTime) {
            // Transición completa
            currentColor = targetColor;
            transitioning = false;
        } else {
            // Transición en progreso
            float progress = (float)elapsedTime / fadeTime;
            
            // Interpolación directa RGB manteniendo valores previos
            currentColor.r = startColor.r + ((targetColor.r - startColor.r) * progress);
            currentColor.g = startColor.g + ((targetColor.g - startColor.g) * progress);
            currentColor.b = startColor.b + ((targetColor.b - startColor.b) * progress);
        }
    }
    
    // Siempre actualizamos los LEDs con el color actual
    CRGB outputColor = currentColor;
    outputColor.nscale8(targetBrightness);
    fill_solid(leds, numLeds, outputColor);
    FastLED.show();
}


void COLORHANDLER_::meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
 
  for(int i = 0; i < LEDSTRIP_NUM_LEDS + LEDSTRIP_NUM_LEDS; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<LEDSTRIP_NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <LEDSTRIP_NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void COLORHANDLER_::fadeToBlack(int ledNo, byte fadeValue) {

  leds[ledNo].fadeToBlackBy(fadeValue);
}


void COLORHANDLER_::set_targetColor(CRGB color){

  targetColor= color;
}

void COLORHANDLER_::set_targetFade(uint16_t fade){

    if (fade > 0 && fade <= UINT16_MAX) fadeTime = fade;
    else {
                                                                                                #ifdef DEBUG
                                                                                                  Serial.println("Algo pasa al hacer set_targetFade");
                                                                                                #endif  
    }
}



void COLORHANDLER_::set_targetBrightness(byte brightin){

    if (brightin <= 255) targetBrightness = brightin;
        else {
                                                                                              #ifdef DEBUG
                                                                                                Serial.println("Algo pasa al hacer set_braitness");
                                                                                              #endif  
    }
}
byte COLORHANDLER_::get_targetBrightness(){
  return targetBrightness;

}

void COLORHANDLER_::set_currentBrightness(byte brightin){

    if (brightin <= 255) currentBrightness = brightin;
        else {
                                                                                              #ifdef DEBUG
                                                                                                Serial.println("Algo pasa al hacer set_braitness");
                                                                                              #endif  
    }
}


byte COLORHANDLER_::get_currentBrightness(){
  return currentBrightness;
}

void COLORHANDLER_::showStrip() {
   FastLED.show();
}

void COLORHANDLER_::setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;

}

void COLORHANDLER_::setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < LEDSTRIP_NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void COLORHANDLER_::Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[LEDSTRIP_NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / LEDSTRIP_NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= LEDSTRIP_NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < LEDSTRIP_NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void COLORHANDLER_::setPixelHeatColor (int Pixel, byte temperature) {
  byte t192 = round((temperature/255.0)*191);
  byte heatramp = t192 & 0x3F;
  heatramp <<= 2;
 
  if( t192 > 0x80) {                     // amarillo brillante con un toque de naranja
    setPixel(Pixel, 255, 120, 20);
  } else if( t192 > 0x40 ) {             //  más brillante
    setPixel(Pixel, 255, 40, 0);
  } else {                               //  oscuro que se desvanece
    setPixel(Pixel, heatramp, 0, 0);
  }
}