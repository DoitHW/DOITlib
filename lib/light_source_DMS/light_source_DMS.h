#pragma once
#ifndef LIGHT_SOURCE_DMS_H
#define LIGHT_SOURCE_DMS_H

#include "defines_DMS.h"
#include "Element_DMS.h"
#include <FastLED.h>
#include <Arduino.h>
#include "Frame_DMS.h"
#include <vector>

#define NUM_LEDS     0x01
#define LED_DATA_PIN 0x02


enum LIGHTSOURCE_MODE_LIST{
    LIGHTSOURCE_CONTEST_MODE= 0,
    LIGHTSOURCE_BASIC_MODE,
    LIGHTSOURCE_FAST_MODE,
    LIGHTSOURCE_MOTION_MODE,
    LIGHTSOURCE_RB_MOTION_MODE,
    LIGHTSOURCE_MIX_MODE,
    LIGHTSOURCE_PASSIVE_MODE
};

class LIGHTSOURCE_ : public ELEMENT_{

    public:
        LIGHTSOURCE_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_LIGHTSOURCE);
        }

        void inic_elem_config()override;
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void element_action()override;
};


extern LIGHTSOURCE_ *element;



#endif