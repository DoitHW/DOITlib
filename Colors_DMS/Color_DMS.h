#pragma once

#ifndef COLOR_DMS_H
#define COLOR_DMS_H
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>

/*
 ██████╗ ██████╗ ██╗      ██████╗ ██████╗ ███████╗
██╔════╝██╔═══██╗██║     ██╔═══██╗██╔══██╗██╔════╝
██║     ██║   ██║██║     ██║   ██║██████╔╝███████╗
██║     ██║   ██║██║     ██║   ██║██╔══██╗╚════██║
╚██████╗╚██████╔╝███████╗╚██████╔╝██║  ██║███████║
 ╚═════╝ ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
*/



const unsigned int listaColores[36] = 
{
    0xFFFFFF, // white_t 0
    0xFF9B00, // yellow_t 1 
    0xFF5900, // orange_t 2
    0xFF0000, // red_t 3
    0xFF00D2, // violet_t 4
    0x0000FF, // blue_t 5
    0x00FFC8, // light_blue_t 6
    0x00FF00, // green_t 7
    0x000000, // black_t 8

    0x806600, // crema_t
    0xFF00D2, // rosa_t
    0x8F00FF, // lila_t
    0x5080FF, // celeste_claro_t
    0x40E0D0, // turquesa_t
    0x66FF00, // verde_claro_t
    0xFF4500, // naranja_t
    0xFF1493, // rosa_oscuro_t
    0x758A00, // verde_oliva_t
    0x7FFF64, // verde_lima_t
    0xFFDF00, // verde_amarillo_t
    0xD9017A, // magenta_t
    0x9400D3, // morado_t
    0x0DBA98, // verde_azulado_t
    0xFF4005, // marron_t
    0x330099, // indigo_t
    0x1F3438, // azul_verdoso_t
    0x50C878, // esmeralda_t
    0x00FFFF, // cyan_t
    0x98FF98, // verde_menta_t
    0xFF7E00, // amarillo_anaranjado_t
    0xFF2300, // rojo_anaranjado_t
    0xE4007C, // rosa_fuerte_t
    0xFF2500, // marron_oscuro_t
    0x7FA364, // verde_grisaceo_t
    0x7FA300, // verde_oliva_claro_t
    0xE73410  // salmon_t
};

const unsigned int listaColoresPasivos[16] = 
{
0xFFFFFF, // white_t (Blanco para iniciar de un tono neutro)
0xFFDF00, // verde_amarillo_t (Amarillo con un toque verde)
0xFF9B00, // yellow_t (Amarillo cálido)
0xFF5900, // orange_t (Naranja)
0xFF0000, // red_t (Rojo puro)
0xD9017A, // magenta_t (Magenta, transición del rojo al rosa)
0xFF00D2, // rosa_t (Rosa vibrante)
0x9400D3, // morado_t (Morado profundo)
0x8F00FF, // lila_t (Lila, una transición suave hacia tonos violetas)
0x0000FF, // blue_t (Azul puro)
0x5080FF, // celeste_claro_t (Azul claro)
0x00FFFF, // cyan_t (Cian, un tono frío de transición)
0x40E0D0, // turquesa_t (Turquesa, mezcla de azul y verde)
0x00FF00, // green_t (Verde puro)
0x50C878, // esmeralda_t (Verde esmeralda)
0x7FFF64 // verde_lima_t (Verde lima para cerrar con un tono brillante)
};

class COLORHANDLER_ {

    public:
        COLORHANDLER_();
        void begin(int numLeds);
        CRGB get_CRGB_from_colorList(int index);

        void set_targetColor      (CRGB color);
        void set_targetFade       (uint16_t fade);
        void set_targetBrightness (byte brightness);

        void action_frank();
        void action();
        CRGB* leds;
        bool transitioning; // Indicates if a transition is active
        unsigned long transitionStartTime;
        
    private:

    int numLeds;

    CRGB startColor;
    CRGB currentColor;
    CRGB targetColor;

    byte targetBrightness;
    byte currentBrightness;

    uint16_t targetFade; // Duration of the fade in milliseconds

    unsigned long lastUpdateTime; // Tracks the last update time
    // Tracks when the transition started
    
};


extern COLORHANDLER_ colorHandler;



#endif