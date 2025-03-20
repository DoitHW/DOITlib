#pragma once
#include <Arduino.h>
#include <vector>

// Definiciones de filas y columnas
#define FILAS 4
#define COLUMNAS 3

// Declarar las variables de pines y colores
extern int filas[FILAS];
extern int columnas[COLUMNAS];

extern byte pulsadorColor[FILAS][COLUMNAS];
extern bool relay_state;
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
private:
    bool relayButtonPressed = false;
    bool blueButtonPressed = false;  // Estado del botón azul
    // Función auxiliar para procesar un evento de un botón
    void processButtonEvent(int i, int j, ButtonEventType event,
                            bool hasPulse, bool hasPassive, bool hasRelay,
                            std::vector<byte>& target);
};

extern PulsadoresHandler pulsadores;
