#pragma once

#include <SPIFFS_handler/SPIFFS_handler.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <vector>

// Pines del encoder
#define ENC_A 26
#define ENC_B 34
#define ENC_BUTTON 33

struct ModeFlags {
    bool modeExist;
    bool nop2;
    bool nop1;
    bool acceptsPatterns;
    bool acceptsBankFile;
    bool canAnswer;
    bool hasPassive;
    bool situatedHigh;

    bool acceptsSensVal2;
    bool acceptsSensVal1;
    bool hasRelay4;
    bool hasRelay3;
    bool hasRelay2;
    bool hasRelay1;
    bool acceptsAdvancedColor;
    bool acceptsBasicColor;
};


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


ModeFlags extractModeFlags(const uint8_t modeConfig[2]);

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
extern int globalVisibleModesMap[17];  // Declaración de la variable global
extern bool ignoreInputs;
extern ESP32Encoder encoder;




