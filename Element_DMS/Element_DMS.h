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

class ELEMENT_ {

    public: 
        ELEMENT_(uint16_t serialNumber);
        virtual ~ELEMENT_();

        void begin(); 
        void          reset_config_file();
        void          start_working_time(); 
        void          stopAndSave_working_time();  
        void          set_sinceStart_time(uint64_t);
        uint64_t      get_sinceStart_time();
        unsigned long read_lifeTime_from_file();
        void          write_lifeTime_to_file(unsigned long uptimeInSeconds);
        void          lifeTime_update();

        void          set_ID_protected();
        void          set_ID(uint8_t deviceID);
        uint8_t       get_ID();

        void          set_type(byte typein);
        byte          get_type();

        void          set_manager(byte managerin);
        byte          get_manager();

        void          set_mode(uint8_t mode);
        byte          get_currentMode();

        byte          get_serialNum(byte ml);
        INFO_PACK_T   get_info_pack(byte typein, byte languajein);
        INFO_STATE_T  get_state_pack();

        void          set_default_ID();
        void          set_custom_ID();   

    protected:
        virtual void  inic_elem_config(){}
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}
    



        uint8_t  name[24];      
        uint8_t  serialNum[2];  
        uint8_t  ID;            
        uint8_t  currentMode;  
        byte     type;
        byte     exclusiveIDmanager;

        uint64_t workedTime;
        uint64_t onStartTime;

        bool stopwatchRunning = false;  
        unsigned long stopwatchStartTime = 0; // Variable para almacenar el tiempo de inicio del cronómetro
        bool canStartStopwatch = true;       // Controla si se permite iniciar el cronómetro     

    private:


};


#endif