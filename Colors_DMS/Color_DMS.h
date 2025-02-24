    #pragma once

#ifndef COLOR_DMS_H
#define COLOR_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include <FastLED.h>

/*
 ██████╗ ██████╗ ██╗      ██████╗ ██████╗ ███████╗
██╔════╝██╔═══██╗██║     ██╔═══██╗██╔══██╗██╔════╝
██║     ██║   ██║██║     ██║   ██║██████╔╝███████╗
██║     ██║   ██║██║     ██║   ██║██╔══██╗╚════██║
╚██████╗╚██████╔╝███████╗╚██████╔╝██║  ██║███████║
 ╚═════╝ ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
*/


extern float varaux;


const unsigned int listaColoresPasivos[]=
{

    0xFF2300, // rojo_anaranjado_t
    0xFF2500, // marron_oscuro_t
    0xFF4005, // marron_t
    0xFF4500, // naranja_t
    0xFF7E00, // amarillo_anaranjado_t
    0xFFDF00, // verde_amarillo_t
    0x806600, // crema_t
    0x66FF00, // verde_claro_t
    0x7FA300, // verde_oliva_claro_t
    0x758A00, // verde_oliva_t
    0x7FA364, // verde_grisaceo_t
    0x7FFF64, // verde_lima_t
    0x98FF98, // verde_menta_t
    0x50C878, // esmeralda_t
    0x0DBA98, // verde_azulado_t
    0x1F3438, // azul_verdoso_t
    0x40E0D0, // turquesa_t
    0x00FFFF, // cyan_t
    0x5080FF, // celeste_claro_t
    0x330099, // indigo_t
    0x9400D3, // morado_t
    0x8F00FF, // lila_t
    0xFF00D2, // rosa_t
    0xFF1493, // rosa_oscuro_t
    0xE4007C, // rosa_fuerte_t
    0xD9017A, // magenta_t
    0xE73410  // salmon_t
};


const unsigned int listaColoresPasivos_COPYRIGHT[]={

    0x05F016, // DOIT
    0x3A007C, // CADSBURY
    0xDA1884, // ROSA BARBIE
    0x00843D, // CORTE INGLES
    0xF40009, // COCA-COLA
    0x0033A0, // YVES KLEIN
    0x582C83, // MILKA
    0x81D8D0, // TIFFANYS BLUE
    0xF2CA00  // CAT
};

const unsigned int listaColoresPasivos_3[]=
{

0xFF0000, // pure_red
0xFF1E1E, // bright_red
0xFF4242, // coral_red

0xFF6B00, // vivid_orange
0xFF8C1A, // amber

0xFFD700, // golden_yellow
0xFFEB3B, // bright_yellow

0xB4D335, // lime_yellow
0x7FBA00, // chartreuse

0x00FF00, // pure_green
0x00D364, // spring_green
0x009B77, // forest_green

0x00CED1, // turquoise
0x20B2AA, // sea_green

0x0099FF, // sky_blue
0x0066CC, // ocean_blue
0x0033FF, // royal_blue

0x6A0DAD, // purple
0x8B00FF, // violet

0xFF00FF  // magenta
};


const unsigned int listaColores[36] = 
{
    0xFFFFAA, // white_t 0
    0xFF9B00, // yellow_t 1 
    0xFF5900, // orange_t 2
    0xFF0000, // red_t 3
    0xFF00D2, // violet_t 4
    0x0000FF, // blue_t 5
    0x00FFC8, // light_blue_t 6
    0x00FF00, // green_t 7
    0x000000, // black_t 8

    0xFF2300, // rojo_anaranjado_t
    0xFF2500, // marron_oscuro_t
    0xFF4005, // marron_t

    0xFF4500, // naranja_t
    0xFF7E00, // amarillo_anaranjado_t

    0xFFDF00, // verde_amarillo_t
    0x806600, // crema_t

    0x66FF00, // verde_claro_t
    0x7FA300, // verde_oliva_claro_t
    0x758A00, // verde_oliva_t
    0x7FA364, // verde_grisaceo_t
    0x7FFF64, // verde_lima_t
    0x98FF98, // verde_menta_t
    0x50C878, // esmeralda_t
    0x0DBA98, // verde_azulado_t

    0x1F3438, // azul_verdoso_t
    0x40E0D0, // turquesa_t
    0x00FFFF, // cyan_t
    0x5080FF, // celeste_claro_t

    0x330099, // indigo_t

    0x9400D3, // morado_t
    0x8F00FF, // lila_t
    0xFF00D2, // rosa_t
    0xFF1493, // rosa_oscuro_t
    0xE4007C, // rosa_fuerte_t
    0xD9017A, // magenta_t
    0xE73410  // salmon_t
};



class COLORHANDLER_ {

    public:
        COLORHANDLER_();
        void begin(int numLeds);

        void set_step(int numStep, CRGB color);
        void reset_step(int numStep);
        void result_win(bool superWin);
        void result_fail();

        CRGB get_CRGB_from_colorList(int index);
        CRGB get_CRGB_from_pasiveColorList(int index);
        CRGB targetColor;

        void fade_in_out(byte red, byte green, byte blue);
        void set_is_paused        (bool pau);
        bool get_is_paused        ();

        void set_passive          (bool pas);
        bool get_passive          ();

        void set_slow_passive(bool slow){slowPassive= slow;}
        bool get_slow_passive()         {return slowPassive;}

        void set_targetColor      (CRGB color);
        void set_targetFade       (uint16_t fade);

        CRGB get_currentColor     (){return currentColor;}
        byte get_current_red      (CRGB color){return color.r;}
        byte get_current_green    (CRGB color){return color.g;}
        byte get_current_blue     (CRGB color){return color.b;}

        void set_targetBrightness (byte brigthin);
        byte get_targetBrightness ();

        void set_activePattern (byte patternin);
        byte get_activePattern ();

        void set_currentBrightness(byte brigthin);
        byte get_currentBrightness();

        void setPatternBotonera(byte mode, DynamicLEDManager& ledManager);
        void set_botoneraPattern(byte patternin);

        void welcomeEffect();

        void elem_color_action();
        CRGB* leds;
        bool transitioning; 
        unsigned long transitionStartTime;

        void set_numLedsToLight(int num) { numLedsToLight = num; }
        int get_numLedsToLight() const { return numLedsToLight; }

        void set_numSegments(int segments) { numSegments = segments; }
        int get_numSegments() const { return numSegments; }

        //T4A FastLed FX
        void showStrip ();
        void setPixel  (int Pixel, byte red, byte green, byte blue);
        void setAll    (byte red, byte green, byte blue);

        void Fire(int Cooling, int Sparking, int SpeedDelay);
        void setPixelHeatColor (int Pixel, byte temperature);
        void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
        void meteorRain2(bool launchNewMeteor, CRGB meteorColor);
        void meteorRain2_random(bool launchNewMeteor);
        void fadeToBlack(int ledNo, byte fadeValue);
        void theaterChaseRainbow(int SpeedDelay);
        byte * Wheel(byte WheelPos);
        void BouncingBalls_PSA(byte red, byte green, byte blue, int BallCount);
        void RunningLights(byte red, byte green, byte blue, int WaveDelay, float peakFactor, float frequencyFactor);
        void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
        void rainbowCycle(int SpeedDelay);
        void FillLEDsFromPaletteColors( uint8_t colorIndex);

        void sequencer_game(byte& colorin);
        void simon_game(byte& colorin); 

        void matrix_draw_circle(int centerX, int centerY, int radius, CRGB color);
        void matrix_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, CRGB color);
        
        #ifdef MIC
            void BouncingBalls(byte red, byte green, byte blue);
            void tone_color();
            void tone_game();
            void speak_game();
            void voice_meteors();
            void block_speak(CRGB colorin); 
            void block_speak_SA(); 
            void block_silence(CRGB colorin);
            void block_silence_SA();
        #endif
        int numLeds;
        void setCurrentFile(const String& file) { currentFile = file; } 
    private:

    unsigned long lastUpdate = 0;
    int position = 0;
    bool isAnimating = false;
    
    int numLedsToLight;  // Número de LEDs a encender
    int numSegments;     // Número de tramos

    bool paused;
    bool passive;
    bool slowPassive;
    byte activePattern;
    CRGB startColor;
    CRGB currentColor;
  
    CRGB currentLEDColor;
    byte targetBrightness;
    byte currentBrightness;

    uint16_t fadeTime;

    unsigned long lastUpdateTime;
    String currentFile;

};

extern COLORHANDLER_ colorHandler;


#endif