#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <string>
#include <Frame_DMS/Frame_DMS.h>

// Cálculo de offsets dentro de INFO_PACK_T:
const size_t OFFSET_NAME = 0;                              
const size_t OFFSET_DESC = OFFSET_NAME + 24;               // 24+192 =216
const size_t OFFSET_SERIAL = OFFSET_DESC + 192;            // 216+2=218
const size_t OFFSET_ID = OFFSET_SERIAL + 2;                // 218+1=219
const size_t OFFSET_CURRENTMODE = OFFSET_ID + 1;           // 219+1=220
const size_t SIZE_MODE = 24 + 192 + 2; // 218 bytes por modo
const size_t SIZE_MODES = SIZE_MODE * 16; // 3488 bytes
const size_t OFFSET_MODES = OFFSET_CURRENTMODE + 1;  // 220 +0=220
// Icono empieza en 220 + 3488 = 3708
const size_t OFFSET_ICONO = OFFSET_MODES + SIZE_MODES; // 220 +3488=3708
// icono =64*64*2=8192 bytes total ~11900

static uint16_t lineBuffer[64]; 
extern INFO_PACK_T ambientesOption;
extern INFO_PACK_T fichasOption;

bool readElementData(fs::File& f, char* elementName, char* modeName, int& startX, int& startY);
bool writeBytesChecked(fs::File &f, const uint8_t* data, size_t length);
void formatSPIFFS();
String generateUniqueFileName(const char* baseName);
bool saveElementFieldByField(const char* baseName, const uint16_t* iconData);
void printElementInfo(const String &fileName);
bool readElementData(fs::File& f, char* elementName, char* modeName, int& startX, int& startY);
void loadElementsFromSPIFFS();
void initializeDynamicOptions();
void loadElementsFromSPIFFS();
byte getCurrentElementID();
bool isCurrentElementSelected();

