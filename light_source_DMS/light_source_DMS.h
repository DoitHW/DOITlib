#pragma once
#ifndef COLUMN_DMS_H
#define COLUMN_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>

// test de commit
class LIGHTSOURCE_ : public ELEMENT_{

    public:
        LIGHTSOURCE_();

        void lightsource_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void fan_relay_handler(byte color);
};

extern LIGHTSOURCE_ *element;

#endif