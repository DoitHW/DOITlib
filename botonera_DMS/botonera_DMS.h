#pragma once
#ifndef BOTONERA_DMS_H
#define BOTONERA_DMS_H

#include <Colors_DMS/Color_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <FastLED.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>


class BOTONERA_ : public ELEMENT_{

    public:
        BOTONERA_(uint16_t serialNumber);

        void botonera_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void sectorIn_handler(std::vector<byte> data, byte tragetin);
        byte buscar_elemento_nuevo();
        byte anadir_elemento_nuevo(const INFO_PACK_T *infoPack);
        void print_info_pack(const INFO_PACK_T *infoPack);
        bool serialExistsInSPIFFS(byte serialNum[2]);
        

};


extern BOTONERA_ *element;



#endif