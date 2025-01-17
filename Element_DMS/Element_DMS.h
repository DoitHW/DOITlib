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

extern byte globalID;

class ELEMENT_ {

    public: 
        ELEMENT_();
        virtual ~ELEMENT_();

        uint8_t  ID;
        byte activePattern;


        void      begin();
        void      configurar_RF(int baudRate);

        void      event_register(int eventNum, int eventVal);
        void      save_register();
        void      print_event_register();
        String    get_word_from_eventNum(int eventNumber);
        String    get_serial_from_file();

        byte      get_ID_from_file();
        void      set_ID_to_file(byte ID);

        void      set_ID(byte IDin);
        byte      get_ID();
  
        void      set_type(byte typein);
        byte      get_type();

        void      set_manager(byte managerin);
        byte      get_manager();

        void      set_mode(uint8_t mode);
        byte      get_currentMode();

        void      set_flag(byte flagNum, bool state);
        byte      get_flag();

    protected:
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}
    
        enum EVENT_TYPE{
            EV_START,
            EV_END,
            EV_SECTOR_REQ,
            EV_MODE_CHANGE,
            EV_COLOR_CHANGE,
            EV_FLAG_CHANGE,
            EV_ID_CHANGE
        };

        struct EVENT_T{
            EVENT_TYPE eventNum;
            int eventVal;
            unsigned long timestamp;
        };

        std::vector<EVENT_T> events;
        unsigned long totalColorTime = 0;
        unsigned long totalWorkTime = 0;
        unsigned long lastStartTime = 0;
        unsigned long lastModeChangeTime = 0;
        unsigned long lastColorChangeTime = 0;
        int lastMode = -1;
        int lastColor = -1;
        int sectorReqCount = 0;
        int flagChangeCount = 0;
        unsigned long totalColorTimeAllCycles = 0;
        unsigned long totalWorkTimeAllCycles = 0;
        int totalCycles = 0;

        uint8_t  name[24];      
        uint8_t  serialNum[5];  
        byte     flag;
        uint8_t  currentMode;  
        byte     type;
        byte     exclusiveIDmanager;

        uint64_t workedTime;
        uint64_t onStartTime;

        unsigned long stopwatchStartTime = 0; // Variable para almacenar el tiempo de inicio del cronómetro
        bool canStartStopwatch = true;       // Controla si se permite iniciar el cronómetro     

    private:

};


#endif







