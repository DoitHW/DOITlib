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
        BOTONERA_();

        void botonera_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void sectorIn_handler(std::vector<byte> data, byte tragetin);
        void print_info_pack(const INFO_PACK_T *infoPack);
        bool serialExistsInSPIFFS(byte serialNum[5]);
        void iniciarEscaneoElemento(const char* mensajeInicial);
        void actualizarBarraProgreso(float progreso);
        void finalizarEscaneoElemento();
        void dibujarMarco(uint16_t color);
        void mostrarMensajeTemporal(int respuesta, int dTime);
        byte getNextAvailableID();

        byte validar_serial();
        void procesar_datos_sector(LAST_ENTRY_FRAME_T &LEF, int sector, INFO_PACK_T* infoPack);
        bool guardar_elemento(INFO_PACK_T* infoPack);
        void reasignar_id_elemento(INFO_PACK_T* infoPack = nullptr);
        void validar_elemento();
        bool esperar_respuesta(unsigned long timeout);
        void actualizar_elemento_existente() ;
        bool procesar_y_guardar_elemento_nuevo(byte targetID);
        bool procesar_sector(int sector, INFO_PACK_T* infoPack, uint8_t targetID);
        bool confirmarCambioID(byte nuevaID);
        byte getIdFromSPIFFS(byte *serial) ;

    private:   
        byte lastAssignedID = DEFAULT_DEVICE;
        byte lastSerial[5] = {0};

};


extern BOTONERA_ *element;



#endif