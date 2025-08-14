#pragma once

#include <SPIFFS_handler/SPIFFS_handler.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <vector>

// Pines del encoder            //BOTONERA FRANC        BOTONERA MARC
#define ENC_A 26                //26                    34                      (33 para nuevas botoneras sin placa)
#define ENC_B 34                //34                    33                      (34 para nuevas botoneras sin placa)
#define ENC_BUTTON 33           //33                    26                      (26 para nuevas botoneras sin placa)
                                                        //3V3 desconectado

enum MODE_CONFIGS{
    HAS_BASIC_COLOR= 0,
    HAS_PULSE,
    HAS_ADVANCED_COLOR,
    HAS_RELAY,
    HAS_RELAY_N1,
    HAS_RELAY_N2,
    NOP_1,
    HAS_SENS_VAL_1,
    HAS_SENS_VAL_2,
    NOP_2,
    HAS_PASSIVE,
    HAS_BINARY_SENSORS,
    HAS_BANK_FILE,
    HAS_PATTERNS,
    HAS_ALTERNATIVE_MODE,
    MODE_EXIST
};

// Declaración de funciones relacionadas con el encoder
void encoder_init_func() noexcept;
void handleEncoder() noexcept;
void handleHiddenMenuNavigation(int &hiddenMenuSelection);

// Declaración de funciones externas (forward declarations)
//void animateTransition(int direction);
void drawModesScreen();
void drawCurrentElement();

void handleModeSelection(const String& currentFile) noexcept;
void toggleElementSelection(const String& currentFile);
//void requestAndSyncElementMode();

bool getModeFlag(const uint8_t modeConfig[2], MODE_CONFIGS flag);
void debugModeConfig(const uint8_t modeConfig[2]);

std::vector<bool> initializeAlternateStates(const String &currentFile) noexcept;

void handleBankSelectionMenu(std::vector<byte>& bankList, std::vector<bool>& selectedBanks);

void handleBrightnessMenu();

void handleSoundMenu();

void handleFormatMenu();

void handleDeleteElementMenu();

void handleConfirmDelete();

void handleConfirmRestoreMenu();

void handleConfirmRestoreElementMenu();

bool isInMainMenu() ;

int getTotalModesForFile(const String &file);

static inline uint8_t mapPercentToFastLEDBrightness(uint8_t percent);

void printElementDetails();
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
extern bool confirmRestoreMenuElementActive;
extern int confirmRestoreElementSelection;

extern unsigned long lastFocusChangeTime;
extern int lastQueriedElementIndex;


extern unsigned long lastModeQueryTime;
extern int pendingQueryIndex;
extern uint8_t pendingQueryID;
extern bool awaitingResponse;

extern int formatMenuCurrentIndex;
extern int32_t formatMenuLastValue;

enum BrightnessOption {
    BRIGHTNESS_NORMAL = 0,
    BRIGHTNESS_DIM    = 1
};


 







