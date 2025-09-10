#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <string>
#include <Frame_DMS/Frame_DMS.h>
#include <Translations_handler/translations.h>
#include <play_DMS/play_DMS.h>
#include <token_DMS/token_DMS.h>
#include <stdint.h>   // en C o C++


// Cálculo de offsets dentro de INFO_PACK_T:
const size_t OFFSET_NAME = 0;                              
const size_t OFFSET_DESC = OFFSET_NAME + 24;               // 24+192 =216
const size_t OFFSET_SERIAL = OFFSET_DESC + 192;            // 216+2=218
const size_t OFFSET_ID = OFFSET_SERIAL + 5;                // 218+1=219
const size_t OFFSET_CURRENTMODE = OFFSET_ID + 1;           // 219+1=220
const size_t SIZE_MODE = 24 + 192 + 2; // 218 bytes por modo
const size_t SIZE_MODES = SIZE_MODE * 16; // 3488 bytes
const size_t OFFSET_MODES = OFFSET_CURRENTMODE + 1;  // 220 +0=220
// Icono empieza en 220 + 3488 = 3708
const size_t OFFSET_ICONO = OFFSET_MODES + SIZE_MODES; // 220 +3488=3708
const size_t OFFSET_SITUACION = OFFSET_ICONO + (ICON_ROWS * ICON_COLUMNS * 2); // 3708 + 8192 = 11900

// icono =64*64*2=8192 bytes total ~11900

extern uint16_t lineBuffer[64]; 
extern INFO_PACK_T ambientesOption;
extern INFO_PACK_T fichasOption;
extern INFO_PACK_T apagarSala;
extern INFO_PACK_T comunicadorOption;
extern TOKEN_ token;
extern DOITSOUNDS_ doitPlayer;



bool readElementData(fs::File& f, char* elementName, char* modeName, int& startX, int& startY);
bool writeBytesChecked(fs::File &f, const uint8_t* data, size_t length);
void formatSPIFFS();
String generateUniqueFileName(const char* baseName);

bool readElementData(fs::File& f, char* elementName, char* modeName, int& startX, int& startY);
void loadElementsFromSPIFFS();
void initializeDynamicOptions();
byte getCurrentElementID();
bool isCurrentElementSelected();
bool checkMostSignificantBit(byte modeConfig[2]);
bool getModeConfig(const String& fileName, byte mode, byte modeConfig[2]);
void setAllElementsToBasicMode();
void updateBankList(byte bank);
void updateBankAndFamilyList(byte bank, const char* familyName);
String getFamilyNameFromBank(byte bank);
void saveBrightnessToSPIFFS(uint8_t value);
uint8_t loadBrightnessFromSPIFFS();
std::vector<byte> readBankList();
void loadDeletableElements();
void saveLanguageToSPIFFS(Language lang);
Language loadLanguageFromSPIFFS();
void saveSoundSettingsToSPIFFS();
void loadSoundSettingsFromSPIFFS();

// ===== Elementos Adicionales (persistencia) =====
struct ExtraElementsConfig {
    bool dadoEnabled;
};

void saveExtraElementsConfig(const ExtraElementsConfig& cfg);
ExtraElementsConfig loadExtraElementsConfig();
bool isDadoEnabled();
void setDadoEnabled(bool enabled);

// Elemento fijo en RAM
extern INFO_PACK_T dadoOption;
