#pragma once

#include <FS.h>
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <vector>
#include <string>

void display_init();
void drawNoElementsMessage();
void drawErrorMessage(const char* message);
void drawElementIcon(fs::File& f, int startX, int startY);

extern TFT_eSPI tft;
extern TFT_eSprite uiSprite;
