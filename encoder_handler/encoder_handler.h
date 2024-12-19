#pragma once

#include <SPIFFS_handler/SPIFFS_handler.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <vector>

// Pines del encoder
#define ENC_A 26
#define ENC_B 34
#define ENC_BUTTON 33

// Declaración de funciones relacionadas con el encoder
void encoder_init_func();
void handleEncoder();

// Declaración de funciones externas (forward declarations)
void animateTransition(int direction);
void drawModesScreen();
void drawCurrentElement();

// Variables externas requeridas
extern std::vector<String> elementFiles;
extern std::vector<bool> selectedStates;
extern int currentIndex;
extern int totalModes;
extern int currentModeIndex;
extern bool inModesScreen;
extern bool modeScreenEnteredByLongPress;
extern bool isLongPress;



