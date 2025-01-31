#pragma once

#include <FS.h>
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <vector>
#include <string>

// Declaración adelantada (forward) para no generar bucle de includes.
class DynamicLEDManager;
void display_init();
void drawNoElementsMessage();
void drawErrorMessage(const char* message);
void drawElementIcon(fs::File& f, int startX, int startY);
void drawHiddenMenu(int selection);
void scrollTextTickerBounce(int selection);

extern bool isScrollingText;
extern TFT_eSPI tft;
extern TFT_eSprite uiSprite;
