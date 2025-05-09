#pragma once

#include <FS.h>
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <vector>
#include <map>
#include <string>

// Declaración adelantada (forward) para no generar bucle de includes.
class DynamicLEDManager;
void display_init();
void drawNoElementsMessage();
void drawErrorMessage(const char* message);
void drawElementIcon(fs::File& f, int startX, int startY);
void drawHiddenMenu(int selection);
void scrollTextTickerBounce(int selection);
void showMessageWithLoading(const char* message, unsigned long delayTime);
void updateNameScroll();
void updateModeScroll();
String getModeDisplayName(const String &fullModeName, bool alternateActive);
void display_sleep();
void display_wakeup();
void drawLanguageMenu(int selection);
void drawBankSelectionMenu(const std::vector<byte>& bankList, const std::vector<bool>& selectedBanks, int currentSelection, int windowOffset);
void drawBrightnessMenu(uint8_t brightness);
void drawSunIcon(int16_t x, int16_t y, uint16_t color);
void drawSoundMenu(int selection);
void scrollTextTickerBounceSound(int selection);
void drawFormatMenu(int selection);
void drawDeleteElementMenu(int selection);
void drawConfirmDelete(const String& fileName);
void scrollTextTickerBounceFormat(int selection);
void scrollTextTickerBounceDelete(int selection);
void showCriticalBatteryMessage();
void drawBatteryIconMini(float percentage);
void drawCognitiveMenu();

extern bool isScrollingText;
extern TFT_eSPI tft;
extern TFT_eSprite uiSprite;
extern bool nameScrollActive;
extern bool modeScrollActive;
extern bool brightnessMenuActive;
extern std::map<String, std::vector<bool>> elementAlternateStates;
extern std::vector<bool> currentAlternateStates;
extern uint8_t currentBrightness;       // Valor actual en porcentaje
extern uint8_t tempBrightness; 
extern bool forceDrawDeleteElementMenu;
extern int bankMenuCurrentSelection;   // 0: Confirmar, 1..n: banks
extern int bankMenuWindowOffset;
extern bool criticalBatteryLock; // lo usarás en otras partes
extern float batteryPercentage;

