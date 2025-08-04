#pragma once

#ifndef ELEMENT_DMS_H
#define ELEMENT_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <stdint.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <FastLED.h>
#include <vector>
#include <Colors_DMS/Color_DMS.h>
#include <map>
#include <algorithm>
#include <Colors_DMS/Color_DMS.h>

extern byte globalID;
extern bool colorReceived;


class ELEMENT_ {

    public: 
        void   begin();
        void   configurar_RF(int baudRate);

        void      set_type(byte typein);
        byte      get_type();

        //void      set_mode(uint8_t mode);
        byte      get_currentMode();

    protected:
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}
        byte     flag;
        uint8_t  currentMode;  
        byte     type;
};

#endif