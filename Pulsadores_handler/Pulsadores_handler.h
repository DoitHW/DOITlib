#pragma once
#include <Arduino.h>

// Definiciones de filas y columnas
#define FILAS 4
#define COLUMNAS 3

// Declarar las variables de pines y colores
extern int filas[FILAS];
extern int columnas[COLUMNAS];

extern byte pulsadorColor[FILAS][COLUMNAS];

class PulsadoresHandler {
public:
    PulsadoresHandler();
    void begin();                      // Inicializa los pines
    byte leerPulsador();               // Lee la matriz y devuelve el color del pulsador presionado
    void mostrarColor(byte color);     // Muestra información del pulsador presionado en el monitor serial
    static void limpiarEstados();
    
};
