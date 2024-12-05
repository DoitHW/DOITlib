#pragma once

#ifndef ELEMENT_DMS_H
#define ELEMENT_DMS_H

#include "defines_DMS.h"
#include <stdint.h>
#include "Frame_DMS.h"
#include <FastLED.h>
#include <vector>


extern uint64_t startTime;
class ELEMENT_ {

    public: 
        ELEMENT_(uint16_t serialNumber){
            serialNum[0] = (serialNumber >> 8) & 0xFF;  // MSB OJO, revisar ordre
            serialNum[1] = serialNumber & 0xFF;
        }

    void set_ID(uint8_t deviceID);
    uint8_t get_ID();
    void set_type(byte typein);
    byte get_type();
    void set_manager(byte managerin);
    byte get_manager();
    void set_mode(uint8_t mode);
    byte get_currentMode();
    byte get_serialNum(byte ml);
    INFO_PACK_T get_info_pack(byte typein, byte languajein);
    INFO_STATE_T get_state_pack();
    uint32_t get_working_time();
    void register_working_time(uint64_t timein);

    protected:
        virtual void inic_elem_config();
        virtual void RX_main_handler(LAST_ENTRY_FRAME_T LEF);
        virtual void element_action();

    private:
        uint8_t name[24];      
        uint8_t serialNum[2];  
        uint8_t ID;            
        uint8_t currentMode;  
        byte    type;
        byte    exclusiveIDmanager;
};


#endif