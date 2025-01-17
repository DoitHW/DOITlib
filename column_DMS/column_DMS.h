#pragma once
#ifndef COLUMN_DMS_H
#define COLUMN_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>


class COLUMN_ : public ELEMENT_{

    public:
        COLUMN_();

        void column_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void relay_handler(bool actionin);      
};                          


extern COLUMN_ *element;



#endif