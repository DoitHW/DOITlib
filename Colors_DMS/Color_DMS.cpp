#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <map>
#include <math.h>
#include <FastLED.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <microphone_DMS/microphone_DMS.h>
#include <play_DMS/play_DMS.h>
//testing MARC 2
//testing 2 3 4
//testing 3

#ifdef MIC
  extern MICROPHONE_ doitMic;
#endif


extern ELEMENT_ *element;
bool colorReceived= false;

bool COLORHANDLER_::color_mix_handler(int color1, int color2, byte *resultado) {
 
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
    #elif defined (ESCALERA)
      const byte dataPin = VUMETER_LED_DATA_PIN;
      // elif botonera SEMPRE el ultim
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


void COLORHANDLER_::set_step(int numStep, CRGB color){
    #if defined(ESCALERA) || defined(WALLWASHER)
      int inicio = numStep * LEDS_STEP;       // Primer LED del tramo
      int fin = inicio + LEDS_STEP;         // Último LED del tramo
      for (int i = inicio; i < fin; i++) colorHandler.leds[i] = color;
      FastLED.show();
    #endif
}

void COLORHANDLER_::reset_step(int numStep){
  #if defined(ESCALERA) || defined(WALLWASHER)
    int inicio = numStep * LEDS_STEP;       // Primer LED del tramo
    int fin = inicio + LEDS_STEP;         // Último LED del tramo
    for (int i = inicio; i < fin; i++) colorHandler.leds[i] = CRGB::Black;
    FastLED.show();
  #endif
}

void COLORHANDLER_::simon_game(byte& colorin) {
  #if defined(ESCALERA) || defined(WALLWASHER)
    static byte currentLevel = 0;
    const byte maxLevel = NUM_STEPS;
    static byte sequence[maxLevel];
    static bool gameActive = false;
    static bool waitingForInput = false;
    static byte playerInputCount = 0;
    const byte INVALID_COLOR = 0xFF;

    // Inicializar la semilla para números aleatorios
    static bool seedInitialized = false;
    if (!seedInitialized) {
        srand(static_cast<unsigned int>(time(0)));
        seedInitialized = true;
    }

    if(colorin >= 0 && colorin <= 8){
        CRGB color = colorHandler.get_CRGB_from_colorList(colorin);
        colorHandler.setAll(color.r, color.g, color.b);
        delay(600);
        colorHandler.setAll(0, 0, 0);
        delay(200);
    }

    // Iniciar nuevo juego
    if (!gameActive) {
        currentLevel = 0;
        gameActive = true;
        waitingForInput = false;
        playerInputCount = 0;
        for (int i = 0; i < maxLevel; i++) {
            sequence[i] = rand() % 7;
        }
        colorin = INVALID_COLOR;
        return;
    }

    // Mostrar secuencia en los steps
    if (!waitingForInput) {
        // Mostrar los colores secuencialmente y acumulativamente
        for (int i = 0; i <= currentLevel; i++) {
            // Encender todos los steps hasta el actual
            for (int j = 0; j <= i; j++) {
                CRGB color = colorHandler.get_CRGB_from_colorList(sequence[j]);
                #ifdef PLAYER
                  doitPlayer.play_file(14, sequence[j] + 11);
                #endif
                set_step(j, color);
            }
            delay(1500); // Esperar 1000ms antes de añadir el siguiente step
        }

        delay(2000); // Mantener la secuencia completa visible por 1 segundo adicional

        // Apagar todos los steps
        for (int i = 0; i <= currentLevel; i++) {
            reset_step(i);
        }
        delay(1000); // Pausa de 1 segundo antes de esperar la entrada del jugador

        waitingForInput = true;
        colorin = INVALID_COLOR;
        return;
    }

    // Procesar entrada del jugador
    if (waitingForInput && colorin != INVALID_COLOR && colorin <= 7) {
        if (colorin != sequence[playerInputCount]) {
            colorHandler.result_fail();
            delay(500);
            playerInputCount = 0; // Reiniciar el contador de entrada del jugador
            colorin = INVALID_COLOR;
            waitingForInput = false; // Volver a mostrar la secuencia
            return;
        }

        playerInputCount++;

        if (playerInputCount > currentLevel) {
            delay(500);
            if(playerInputCount == maxLevel) colorHandler.result_win(true);
            else                             colorHandler.result_win(false);
            currentLevel++;
            playerInputCount = 0;
            waitingForInput = false;
            
            if (currentLevel >= maxLevel) {
                gameActive = false;
            }
        }
        colorin = INVALID_COLOR;
    }
  #endif
}

void COLORHANDLER_::result_win(bool superWin){
    delay(200);

    #ifdef PLAYER
      byte res= rand() % 4;
      doitPlayer.play_file(WIN_RESP_BANK, 11 + res);
    #endif

    const int cycles = superWin ? 2 : 1;
    const int steps = 40;
    const int fadeTime = 20; 
    const int totalLeds = NUM_LEDS;
    const int centerLed = totalLeds / 2;

    for(int cycle = 0; cycle < cycles; cycle++){
        if (!superWin) {
            // Comportamiento original
            for(int i = 0; i < steps; i++){
                float ratio = static_cast<float>(i) / steps;
                byte intensity = static_cast<byte>(255 * ratio);
                int ledsToLight = static_cast<int>(totalLeds * ratio / 2);

                for(int led = 0; led < totalLeds; led++){
                    if(abs(led - centerLed) < ledsToLight){
                        colorHandler.setPixel(led, 0, intensity, 0);
                    } else {
                        colorHandler.setPixel(led, 0, 0, 0);
                    }
                }
                FastLED.show();
                delay(fadeTime / steps);
            }
            delay(100);

            for(int i = steps - 1; i >= 0; i--){
                float ratio = static_cast<float>(i) / steps;
                byte intensity = static_cast<byte>(255 * ratio);
                int ledsToLight = static_cast<int>(totalLeds * ratio / 2);

                for(int led = 0; led < totalLeds; led++){
                    if(abs(led - centerLed) < ledsToLight){
                        colorHandler.setPixel(led, 0, intensity, 0);
                    } else {
                        colorHandler.setPixel(led, 0, 0, 0);
                    }
                }
                FastLED.show();
                delay(fadeTime / steps);
            }
        } else {
            // Nuevo comportamiento: iluminar paso a paso
            for(int i = 0; i <= steps; i++){
                colorHandler.setAll(0, 0, 0);
                colorHandler.setPixel(i, 0, 255, 0);
                FastLED.show();
                delay(fadeTime / 2);
            }
            for(int i = steps - 1; i >= 0; i--){
                colorHandler.setAll(0, 0, 0);
                colorHandler.setPixel(i, 0, 255, 0);
                FastLED.show();
                delay(fadeTime / 2);
            }
        }
        delay(100);
    }
    colorHandler.setAll(0, 0, 0);
    delay(1000);
}



void COLORHANDLER_::result_fail(){
    delay(200);
    #ifdef PLAYER
      byte res= rand() % 4;
      doitPlayer.play_file(FAIL_RESP_BANK, 11 + res);
    #endif

    const int cycles = 1;
    const int steps = 40;
    const int fadeTime = 100; 
    const int totalLeds = NUM_LEDS;
    const int centerLed = totalLeds / 2;

    for(int cycle = 0; cycle < cycles; cycle++){
        // Fundido de entrada y encendido desde el centro
        for(int i = 0; i < steps; i++){
            float ratio = static_cast<float>(i) / steps;
            byte intensity = static_cast<byte>(255 * ratio);
            int ledsToLight = static_cast<int>(totalLeds * ratio / 2);

            for(int led = 0; led < totalLeds; led++){
                if(abs(led - centerLed) < ledsToLight){
                    colorHandler.setPixel(led, intensity, 0, 0);
                } else {
                    colorHandler.setPixel(led, 0, 0, 0);
                }
            }
            FastLED.show();
            delay(fadeTime / steps);
        }
        delay(100);

        for(int i = steps - 1; i >= 0; i--){
            float ratio = static_cast<float>(i) / steps;
            byte intensity = static_cast<byte>(255 * ratio);
            int ledsToLight = static_cast<int>(totalLeds * ratio / 2);

            for(int led = 0; led < totalLeds; led++){
                if(abs(led - centerLed) < ledsToLight){
                    colorHandler.setPixel(led, intensity, 0, 0);
                } else {
                    colorHandler.setPixel(led, 0, 0, 0);
                }
            }
            FastLED.show();
            delay(fadeTime / steps);
        }
        delay(100);
    }
    colorHandler.setAll(0, 0, 0);
    delay(1000);
}


void COLORHANDLER_::fade_in_out(byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}

CRGB COLORHANDLER_::get_CRGB_from_pasiveColorList(int index) {
    if (index < 0 || index >= sizeof(listaColoresPasivos) / sizeof(listaColoresPasivos[0])) {
    _ERR_THROW_START_ "Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes..." _ERR_THROW_END_
                                                                #ifdef DEBUG
                                                                  DEBUG__________ln("Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes...");
                                                                #endif
        return CRGB::Black;   
    }
    return CRGB(listaColoresPasivos[index]);
}


CRGB COLORHANDLER_::get_CRGB_from_colorList(int index) {
    if (index < 0 || index >= sizeof(listaColores) / sizeof(listaColores[0])) {
                                                                #ifdef DEBUG
                                                                  DEBUG__________ln("Al intentar crear un CRGB a partir del numero de color, el numero de color se sale de los margenes...");
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

void COLORHANDLER_::elem_color_action() {

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
        
        //_ERR_THROW_START_ "PATRON ACTIVO" _ERR_THROW_END_
        switch (activePattern) {
            case COLOR_PATT:    colorHandler.RunningLights(0x40, 0x02, 0xFF, 30, 0.1, 5.0); 
                                break;

            case FIRE_PATT:     colorHandler.Fire(40, 90, 1); 
                                break;

            case METEOR_PATT:   colorHandler.meteorRain2(colorReceived, colorHandler.targetColor); 
                                if(colorReceived == true) colorReceived= false;
                                break;

            case BOUNCING_PATT: colorHandler.BouncingBalls_PSA(0x50, 0x90, 0xFF, 1); 
                                break;

            case RAINBOW_PATT:  colorHandler.rainbowCycle(10); 
                                break;

            case SNOW_PATT:     colorHandler.SnowSparkle(0x01, 0x05, 0x09, 100, random(80, 900)); 
                                break;

            case CLOUD_PATT:    static uint8_t startIndex = 0;
                                startIndex = startIndex + 1; colorHandler.FillLEDsFromPaletteColors(startIndex); FastLED.show(); 
                                break;
        }
        return;
    }
    // Modo pasivo (ciclo automático de colores)
    if (passive) {
     // _ERR_THROW_START_ "PASIVO ACTIVO" _ERR_THROW_END_
        int passiveInterval=0;
        if(slowPassive) passiveInterval= 5000;
        else            passiveInterval= 3000;
        static unsigned long lastColorChangeTime = 0;
        static int currentPassiveColorIndex = 0;
        
        if (!paused && (currentTime - lastColorChangeTime >= passiveInterval)) {
            currentPassiveColorIndex = (currentPassiveColorIndex + 1) % (sizeof(listaColoresPasivos_COPYRIGHT) / sizeof(listaColoresPasivos_COPYRIGHT[0]));
            targetColor = CRGB(listaColoresPasivos_COPYRIGHT[currentPassiveColorIndex]);
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
  
  // Increase the step size to make the meteor move faster
  int stepSize = 2; // You can adjust this value to control the speed
  
  for(int i = 0; i < NUM_LEDS + NUM_LEDS; i += stepSize) {
    // fade brightness 
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
    
    // Reduce the delay to increase speed
    //delay(SpeedDelay / 2); // You can adjust this divisor to fine-tune the speed
  }
}


void COLORHANDLER_::meteorRain2(bool launchNewMeteor, CRGB meteorColor) {
  float pps = 200.0f;
  static const int MAX_METEORS = 10;
  static struct Meteor {
    float position;
    CRGB color;
    byte size;
    byte trailDecay;
    bool randomDecay;
    unsigned long lastUpdate;
    bool active;
  } meteors[MAX_METEORS] = {};
  static int meteorCount = 0;
  static unsigned long lastLaunchTime = 0;

  unsigned long currentTime = millis();

  // Lanzar nuevo meteorito
  if (launchNewMeteor && meteorCount < MAX_METEORS && currentTime - lastLaunchTime >= 100) {
    for (int i = 0; i < MAX_METEORS; i++) {
      if (!meteors[i].active) {
        meteors[i] = {
          static_cast<float>(NUM_LEDS),
          meteorColor, // Usar el color proporcionado como parámetro
          static_cast<byte>(random(3, 8)),
          200,
          true,
          currentTime,
          true
        };
        meteorCount++;
        lastLaunchTime = currentTime;
        break;
      }
    }
  }

  // Actualizar y dibujar cada meteorito
  for (int i = 0; i < MAX_METEORS; i++) {
    if (meteors[i].active) {
      Meteor& m = meteors[i];
      
      // Actualizar posición
      float elapsedTime = (currentTime - m.lastUpdate) / 1000.0f; // Tiempo en segundos
      m.position -= pps * elapsedTime; // píxeles por segundo
      m.lastUpdate = currentTime;

      // Dibujar el meteorito y su cola
      for (int j = 0; j < NUM_LEDS; j++) {
        float distance = j - m.position;
        if (distance >= 0 && distance < m.size + 5) { // Cola de 5 píxeles
          float intensity = 1.0f - (distance / (m.size + 10));
          intensity = intensity * intensity; // Función cuadrática para una caída más natural
          CRGB pixelColor = m.color;
          pixelColor.nscale8(255 * intensity);
          leds[j] += pixelColor;
        }
      }

      // Eliminar el meteorito si ha salido de la tira
      if (m.position + m.size < 0) {
        m.active = false;
        meteorCount--;
      }
    }
  }

  // Aplicar un desvanecimiento general a toda la tira
  fadeToBlackBy(leds, NUM_LEDS, 40);

  FastLED.show();
}

void COLORHANDLER_::meteorRain2_random(bool launchNewMeteor) {
  static const int SPEED = 5;        // Velocidad del meteorito (píxeles por frame)
  static const int MAX_METEORS = 20;
  static const int MIN_LAUNCH_DELAY = 30;  // Tiempo mínimo entre meteoritos
  
  static struct Meteor {
    int position;    // Posición en píxeles * 16 para movimiento suave
    CRGB color;
    byte size;
    bool active;
  } meteors[MAX_METEORS] = {};
  
  static int meteorCount = 0;
  static unsigned long lastLaunchTime = 0;
  
  // Lanzar nuevo meteorito
  if (launchNewMeteor && meteorCount < MAX_METEORS && (millis() - lastLaunchTime >= MIN_LAUNCH_DELAY)) {
    for (int i = 0; i < MAX_METEORS; i++) {
      if (!meteors[i].active) {
        meteors[i] = {
          NUM_LEDS * 16,  // Posición inicial escalada
          CRGB(random(0, 255), random(0, 120), random(0, 130)),
          static_cast<byte>(random(3, 20)),
          true
        };
        meteorCount++;
        lastLaunchTime = millis();
        break;
      }
    }
  }

  // Limpiar LEDs
  fadeToBlackBy(leds, NUM_LEDS, 40);

  // Actualizar y dibujar meteoritos
  for (int i = 0; i < MAX_METEORS; i++) {
    if (meteors[i].active) {
      Meteor& m = meteors[i];
      
      // Actualizar posición
      m.position -= SPEED;
      
      // Convertir posición escalada a posición real
      int currentPos = m.position >> 4;  // División por 16
      
      // Solo dibujar si está en rango visible
      if (currentPos + m.size >= 0 && currentPos < NUM_LEDS) {
        for (int j = max(0, currentPos); j < min(NUM_LEDS, currentPos + m.size + 5); j++) {
          int distance = j - currentPos;
          if (distance >= 0 && distance < m.size + 5) {
            int intensity = 255 - ((distance * 255) / (m.size + 5));
            CRGB pixelColor = m.color;
            pixelColor.nscale8(intensity);
            leds[j] += pixelColor;
          }
        }
      }

      // Eliminar meteorito si está fuera de la tira
      if (currentPos + m.size < 0) {
        m.active = false;
        meteorCount--;
      }
    }
  }

  FastLED.show();
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




void COLORHANDLER_::BouncingBalls_PSA(byte red, byte green, byte blue, int BallCount) {
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
                                                                                                  DEBUG__________ln("Algo pasa al hacer set_targetFade");
                                                                                                #endif  
    }
}



void COLORHANDLER_::set_targetBrightness(byte brightin){

    if (brightin <= 255) targetBrightness = brightin;
        else {
                                                                                              #ifdef DEBUG
                                                                                                DEBUG__________ln("Algo pasa al hacer set_braitness");
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
                                                                                                DEBUG__________ln("Algo pasa al hacer set_braitness");
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
  FastLED.show();
  //showStrip();
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


#ifdef NOELEM
void COLORHANDLER_::welcomeEffect() {
 
  const int totalLEDs = NUM_LEDS;  // Asumimos 9 LEDs

  // 1. Definir los colores finales para cada LED.
  //    LED 0 se mantiene apagado.
    CRGB targetColors[totalLEDs] = {
        CRGB::Black,      // LED 0: apagado
        CRGB::White,      // LED 1: blanco
        CRGB::Red,        // LED 2: rojo
        CRGB::Cyan,       // LED 3: celeste
        CRGB::Yellow,     // LED 4: amarillo
        CRGB(0xFF, 0x59, 0x00), // LED 5: naranja
        CRGB::Green,      // LED 6: verde
        CRGB::Purple,     // LED 7: violeta
        CRGB::Blue        // LED 8: azul
    };

  // 2. Limpiar la tira: apagar todos los LEDs.
  fill_solid(this->leds, totalLEDs, CRGB::Black);
  FastLED.show();
  delay(50);  // Breve retardo para asegurar la actualización

  // 3. Configurar el efecto "breath": número de pasos y retardo entre ellos.
  const int steps = 50;        // Número de pasos en el fade-in
  const int stepDelay = 100;   // Retardo en milisegundos entre cada paso

  // 4. Para cada paso, se calcula el nivel de brillo (de 0 a 255)
  //    y se actualiza cada LED (excepto el 0) con su color escalado.
  for (int step = 0; step <= steps; step++) {
      uint8_t brightness = map(step, 0, steps, 0, 255);
      for (int i = 0; i < totalLEDs; i++) {
          if (i == 0) {
              this->leds[i] = CRGB::Black;
          } else {
              // Se copia el color objetivo y se escala su brillo
              CRGB c = targetColors[i];
              c.nscale8_video(brightness);
              this->leds[i] = c;
          }
      }
      FastLED.show();
      delay(stepDelay);
  }

  // 5. Asegurar que el estado final es el brillo completo.
  for (int i = 0; i < totalLEDs; i++) {
      this->leds[i] = targetColors[i];
  }
  FastLED.show();
  
}
#endif

void COLORHANDLER_::setPatternBotonera(byte mode, DynamicLEDManager& ledManager) {
  ledManager.clearEffects(); // Limpiar efectos dinámicos previos

  if (mode >= 16) {
#ifdef DEBUG
      DEBUG__________printf("⚠️ Modo inválido: %d\n", mode);
#endif
      return;
  }

  byte modeConfig[2] = {0};
  if (!getModeConfig(currentFile, mode, modeConfig)) {
#ifdef DEBUG
      DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo.");
#endif
      return;
  }

#ifdef DEBUG
  DEBUG__________printf("Mode %d - Byte Config: 0x%02X%02X\n", mode, modeConfig[0], modeConfig[1]);
#endif

  // --- FICHAS ---
  if (currentFile == "Fichas") {
      fill_solid(leds, NUM_LEDS, CRGB::Black);

      if (mode == 0) {
          ledManager.addEffect(new FadeEffect(*this, 0, CRGB::Blue, CRGB::Cyan, 50));
      } else if (mode == 1 || mode == 2) {
          ledManager.addEffect(new FadeEffect(*this, 0, CRGB::Blue, CRGB::Cyan, 50));
          leds[8] = CRGB::White;
      }

      FastLED.show();
      return;
  }

  // --- FLAGS DE RELÉ ---
  bool hasRelay    = getModeFlag(modeConfig, HAS_RELAY);
  bool hasRelayN1  = getModeFlag(modeConfig, HAS_RELAY_N1);
  bool hasRelayN2  = getModeFlag(modeConfig, HAS_RELAY_N2);

  int relayCount = 0;

  if (!hasRelay && hasRelayN1 && hasRelayN2) {
      relayCount = 4; // caso aromaterapia
  } else if (hasRelay) {
      if (!hasRelayN1 && !hasRelayN2)     relayCount = 1;
      else if (!hasRelayN1 && hasRelayN2) relayCount = 2;
      else if (hasRelayN1 && !hasRelayN2) relayCount = 3;
      else if (hasRelayN1 && hasRelayN2)  relayCount = 4;
  }


  bool isMultiRelay = relayCount > 1;
  bool isAromaterapia = (!hasRelay && hasRelayN1 && hasRelayN2);

  // --- COLORES DE LOS BOTONES (excepto LED 0) ---
  if (getModeFlag(modeConfig, HAS_PATTERNS)) {
      fill_solid(leds + 1, NUM_LEDS - 1, CRGB::White);
  } else if (getModeFlag(modeConfig, HAS_PASSIVE)) {
      fill_solid(leds + 1, NUM_LEDS - 1, CRGB::Black);
      leds[8] = CRGB::White;
  } else if (getModeFlag(modeConfig, HAS_BASIC_COLOR) || getModeFlag(modeConfig, HAS_ADVANCED_COLOR)) {
      CRGB colorMap[] = {
          CRGB::Black, CRGB::White, CRGB::Red, CRGB::Cyan,
          CRGB::Yellow, CRGB(0xFF, 0x59, 0x00), CRGB::Green,
          CRGB(0xFF, 0x00, 0xD2), CRGB::Blue
      };
      for (int i = 1; i < NUM_LEDS; i++) {
          leds[i] = colorMap[i];
      }
  } else {
      fill_solid(leds + 1, NUM_LEDS - 1, CRGB::Black);
  }
  DEBUG__________ln("relayCount: " + String(relayCount));
  // --- VISUALIZACIÓN DE RELÉS ---
  if (relayCount == 1 && hasRelay) {
      // Caso: solo un relé → usar LED 0 con efecto
      ledManager.addEffect(new FadeEffect(*this, 0, CRGB::Blue, CRGB::Cyan, 50));
  } else if (isMultiRelay || isAromaterapia) {
      // Caso: 2 o más relés → usar LEDs 9, 7, 5, 3 en blanco
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      if (relayCount >= 1) leds[8] = CRGB::Blue;
      if (relayCount >= 2) leds[6] = CRGB::Green;
      if (relayCount >= 3) leds[4] = CRGB::Yellow;
      if (relayCount >= 4) leds[2] = CRGB::Red;

  } else {
      leds[0] = CRGB::Black;
  }

  FastLED.show();
}

void COLORHANDLER_::sequencer_game(byte& colorin) {
    static std::vector<byte> sequence;
    const byte INVALID_COLOR = 0xFF;
    const byte NUM_COLORS = 8;


    // Si se recibe un color válido
    if (colorin >= 0 && colorin < NUM_COLORS) {
        // Iluminar toda la tira LED con el color introducido
        CRGB color = colorHandler.get_CRGB_from_colorList(colorin);
        #ifdef PLAYER
          doitPlayer.play_file(1, colorin + 80);
        #endif
        if (colorin < NUM_STEPS) {
            set_step(colorin, color);
            delay(600);
            reset_step(colorin);
        }

        // Añadir el color a la secuencia
        sequence.push_back(colorin);

        // Si la secuencia es igual o mayor que NUM_STEPS, reproducirla
        if (sequence.size() >= NUM_STEPS) {
            // Pequeña pausa antes de reproducir
            delay(1000);

            // Reproducir la secuencia
            for (size_t i = 0; i < sequence.size(); i++) {
                byte currentColor = sequence[i];
                CRGB stepColor = colorHandler.get_CRGB_from_colorList(currentColor);
                #ifdef PLAYER
                  doitPlayer.play_file(1, currentColor + 80);
                #endif
                
                if (currentColor < NUM_STEPS) {
                    set_step(currentColor, stepColor);
                    delay(800);
                    reset_step(currentColor);
                } else {
                    // Para el color 7, iluminar toda la tira
                    colorHandler.setAll(stepColor.r, stepColor.g, stepColor.b);
                    delay(800);
                    colorHandler.setAll(0, 0, 0);
                }
                
                delay(100); // Pequeña pausa entre colores
            }

            // Reiniciar la secuencia para la próxima ronda
            sequence.clear();
        }

        colorin = INVALID_COLOR; // Resetear colorin
    }
}

void COLORHANDLER_::mapCognitiveLEDs() {
  DEBUG__________ln("Mapeo de LEDs cognitivos: ");
  colorHandler.setPatternBotonera(0, ledManager); // Desactiva todos
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
  }
  leds[2] = CRGB::Red;
  leds[4] = CRGB::Yellow;
  leds[6] = CRGB::Green;
  leds[8] = CRGB::Blue;
  FastLED.show();
}



#ifdef MIC 
void COLORHANDLER_::BouncingBalls(byte baseRed, byte baseGreen, byte baseBlue) {
  // Parámetros físicos y de simulación
  const float gravedad      = -9.81;      // La aceleración es negativa: si definimos el tope con h = alturaInicial, la caída hará decrecer h.
  const float alturaInicial = 1.0;        // Altura máxima (tope)
  const float timeScale     = 0.5;        // Factor para ralentizar la simulación (movimiento más suave)
  const int   maxBloques    = 10;         // Número máximo de bloques (pelotas) activos
  
  // Variables estáticas para mantener el estado de cada bloque entre llamadas
  static bool bloqueActivo[maxBloques] = { false };
  static float h0[maxBloques];             // Altura inicial de la fase actual (1.0 para caída, 0 para rebote desde suelo)
  static float velocidad[maxBloques];      // Velocidad inicial de la fase actual
  static unsigned long tiempoUltimoRebote[maxBloques] = { 0 };  // Tiempo de inicio de la fase actual
  static float amortiguacion[maxBloques] = { 0 };  // Factor de amortiguación para cada bloque
  static CRGB colorBloque[maxBloques];     // Color asignado a cada bloque

  // 1. Lanzar un nuevo bloque si se detecta sonido.
  // IMPORTANTE: Si detect_sound_threshold() nunca retorna true, no se lanzará ningún bloque.
  if (doitMic.detect_sound_threshold()) {  // Esta función debe devolver true cuando se supera el umbral
    for (int i = 0; i < maxBloques; i++) {
      if (!bloqueActivo[i]) {
        bloqueActivo[i] = true;
        // Inicializar para caer desde el tope:
        h0[i] = alturaInicial;
        velocidad[i] = 0.0;
        tiempoUltimoRebote[i] = millis();
        // Amortiguación levemente distinta para cada bloque:
        amortiguacion[i] = 0.90 - float(i) / (maxBloques * maxBloques);
        // Variación aleatoria en el color para diferenciar bloques:
        int r = baseRed   + random(-30, 31);
        int g = baseGreen + random(-30, 31);
        int b = baseBlue  + random(-30, 31);
        colorBloque[i] = CRGB(constrain(r, 0, 255),
                               constrain(g, 0, 255),
                               constrain(b, 0, 255));
        break;  // Se lanza solo un bloque por detección.
      }
    }
  }

  // Opcional (para pruebas): Si no hay ningún bloque activo, lanzar uno automáticamente.
  // Descomenta este bloque para ver la animación sin depender de la detección de sonido.
  /*
  {
    bool ningunoActivo = true;
    for (int i = 0; i < maxBloques; i++) {
      if (bloqueActivo[i]) { ningunoActivo = false; break; }
    }
    if (ningunoActivo) {
      bloqueActivo[0] = true;
      h0[0] = alturaInicial;
      velocidad[0] = 0.0;
      tiempoUltimoRebote[0] = millis();
      amortiguacion[0] = 0.90;
      colorBloque[0] = CRGB(baseRed, baseGreen, baseBlue);
    }
  }
  */

  // 2. Aplicar un fundido a toda la tira para lograr el efecto “trail”
  // Se asume que colorHandler.leds es el array de LEDs con NUM_STEPS * LEDS_STEP elementos.
  fadeToBlackBy(colorHandler.leds, NUM_STEPS * LEDS_STEP, 30);

  // 3. Actualizar la física y dibujar cada bloque activo.
  unsigned long tiempoActual = millis();
  for (int i = 0; i < maxBloques; i++) {
    if (bloqueActivo[i]) {
      // Calcular el tiempo transcurrido (en segundos) desde el inicio de la fase actual,
      // aplicando el factor timeScale para un movimiento más suave.
      float t = ((tiempoActual - tiempoUltimoRebote[i]) / 1000.0) * timeScale;
      // Ecuación de movimiento: h = h₀ + v₀*t + ½*g*t²
      float h = h0[i] + velocidad[i] * t + 0.5 * gravedad * t * t;

      // Si la altura cae por debajo o llega a 0 (suelo), se produce el rebote:
      if (h <= 0) {
        h = 0;
        // Calcular la velocidad de impacto: vImpact = v₀ + g*t
        float vImpact = velocidad[i] + gravedad * t;
        // La nueva velocidad es el rebote (invertida y amortiguada)
        velocidad[i] = -amortiguacion[i] * vImpact;
        // Se reinicia la fase a partir del suelo (h₀ = 0)
        h0[i] = 0;
        tiempoUltimoRebote[i] = tiempoActual;
        // Si la velocidad es muy baja, damos por terminado el movimiento de este bloque
        if (velocidad[i] < 0.01) {
          bloqueActivo[i] = false;
          continue;
        }
        t = 0; // Reiniciar el tiempo para la nueva fase
      }

      // 4. Mapear la altura a una posición de LED.
      // Queremos que: h = alturaInicial → pos = 0 (techo)
      //             h = 0 → pos = NUM_STEPS - 1 (suelo)
      float posFraccional = (NUM_STEPS - 1) - (h * (NUM_STEPS - 1) / alturaInicial);
      int posEntera = floor(posFraccional);
      float fraccion = posFraccional - posEntera;

      // 5. Dibujar el bloque con degradado entre dos steps para suavizar el movimiento.
      CRGB colorInferior = colorBloque[i];
      CRGB colorSuperior = colorBloque[i];
      colorInferior.nscale8_video((1.0 - fraccion) * 255);
      colorSuperior.nscale8_video(fraccion * 255);

      if (posEntera >= 0 && posEntera < NUM_STEPS) {
        set_step(posEntera, colorInferior);
      }
      if ((posEntera + 1) >= 0 && (posEntera + 1) < NUM_STEPS) {
        set_step(posEntera + 1, colorSuperior);
      }
    }
  }

  // 6. Actualizar la tira de LEDs
  FastLED.show();
}



  void COLORHANDLER_::tone_color() {
      // Variables estáticas para la estabilidad de la nota (0 indica error o ausencia de nota válida)
      static byte lastDetectedNote = 0;
      static byte lastStableNote = 0;
      static byte noteCounter = 0;
      
      const byte STABILITY_THRESHOLD = 3;
      // Lista de nombres para las notas (índice 0: DO, …, índice 6: SI)
      const char* noteNames[] = {"DO", "RE", "MI", "FA", "SOL", "LA", "SI"};
      
      byte detectedNote = doitMic.detect_musical_note();
      DEBUG__________ln("Nota escuchada: " + String(detectedNote));

      // Procesar solo si se detecta una nota válida (entre 1 y 7)
      if (detectedNote > 0 && detectedNote <= 7) {
          if (detectedNote == lastDetectedNote) {
              noteCounter++;
              if (noteCounter >= STABILITY_THRESHOLD && detectedNote != lastStableNote) {
                  byte mappedNote = detectedNote - 1;  // Mapea 1-7 a índice 0-6
                  CRGB noteColor = colorHandler.get_CRGB_from_colorList(mappedNote);
                  for (int i = 0; i < NUM_STEPS * LEDS_STEP; i++) {
                      colorHandler.leds[i] = noteColor;
                  }
                  FastLED.show();
                  DEBUG__________ln("Nota estable detectada: " + String(noteNames[mappedNote]));
                  lastStableNote = detectedNote;
                  #ifdef PLAYER
                                  // doitPlayer.play_file(14, detectedNote + 11);
                  #endif
              }
          } else {
              noteCounter = 0;
          }
          lastDetectedNote = detectedNote;
      } else {
          noteCounter = 0;
      }
  }




  void COLORHANDLER_::tone_game() {
      // Variable estática para almacenar la última nota mostrada (valores válidos: 1 a 7)
      static byte lastNote = 0;
      
      // Obtener la nota detectada. Se asume que detect_musical_note() devuelve un valor entre 1 y 7 (1 = DO, ..., 7 = SI)
      byte currentNote = doitMic.detect_musical_note();
      
      // Solo se procesa si la nota detectada es válida
      if (currentNote < 1 || currentNote > 7) {
          return;
      }
      
      // Si la nota no ha cambiado, no se actualiza la tira LED
      if (currentNote == lastNote) {
          return;
      }
      
      // Actualizar la última nota detectada
      lastNote = currentNote;
      
      for(int i= 0; i < NUM_LEDS; i++){
          
      }
  }

  void COLORHANDLER_::speak_game() {
      static byte propuesta = 0;
      static byte jugada = 0;
      static bool gameActive = false;
      const  byte MAX_STEPS = NUM_STEPS - 1;
      static byte col = 0;

      // Inicializar el juego
      if (!gameActive) {
          // Calcular el rango para la propuesta (30% superior)
          byte minPropuesta = MAX_STEPS * 0.7 + 1; // 70% + 1 para asegurar el 30% superior
          propuesta = random(minPropuesta, NUM_STEPS); // Propuesta entre minPropuesta y MAX_STEPS
          jugada = 0;
          gameActive = true;
          
          // Iluminar el step "propuesta"
          CRGB color = colorHandler.get_CRGB_from_colorList(col++);
          if (col == 8) col = 0;
          set_step(propuesta, color);
      }

      // Lógica del juego
      if (gameActive) {
          byte micValue = doitMic.get_mic_value_BYTE(MIC_SENS);
          byte newJugada = map(micValue, 0, 255, 0, MAX_STEPS);
          
          if (newJugada > jugada) {
              jugada = newJugada;
              
              // Iluminar hasta el step "jugada" actual
              CRGB colorJugada = CRGB::Blue; // Puedes cambiar este color según necesites
              for (int i = 0; i <= jugada; i++) {
                  set_step(i, colorJugada);
              }
          }

          // Verificar si el jugador ha alcanzado la "propuesta"
          if (jugada >= propuesta) {
              // El jugador gana
              colorHandler.result_win(false);
              gameActive = false; // Reiniciar el juego
              delay(1000); // Esperar antes de reiniciar
          }
      }
  }


  void COLORHANDLER_::voice_meteors(){

      bool meteor= doitMic.detect_sound_threshold();
      colorHandler.meteorRain2_random(meteor);
  }

  void COLORHANDLER_::block_speak_SA() {

    static unsigned long lastUpdateTime = 0;
    static int currentStep = NUM_STEPS - 1;  // Comienza desde el paso más alto
    static int targetStep = NUM_STEPS - 1;
    static float currentPosition = NUM_STEPS - 1;
    static int colorIndex = 0;  // Índice del color actual (0 a 7)
    const float RISE_SPEED = 0.4;  // Velocidad de subida (ajusta según necesites)
    const float FALL_SPEED = 0.2;  // Velocidad de caída (ajusta según necesites)
    const int UPDATE_INTERVAL = 10;  // Intervalo de actualización en ms

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < UPDATE_INTERVAL) return;  // Aún no es tiempo de actualizar

    lastUpdateTime = currentTime;

    // Detectar sonido
    bool soundDetected = doitMic.detect_sound_threshold();

    // Actualizar el paso objetivo (invertido)
    if (soundDetected && targetStep > 0) {
        targetStep--;
    } else if (!soundDetected && targetStep < NUM_STEPS - 1) {
        targetStep++;
    }

    // Mover suavemente hacia el paso objetivo (invertido)
    if (currentPosition > targetStep) {
        currentPosition -= RISE_SPEED;
    } else if (currentPosition < targetStep) {
        currentPosition += FALL_SPEED;
    }

    // Actualizar los LEDs (invertido)
    int newStep = round(currentPosition);
    if (newStep != currentStep) {
        // Si se ilumina el último bloque, cambiar el color
        if (newStep == NUM_STEPS - 1 && currentStep != NUM_STEPS - 1) {
            colorIndex = (colorIndex + 1) % 8;  // Incrementar índice y volver a 0 si llega a 8
        }

        CRGB currentColor = colorHandler.get_CRGB_from_colorList(colorIndex);  // Obtener el color actual según el índice

        // Apagar todos los LEDs por debajo del nuevo paso
        for (int i = 0; i < newStep; i++) {
            reset_step(i);
        }
        // Encender todos los LEDs desde el nuevo paso hasta arriba
        for (int i = newStep; i < NUM_STEPS; i++) {
            set_step(i, currentColor);  // Usar el color actual
        }
        currentStep = newStep;
    }
}


  void COLORHANDLER_::block_speak(CRGB colorin) {

      static unsigned long lastUpdateTime = 0;
      static int currentStep = NUM_STEPS - 1;  // Comienza desde el paso más alto
      static int targetStep = NUM_STEPS - 1;
      static float currentPosition = NUM_STEPS - 1;
      const float RISE_SPEED = 0.4;  // Velocidad de subida (ajusta según necesites)
      const float FALL_SPEED = 0.2;  // Velocidad de caída (ajusta según necesites)
      const int UPDATE_INTERVAL = 10;  // Intervalo de actualización en ms
    // Número total de pasos en el VU meter

      unsigned long currentTime = millis();
      if (currentTime - lastUpdateTime < UPDATE_INTERVAL) return;  // Aún no es tiempo de actualizar

      lastUpdateTime = currentTime;

      // Detectar sonido
      bool soundDetected = doitMic.detect_sound_threshold();

      // Actualizar el paso objetivo (invertido)
      if (soundDetected && targetStep > 0) {
          targetStep--;
      } else if (!soundDetected && targetStep < NUM_STEPS - 1) {
          targetStep++;
      }

      // Mover suavemente hacia el paso objetivo (invertido)
      if (currentPosition > targetStep) {
          currentPosition -= RISE_SPEED;
      } else if (currentPosition < targetStep) {
          currentPosition += FALL_SPEED;
      }

      // Actualizar los LEDs (invertido)
      int newStep = round(currentPosition);
      if (newStep != currentStep) {
          // Apagar todos los LEDs por debajo del nuevo paso
          for (int i = 0; i < newStep; i++) {
              reset_step(i);
          }
          // Encender todos los LEDs desde el nuevo paso hasta arriba
          for (int i = newStep; i < NUM_STEPS; i++) {
              set_step(i, colorin);  // Puedes cambiar el color aquí
          }
          currentStep = newStep;
      }
  }

void COLORHANDLER_::block_silence_SA() {

    static unsigned long lastUpdateTime = 0;
    static int currentStep = 0;  // Comienza desde el paso más bajo
    static int targetStep = 0;
    static float currentPosition = 0;
    static int colorIndex = 0;  // Índice del color actual (0 a 7)
    const float RISE_SPEED = 0.4;  // Velocidad de subida (ajusta según necesites)
    const float FALL_SPEED = 0.2;  // Velocidad de caída (ajusta según necesites)
    const int UPDATE_INTERVAL = 10;  // Intervalo de actualización en ms

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < UPDATE_INTERVAL) return;  // Aún no es tiempo de actualizar

    lastUpdateTime = currentTime;

    // Detectar sonido
    bool soundDetected = doitMic.detect_sound_threshold();

    // Actualizar el paso objetivo
    if (soundDetected && targetStep < NUM_STEPS - 1) {
        targetStep++;
    } else if (!soundDetected && targetStep > 0) {
        targetStep--;
    }

    // Mover suavemente hacia el paso objetivo
    if (currentPosition < targetStep) {
        currentPosition += RISE_SPEED;
    } else if (currentPosition > targetStep) {
        currentPosition -= FALL_SPEED;
    }

    // Actualizar los LEDs
    int newStep = round(currentPosition);
    if (newStep != currentStep) {
        // Si se ilumina el último bloque, cambiar el color
        if (newStep == NUM_STEPS - 1 && currentStep != NUM_STEPS - 1) {
            colorIndex = (colorIndex + 1) % 8;  // Incrementar índice y volver a 0 si llega a 8
        }

        CRGB currentColor = colorHandler.get_CRGB_from_colorList(colorIndex);  // Obtener el color actual según el índice

        // Encender todos los LEDs por debajo del nuevo paso
        for (int i = 0; i <= newStep; i++) {
            set_step(i, currentColor);  // Usar el color actual
        }
        // Apagar todos los LEDs desde el nuevo paso hasta arriba
        for (int i = newStep + 1; i < NUM_STEPS; i++) {
            reset_step(i);
        }
        currentStep = newStep;
    }
}


void COLORHANDLER_::block_silence(CRGB colorin) {

    static unsigned long lastUpdateTime = 0;
    static int currentStep = 0;         // Comienza desde el paso más bajo
    static int targetStep = 0;
    static float currentPosition = 0;
    const float RISE_SPEED = 0.4;       // Velocidad de subida (ajusta según necesites)
    const float FALL_SPEED = 0.2;       // Velocidad de caída (ajusta según necesites)
    const int UPDATE_INTERVAL = 10;     // Intervalo de actualización en ms

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < UPDATE_INTERVAL) return;  // Aún no es tiempo de actualizar

    lastUpdateTime = currentTime;

    // Detectar sonido
    bool soundDetected = doitMic.detect_sound_threshold();

    // Actualizar el paso objetivo
    if (soundDetected && targetStep < NUM_STEPS - 1) {
        targetStep++;
    } else if (!soundDetected && targetStep > 0) {
        targetStep--;
    }

    // Mover suavemente hacia el paso objetivo
    if (currentPosition < targetStep) {
        currentPosition += RISE_SPEED;
    } else if (currentPosition > targetStep) {
        currentPosition -= FALL_SPEED;
    }

    // Actualizar los LEDs
    int newStep = round(currentPosition);
    if (newStep != currentStep) {
        // Encender todos los LEDs por debajo del nuevo paso
        for (int i = 0; i <= newStep; i++) {
            set_step(i, colorin);  // Puedes cambiar el color aquí
        }
        // Apagar todos los LEDs desde el nuevo paso hasta arriba
        for (int i = newStep + 1; i < NUM_STEPS; i++) {
            reset_step(i);
        }
        currentStep = newStep;
    }
}

void COLORHANDLER_::matrix_draw_circle(int centerX, int centerY, int radius, CRGB color) {
    auto getLedIndex = [](int x, int y) -> int {
        if (y < 0 || y >= NUM_STEPS || x < 0 || x >= LEDS_STEP) return -1; // Fuera de rango
        if (y % 2 == 0) {
            // Escalón con dirección izquierda a derecha
            return y * LEDS_STEP + x;
        } else {
            // Escalón con dirección derecha a izquierda
            return (y + 1) * LEDS_STEP - x - 1;
        }
    };

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        int indices[] = {
            getLedIndex(centerX + x, centerY + y),
            getLedIndex(centerX + y, centerY + x),
            getLedIndex(centerX - y, centerY + x),
            getLedIndex(centerX - x, centerY + y),
            getLedIndex(centerX - x, centerY - y),
            getLedIndex(centerX - y, centerY - x),
            getLedIndex(centerX + y, centerY - x),
            getLedIndex(centerX + x, centerY - y)
        };

        for (int i = 0; i < 8; i++) {
            if (indices[i] >= 0 && indices[i] < NUM_LEDS) {
                leds[indices[i]] = color;
            }
        }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }

    FastLED.show(); 
}

void COLORHANDLER_::matrix_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, CRGB color) {
    auto getLedIndex = [](int x, int y) -> int {
        if (y < 0 || y >= NUM_STEPS || x < 0 || x >= LEDS_STEP) return -1; // Fuera de rango
        if (y % 2 == 0) {
            // Escalón con dirección izquierda a derecha
            return y * LEDS_STEP + x;
        } else {
            // Escalón con dirección derecha a izquierda
            return (y + 1) * LEDS_STEP - x - 1;
        }
    };

    auto drawLine = [&](int x0, int y0, int x1, int y1) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            int index = getLedIndex(x0, y0);
            if (index >= 0 && index < NUM_LEDS) {
                leds[index] = color;
            }
            if (x0 == x1 && y0 == y1) break;
            int e2 = err * 2;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    };

    // Dibuja las tres líneas del triángulo
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);

    FastLED.show(); // Actualiza los LEDs después de dibujar el triángulo
}


#endif