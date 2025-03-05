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

extern bool isScrollingText;
extern TFT_eSPI tft;
extern TFT_eSprite uiSprite;
extern bool nameScrollActive;
extern bool modeScrollActive;
extern std::map<String, std::vector<bool>> elementAlternateStates;
extern std::vector<bool> currentAlternateStates;
