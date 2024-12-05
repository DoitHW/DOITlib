#pragma once

#ifndef FRAME_DMS_H
#define FRAME_DMS_H

#include "defines_DMS.h"
#include <Arduino.h>
#include <stdint.h>
#include <vector>




extern volatile bool startFrameReceived;
extern volatile bool frameInProgress;
extern volatile bool partialFrameReceived;
extern volatile bool frameReceived;
extern std::vector<byte> uartBuffer;

extern volatile unsigned long last_received_time;
extern int lastReceivedTime;

struct LAST_ENTRY_FRAME_T{

    byte origin;
    byte function;
    std::vector<byte> data;
};

struct FRAME_T{

    byte start;
    byte frameLengthMsb;
    byte frameLengthLsb;
    byte origin;
    byte numTargets;
    std::vector<byte> target;
    byte function;
    byte dataLengthMsb;
    byte dataLengthLsb;
    std::vector<byte> data;
    byte checksum;
    byte end;
};

struct SENSOR_VALUE_T{
    byte msb_min;
    byte lsb_min;
    byte msb_max;
    byte lsb_max;
    byte msb_val;
    byte lsb_val;
};

struct COLOR_T{

    byte fade;
    byte brightness;
    byte red;
    byte green;
    byte blue;
};

struct MODE_T{
    
    byte name[24];
    byte desc[192];
    byte config[2];
};

struct INFO_PACK_T{

    byte name[24];
    byte desc[192];
    byte serialNum[2];
    byte ID;
    byte currentMode;
    MODE_T mode[16];
    uint16_t icono[64][64];
};

struct INFO_STATE_T{
    
    byte exclusiveOrigins; // indica si esta configurado para atender exclusivamente a una ID
    byte protectedID;   // indica si la ID ha sido configurada por botonera
    byte currentMode;   
    byte settedFlags;   // indica que flags / reles estan activados
    byte colorFlag;     // indica si esta en un color basico
    byte currentColor;  
    byte customColorFlag;   // indica si esta en un color RGB
    COLOR_T customCurrentColor;
    byte serialNum[2]; 
    byte workingTime[4]; 
    byte lifeTime[4];
};

void IRAM_ATTR handleUARTInterrupt(void* arg);

LAST_ENTRY_FRAME_T extract_info_from_frameIn (std::vector<byte> vectorin);
byte checksum_calc                           (const FRAME_T &framein);
void send_frame                              (const FRAME_T &framein);

byte get_mapped_sensor_value(byte minMSB, byte minLSB, byte maxLSB, byte maxMSB, byte valLSB, byte valMSB);

FRAME_T frameMaker_REQ_ELEM_INFO       (byte targetin, INFO_PACK_T infoPack);
FRAME_T frameMaker_SEND_COLOR          (std::vector<byte>targetin, byte color);
FRAME_T frameMaker_RETURN_ELEM_INFO    (byte targetin, INFO_PACK_T infoPack);
FRAME_T frameMaker_RETURN_ELEM_STATE   (byte targetin, INFO_STATE_T infoState);




void sergi_truchilla();




#endif