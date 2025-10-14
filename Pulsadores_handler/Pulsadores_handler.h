#pragma once
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>

// Definiciones de filas y columnas
#define FILAS 4
#define COLUMNAS 3

// Declarar las variables de pines y colores
extern int filas[FILAS];
extern int columnas[COLUMNAS];

extern byte pulsadorColor[FILAS][COLUMNAS];
extern byte relay_state;
enum ButtonEventType { BUTTON_PRESSED, BUTTON_RELEASED };

class PulsadoresHandler {
public:
    PulsadoresHandler();
    void begin();                      // Inicializa los pines
    static void limpiarEstados();
    void procesarPulsadores();
    bool relayButtonIsPressed() const { return relayButtonPressed; }
    bool isButtonPressed(byte color);
    byte lastBasicColor = 0; // Inicialmente 0 (o BLACK)
    bool isBlueButtonPressed() { return blueButtonPressed; }
    static bool isButtonEnabled(uint8_t ledIdx); 
    static void setButtonActiveMask(const bool mask[9]);   // copia completa (0..8)
    static void setResponseRoute(uint8_t targetType, const TARGETNS& targetNS);
    static void clearResponseRoute();                                          
    static bool isResponseRouteActive();     
private:
    bool relayButtonPressed = false;
    bool blueButtonPressed = false;  // Estado del botón azul
    // Función auxiliar para procesar un evento de un botón
    void processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           uint8_t targetType, const TARGETNS& targetNS);
                                           // === Nueva API global para enmascarar botones ===
    
    static bool     s_responseModeEnabled;
    static uint8_t  s_respTargetType;
    static TARGETNS s_respTargetNS;


};

extern PulsadoresHandler pulsadores;
extern std::vector<uint8_t> idsSPIFFS;   // IDs ordenadas               (solo Comunicador)
extern int  relayStep;              // -1 = BROADCAST encendido
//extern uint8_t communicatorActiveID;

extern std::vector<TARGETNS> nsSPIFFS;
extern uint8_t  communicatorTargetType;
extern TARGETNS communicatorActiveNS;
