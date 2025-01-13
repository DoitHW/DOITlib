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

        void          start_working_time(); 
        void          stopAndSave_working_time();  
        void          work_time_handler(byte colorin);
        void          lifeTime_update();
        int           get_lifeTime();
        void          set_lifeTime(int lifeTime);

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

        byte          get_serialNum(byte ml);
        INFO_PACK_T   get_info_pack(byte typein, byte languajein);
        INFO_STATE_T  get_state_pack();


        uint8_t  ID;
    protected:
        virtual void  inic_elem_config(){}
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}
    



        uint8_t  name[24];      
        uint8_t  serialNum[2];  
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