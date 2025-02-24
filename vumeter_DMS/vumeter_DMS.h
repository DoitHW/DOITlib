#pragma once
#ifndef VUMETER_DMS_H
#define VUMETER_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <driver/i2s.h>

class VUMETER_ : public ELEMENT_{

    private:

    CRGB currentColor;
    byte currentBrightness;

    public:
        VUMETER_();

        void vumeter_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;

        void update_vum();

        void set_vumeter_currentMode(byte modein){currentMode= modein;}
        byte get_vumeter_currentMode(){return currentMode;}

        void set_currentColor(byte colorin){currentColor= colorin;}
        CRGB get_currentColor(){return currentColor;}

        void set_currentBrightness(byte bright){currentBrightness= bright;}
        byte get_currentBrightness(){return currentBrightness;}

};


extern VUMETER_ *element;


#endif