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
#include <play_DMS/play_DMS.h>


class BOTONERA_ : public ELEMENT_{

    public:
        BOTONERA_();

        void botonera_begin();
        void RX_main_handler(LAST_ENTRY_FRAME_T LEF)override;
        void sectorIn_handler(std::vector<byte> data, byte tragetin);
        void print_info_pack(const INFO_PACK_T *infoPack);
        bool serialExistsInSPIFFS(byte serialNum[5]);
        //void iniciarEscaneoElemento(const char* mensajeInicial);
        //void actualizarBarraProgreso(float progreso);
        //void actualizarBarraProgreso(float progreso, const char* detalleTexto = nullptr);
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
        byte getIdFromSPIFFS(byte *serial);
        String getCurrentFilePath(byte elementID);
        void printFrameInfo(LAST_ENTRY_FRAME_T LEF);
        void activateCognitiveMode();
        void deactivateCognitiveMode();

        ////////////////////////////////

        void escanearSala();
        bool procesar_y_guardar_elemento_nuevo(byte targetID, const byte serialNumDelElemento[5]);
        byte buscarPrimerIDLibre(const bool ocupadas[32]);
        void actualizarIDenSPIFFS(const byte serial[5], byte nuevaID);
        bool escanearID(byte targetID, byte serial[5], unsigned long timeoutPerAttempt, int retries);
        bool elementoAsignadoA_ID_enSPIFFS(byte idToFind);
        void iniciarEscaneoElemento(const char* mensajeInicial);
        void actualizarBarraProgreso(int pasoActual,int pasosTotales,const char* etiqueta = nullptr);
        
    
        

    private:   
        byte lastAssignedID = DEFAULT_DEVICE;
        byte lastSerial[5] = {0};

};


extern BOTONERA_ *element;
extern DOITSOUNDS_ doitPlayer;
extern byte currentCognitiveCommand;
extern bool inCognitiveMenu;



#endif