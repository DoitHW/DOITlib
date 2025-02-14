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
void handleHiddenMenuNavigation(int &hiddenMenuSelection);

// Declaración de funciones externas (forward declarations)
void animateTransition(int direction);
void drawModesScreen();
void drawCurrentElement();

void handleModeSelection(const String& currentFile);
void toggleElementSelection(const String& currentFile);
void requestAndSyncElementMode();

// Variables externas requeridas
extern std::vector<String> elementFiles;
extern std::vector<bool> selectedStates;
extern int currentIndex;
extern int totalModes;
extern int currentModeIndex;
extern bool inModesScreen;
extern bool modeScreenEnteredByLongPress;
extern bool isLongPress;
extern bool hiddenMenuActive;
extern unsigned long buttonPressStart;
extern int globalVisibleModesMap[16];  // Declaración de la variable global




