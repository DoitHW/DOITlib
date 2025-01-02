#pragma once
#ifndef LEDSTRIP_DMS_H
#define LEDSTRIP_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>


class LEDSTRIP_ : public ELEMENT_{

    public:
        LEDSTRIP_(uint16_t serialNumber);

        void ledstrip_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
};


extern LEDSTRIP_ *element;


#endif