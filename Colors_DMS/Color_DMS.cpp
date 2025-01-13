#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <map>
#include <math.h>
#include <FastLED.h>
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



COLORHANDLER_::COLORHANDLER_() : leds(nullptr), numLeds(0), paused(false), passive(false), activePattern(NO_PATTERN), startColor(CRGB::Black), currentColor(CRGB::Black), 
                                  targetColor(CRGB::Black), targetBrightness(255),
                                  currentBrightness(255), fadeTime(1000), 
                                  lastUpdateTime(0), transitionStartTime(0), transitioning(false) {}

void COLORHANDLER_::begin(int numLeds) {
    #if   defined (COLUMNA)
    const byte dataPin = COLUMN_LED_DATA_PIN;
    #elif defined (WALLWASHER)
    const byte dataPin = LEDSTRIP_LED_DATA_PIN;
    #elif defined (BOTONERA)
    constexpr byte dataPin = BOTONERA_DATA_PIN;
    #endif
 

    this->numLeds = numLeds;
    if (this->leds != nullptr) {
        delete[] leds;  // Liberar memoria previa si ya estaba asignada
    }
    this->leds = new CRGB[numLeds];
    
    FastLED.addLeds<WS2812, dataPin, RGB>(leds, numLeds);
    FastLED.setBrightness(255);
}

CRGB COLORHANDLER_::get_CRGB_from_pasiveColorList(int index) {
    if (index < 0 || index >= sizeof(listaColoresPasivos) / sizeof(listaColoresPasivos[0])) {
                                                                #ifdef DEBUG
                                                                  Serial.println("Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes...");
                                                                #endif
        return CRGB::Black;   
    }
    return CRGB(listaColoresPasivos[index]);
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
    byte activePattern = colorHandler.get_activePattern();

    // Nuevas variables estáticas
    static bool brightnessTransitioning = false;
    static uint8_t currentBrightness = 255;
    static uint8_t targetBrightnessPrev = 255;
    static uint8_t startBrightness = 255;
    static unsigned long brightnessTransitionStartTime = 0;
    static unsigned long colorTransitionStartTime = 0;

    unsigned long currentTime = millis();
    static CRGB targetColorPrev = CRGB::Black;

    if (activePattern != NO_PATTERN) {
        // Manejar patrones activos
        switch (activePattern) {
            case COLOR_PATT:    colorHandler.RunningLights(0x40, 0x02, 0xFF, 30, 0.1, 5.0); break;
            case FIRE_PATT:     colorHandler.Fire(40, 90, 1); break;
            case METEOR_PATT:   colorHandler.meteorRain(0xFF, 0x30, 0x10, 4, 70, true, 1); break;
            case BOUNCING_PATT: colorHandler.BouncingBalls(0x00, 0x50, 0xFF, 1); break;
            case RAINBOW_PATT:  colorHandler.rainbowCycle(10); break;
            case SNOW_PATT:     colorHandler.SnowSparkle(0x04, 0x05, 0x06, 80, random(60, 600)); break;
            case CLOUD_PATT:   static uint8_t startIndex = 0;
                                startIndex = startIndex + 1; colorHandler.FillLEDsFromPaletteColors(startIndex); FastLED.show(); break;
        }
        return;
    }

    // Modo pasivo (ciclo automático de colores)
    if (passive) {
        static unsigned long lastColorChangeTime = 0;
        static int currentPassiveColorIndex = 0;
        
        if (!paused && (currentTime - lastColorChangeTime >= 3000)) {
            currentPassiveColorIndex = (currentPassiveColorIndex + 1) % (sizeof(listaColoresPasivos) / sizeof(listaColoresPasivos[0]));
            targetColor = CRGB(listaColoresPasivos[currentPassiveColorIndex]);
            lastColorChangeTime = currentTime;
        }
        if (paused) return;
    }

    // Iniciar nueva transición de color si es necesario
    if (targetColor != currentColor && (targetColor != targetColorPrev || !transitioning)) {
        startColor = currentColor;
        colorTransitionStartTime = currentTime;
        transitioning = true;
        targetColorPrev = targetColor;
    }

    // Iniciar nueva transición de brillo si es necesario
    if (targetBrightness != currentBrightness && (targetBrightness != targetBrightnessPrev || !brightnessTransitioning)) {
        startBrightness = currentBrightness;
        brightnessTransitionStartTime = currentTime;
        brightnessTransitioning = true;
        targetBrightnessPrev = targetBrightness;
    }

    // Calcular progreso de la transición de color
    float colorProgress = min(1.0f, float(currentTime - colorTransitionStartTime) / fadeTime);
    if (colorProgress >= 1.0f) {
        currentColor = targetColor;
        transitioning = false;
    } else {
        // Interpolación de color
        currentColor.r = startColor.r + ((targetColor.r - startColor.r) * colorProgress);
        currentColor.g = startColor.g + ((targetColor.g - startColor.g) * colorProgress);
        currentColor.b = startColor.b + ((targetColor.b - startColor.b) * colorProgress);
    }

    // Calcular progreso de la transición de brillo
    float brightnessProgress = min(1.0f, float(currentTime - brightnessTransitionStartTime) / fadeTime);
    if (brightnessProgress >= 1.0f) {
        currentBrightness = targetBrightness;
        brightnessTransitioning = false;
    } else {
        // Interpolación de brillo
        currentBrightness = startBrightness + ((targetBrightness - startBrightness) * brightnessProgress);
    }

    // Aplicar color y brillo actuales
    CRGB outputColor = currentColor;
    outputColor.nscale8(currentBrightness);
    fill_solid(leds, numLeds, outputColor);
    FastLED.show();
}


void COLORHANDLER_::SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
 
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xf0,0xf0,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}

void COLORHANDLER_::meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
 
  for(int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
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

void COLORHANDLER_::theaterChaseRainbow(int SpeedDelay) {
  byte *c;
 
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
        }
        showStrip();
       
        delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          setPixel(i+q, 0,0,0);        //turn every third pixel off
        }
    }
  }
}

byte* COLORHANDLER_::Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void COLORHANDLER_::rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

void COLORHANDLER_::RunningLights(byte red, byte green, byte blue, int WaveDelay, float frequencyFactor, float peakFactor) {
  int Position = 0;

  for(int j = 0; j < NUM_LEDS * 2; j++) {
    Position++;
    for(int i = 0; i < NUM_LEDS; i++) {
      float sinValue = sin((i + Position) * frequencyFactor * PI);
      float normalizedSin = pow(std::max(0.0f, static_cast<float>(sinValue)), peakFactor);
      setPixel(i,
               normalizedSin * red,
               normalizedSin * green,
               normalizedSin * blue);
    }
    
    showStrip();
    delay(WaveDelay);
  }
}




void COLORHANDLER_::BouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],red,green,blue);
    }
   
    showStrip();
    setAll(0,0,0);
  }
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

void COLORHANDLER_::set_activePattern (byte patternin){
  activePattern= patternin;
}
byte COLORHANDLER_::get_activePattern (){
  return activePattern;
}

void COLORHANDLER_::set_currentBrightness(byte brightin){

    if (brightin <= 255) currentBrightness = brightin;
        else {
                                                                                              #ifdef DEBUG
                                                                                                Serial.println("Algo pasa al hacer set_braitness");
                                                                                              #endif  
    }
}

void COLORHANDLER_::FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( CloudColors_p, colorIndex, brightness, LINEARBLEND);
        colorIndex++;
    }
    delay(100);
    
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
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void COLORHANDLER_::Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
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
    setPixel(Pixel, 255, 80, 20);
  } else if( t192 > 0x40 ) {             //  más brillante
    setPixel(Pixel, 255, 30, 0);
  } else {                               //  oscuro que se desvanece
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void COLORHANDLER_::set_botoneraPattern (byte patternin){
  switch (patternin)
  {
  case 0:
    
    break;
  case 1:
    
    break;
  }
}

void COLORHANDLER_::setPatternBotonera(byte mode) {
  //Serial.println("setPatternBotonera: " + String(mode));
    switch (mode) {
        case 0: {
         // Serial.println("Basico");
            // Patrón básico: Colores específicos para cada LED
            for (int i = 0; i < numLeds; i++) {
                switch (i) {
                    case 0: leds[i] = CRGB::Brown; break;
                    case 1: leds[i] = CRGB::White; break;
                    case 2: leds[i] = CRGB::Red; break;
                    case 3: leds[i] = CRGB::Cyan; break;
                    case 4: leds[i] = CRGB::Yellow; break;
                    case 5: leds[i] = CRGB(0xFF, 0x59, 0x00); break;
                    case 6: leds[i] = CRGB::Green; break;
                    case 7: leds[i] = CRGB(0xFF, 0x00, 0xD2); break;
                    case 8: leds[i] = CRGB::Blue; break;
                    default: leds[i] = CRGB::Black; break;
                }
            }
            break;
        }
        case 1: {
          //Serial.println("Calientes");
          // leds[0] = CRGB(0xFF, 0x00, 0x00); // Rojo
          // leds[1] = CRGB(0xFF, 0x1E, 0x14); 
          // leds[2] = CRGB(0xFF, 0x3C, 0x28); 
          // leds[3] = CRGB(0xFF, 0x5A, 0x1E); 
          // leds[4] = CRGB(0xFF, 0x78, 0x14); 
          // leds[5] = CRGB(0xFF, 0x96, 0x0A);
          // leds[6] = CRGB(0xFF, 0xB4, 0x05); 
          // leds[7] = CRGB(0xFF, 0xD2, 0x03);
          // leds[8] = CRGB(0xFF, 0xF0, 0x00); 

          leds[0] = CRGB::Black;               // Negro (equivalente al "rojo" inicial apagado)
leds[1] = CRGB(0x80, 0x00, 0x00);   // Rojo oscuro (borgoña)
leds[2] = CRGB(0xFF, 0x00, 0x00);   // Rojo puro
leds[3] = CRGB(0xFF, 0x66, 0x00);   // Naranja brillante
leds[4] = CRGB(0xFF, 0xA5, 0x00);   // Naranja cálido (ámbar)
leds[5] = CRGB(0xFF, 0xFF, 0x00);   // Amarillo puro
leds[6] = CRGB(0xFF, 0xE6, 0x33);   // Amarillo dorado (mostaza)
leds[7] = CRGB(0xFF, 0xF2, 0xCC);   // Blanco cálido con tinte crema
leds[8] = CRGB(0xFF, 0xFA, 0xE6);   // Blanco marfil (ligeramente más claro)


        break;
        }
        case 2: {
          //Serial.println("Frios");
            // Patrón gradiente de colores fríos
          leds[0] = CRGB::Black; // Rojo  
          leds[1] = CRGB(0x33, 0x00, 0x66); // Morado oscuro
          leds[2] = CRGB(0x66, 0x00, 0xCC); // Morado brillante
          leds[3] = CRGB(0x00, 0x00, 0xFF); // Azul puro
          leds[4] = CRGB(0x33, 0x66, 0xFF); // Azul cielo
          leds[5] = CRGB(0x00, 0xCC, 0xFF); // Turquesa intenso
          leds[6] = CRGB(0x33, 0xFF, 0xFF); // Turquesa brillante
          leds[7] = CRGB(0x99, 0xCC, 0xFF); // Azul claro con blanco
          leds[8] = CRGB(0xFF, 0xFF, 0xFF); // Blanco puro
        break;
        }
        default: {
            // Sin patrón: Apagar todos los LEDs
            fill_solid(leds, numLeds, CRGB::Black);
            break;
        }
    }
    // Actualizar la visualización de los LEDs
    FastLED.show();
}