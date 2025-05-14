#pragma once

#include <SPIFFS_handler/SPIFFS_handler.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <vector>

// Pines del encoder            //BOTONERA FRANC        BOTONERA MARC
#define ENC_A 26                //26                    33                      (33 para nuevas botoneras sin placa)
#define ENC_B 34                //34                    34                      (34 para nuevas botoneras sin placa)
#define ENC_BUTTON 33           //33                    26                      (26 para nuevas botoneras sin placa)


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
//void requestAndSyncElementMode();

bool getModeFlag(const uint8_t modeConfig[2], MODE_CONFIGS flag);
void debugModeConfig(const uint8_t modeConfig[2]);

std::vector<bool> initializeAlternateStates(const String &currentFile);

void handleBankSelectionMenu(std::vector<byte>& bankList, std::vector<bool>& selectedBanks);

void handleBrightnessMenu();

void handleSoundMenu();

void handleFormatMenu();

void handleDeleteElementMenu();

void handleConfirmDelete();

void handleConfirmRestoreMenu();

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
extern bool modeAlternateActive;

extern unsigned long lastDisplayInteraction; // Última vez que se interactuó con la pantalla
extern bool displayOn;                    // Estado de la pantalla (encendida por defecto)
extern unsigned long encoderIgnoreUntil; // Tiempo hasta el cual se ignoran las entradas del encoder
extern bool systemLocked;


// Variables para el submenú de selección de idioma
extern bool languageMenuActive;
extern int languageMenuSelection;  // Índice de la opción seleccionada (0 a 5)

extern bool formatSubMenuActive;
extern int formatMenuSelection;


extern bool soundMenuActive;
extern int soundMenuSelection;
extern byte selectedVoiceGender;
extern bool negativeResponse;
extern byte selectedVolume;

extern bool deleteElementMenuActive;
extern int deleteElementSelection;
extern std::vector<String> deletableElementFiles;
extern bool confirmDeleteActive;
extern int confirmSelection;
extern bool confirmDeleteMenuActive;
extern bool ignoreNextEncoderClick;

extern bool confirmRestoreMenuActive;
extern int confirmRestoreSelection;
 







