#pragma once
#ifndef LEDSTRIP_DMS_H
#define LEDSTRIP_DMS_H

#include "defines_DMS.h"
#include "Element_DMS.h"
#include <FastLED.h>
#include <Arduino.h>
#include "Frame_DMS.h"
#include <vector>



enum LEDSTRIP_MODE_LIST{
    LEDSTRIP_CONTEST_MODE= 0,
    LEDSTRIP_BASIC_MODE,
    LEDSTRIP_FAST_MODE,
    LEDSTRIP_MOTION_MODE,
    LEDSTRIP_RB_MOTION_MODE,
    LEDSTRIP_MIX_MODE,
    LEDSTRIP_PASSIVE_MODE,
    LEDSTRIP_RELAX_MODE,
    LEDSTRIP_DRIVEPARK_MODE,
    LEDSTRIP_BALL_MODE,
    LEDSTRIP_FILLSTRIP_MODE
};

class LEDSTRIP_ : public ELEMENT_{

    public:
        LEDSTRIP_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_LEDSTRIP);
        }

        void inic_elem_config()override;
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void element_action()override;

};

extern LEDSTRIP_ *element;

#endif