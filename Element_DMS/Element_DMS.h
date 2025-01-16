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
        void begin(); 

        unsigned long lastLifeTimeUpdate = 0;
        unsigned long workTimeStart = 0;
        bool workTimerRunning = false;
        bool stopwatchRunning = false;
        byte activePattern;
        static const int MAX_EVENTS = 1000;
        //static unsigned long lastEventTime;
        unsigned long lastStartTime = 0;
        unsigned long lastModeChangeTime = 0;
        unsigned long lastColorChangeTime = 0;
        int lastMode = -1;
        int lastColor = -1;
        unsigned long* lastEventTime;  // Array para almacenar los tiempos del último evento
        int* lastEventValue;

        void          start_working_time(); 
        void          stopAndSave_working_time();  
        void          work_time_handler(byte colorin);
        void          lifeTime_update();
        unsigned long get_lifeTime();
        void          set_lifeTime(unsigned long lifeTime);

        int           get_workTime();
        void          set_workTime(int workTime);

        String        get_serial_from_file();

        byte          get_ID_from_file();
        void          set_ID_to_file(byte ID);

        void          set_ID(byte IDin);
        byte          get_ID();
  
        void          set_type(byte typein);
        byte          get_type();

        void          set_manager(byte managerin);
        byte          get_manager();

        void          set_mode(uint8_t mode);
        byte          get_currentMode();

        void          set_flag(byte flagNum, bool state);
        byte          get_flag();

        void            event_register_update(int eventNumber, int eventValue);
        void            print_event_register();
        String          get_word_from_eventNum(int eventNumber);

        uint8_t  ID;

        void            configurar_RF(int baudRate);
        void            save_event_register();
        void            agregar_evento(byte eventType, int eventData);

    protected:
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}
    
        struct EVENT_REGISTER_T {
            byte type;
            int value;
            int duration;
        };
        std::vector<EVENT_REGISTER_T> eventVector;
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