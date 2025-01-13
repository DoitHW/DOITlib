#pragma once

#ifndef FRAME_DMS_H
#define FRAME_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <stdint.h>
#include <vector>
#include <FastLED.h>

extern volatile bool startFrameReceived;
extern volatile bool frameInProgress;
extern volatile bool partialFrameReceived;
extern volatile bool frameReceived;
extern byte globalID;
extern std::vector<byte> uartBuffer;

extern volatile unsigned long last_received_time;
extern int lastReceivedTime;

void IRAM_ATTR onUartInterrupt();


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
    byte serialNum[5];
    byte ID;
    byte currentMode;
    MODE_T mode[16];
    uint16_t icono[ICON_ROWS][ICON_COLUMNS];
};

struct ICON_PACK_T{
    uint16_t icono[ICON_ROWS][ICON_COLUMNS]; 
};

struct INFO_STATE_T{
    
    byte exclusiveOrigins; // indica si esta configurado para atender exclusivamente a una ID
    byte currentMode;   
    byte settedFlags;   // indica que flags / reles estan activados
    byte currentRed;
    byte currentGreen;
    byte currentBlue; 
    byte serialNum[2]; 
    byte workingTime[4]; 
    byte lifeTime[4];
};

void IRAM_ATTR handleUARTInterrupt(void* arg);

LAST_ENTRY_FRAME_T extract_info_from_frameIn (std::vector<byte> vectorin);
byte checksum_calc                           (const FRAME_T &framein);
void send_frame                              (const FRAME_T &framein);

byte get_mapped_sensor_value(byte minMSB, byte minLSB, byte maxLSB, byte maxMSB, byte valLSB, byte valMSB);

byte get_brightness_from_sensorValue(LAST_ENTRY_FRAME_T LEFin);
byte get_color_from_sensorValue(LAST_ENTRY_FRAME_T LEFin);
float get_aux_var_01_from_sensorValue(LAST_ENTRY_FRAME_T LEFin);

void  get_sector_data(byte *sector_data, byte lang, byte sector);

FRAME_T frameMaker_REQ_ELEM_INFO       (byte originin, byte targetin, byte idiomain, byte sectorin);
//FRAME_T frameMaker_REQ_ELEM_ICON       (byte origin, byte targetin);
FRAME_T frameMaker_REQ_ELEM_STATE      (byte originin, byte targetin);

FRAME_T frameMaker_SET_ELEM_MODE       (byte originin, std::vector<byte>targetin, byte modein);
FRAME_T frameMaker_SET_ELEM_ID         (byte originin, std::vector<byte>targetin, byte IDin);  

FRAME_T frameMaker_SEND_COLOR          (byte originin, std::vector<byte>targetin, byte colorin);
FRAME_T frameMaker_SEND_TEST           (byte originin, std::vector<byte>targetin, byte testin);
FRAME_T frameMaker_SEND_SENSOR_VALUE   (byte originin, std::vector<byte>targetin, SENSOR_VALUE_T sensorin);
FRAME_T frameMaker_SEND_FLAG_BYTE      (byte originin, std::vector<byte>targetin, byte flagin);
FRAME_T frameMaker_SEND_PATTERN_NUM    (byte irigin, std::vector<byte>targetin, byte patternin);
FRAME_T frameMaker_SEND_FILE_NUM       (byte originin, std::vector<byte>targetin, byte bankin, byte filein);

//FRAME_T frameMaker_RETURN_ELEM_INFO(byte originin, byte targetin, INFO_PACK_T* infoPack);
//FRAME_T frameMaker_RETURN_ELEM_ICON    (byte origin, byte targetin, ICON_PACK_T iconPack);
FRAME_T frameMaker_RETURN_ELEM_SECTOR  (byte originin, byte targetin, byte *sector_data, byte sectorin);
FRAME_T frameMaker_RETURN_ELEM_STATE   (byte originin, byte targetin, INFO_STATE_T infoState);
FRAME_T frameMaker_REQ_ELEM_SECTOR(byte originin, byte targetin, byte idiomain, byte sectorin);





#endif