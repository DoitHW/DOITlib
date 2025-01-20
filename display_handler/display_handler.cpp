#include <display_handler/display_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>


// Definir dimensiones
#define CARD_WIDTH 110
#define CARD_HEIGHT 20
#define CARD_MARGIN 5
#define SCROLL_BAR_WIDTH 5

#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define HIGHLIGHT_COLOR TFT_GREEN
#define CARD_COLOR TFT_NAVY

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite uiSprite = TFT_eSprite(&tft);

void display_init() {
    //Serial.println("Inicializando display...");
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setSwapBytes(true);
    uiSprite.createSprite(tft.width(), tft.height());
    uiSprite.setSwapBytes(true);
    //Serial.println("Display inicializado.");
}

void drawNoElementsMessage() {
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.setTextDatum(MC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.setTextWrap(true, true);
    uiSprite.drawString("No hay elementos en la sala.", tft.width() / 2, tft.height() / 2);
    uiSprite.pushSprite(0, 0);
}


void drawErrorMessage(const char* message) {
    Serial.println(message);
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.setTextDatum(MC_DATUM);
    uiSprite.setTextSize(2);
    uiSprite.drawString(message, tft.width() / 2, tft.height() / 2);
    uiSprite.pushSprite(0, 0);
}

void drawElementIcon(fs::File& f, int startX, int startY) {
    f.seek(OFFSET_ICONO, SeekSet);
    for (int y = 0; y < 64; y++) {
        int br = f.read((uint8_t*)lineBuffer, 64 * 2);
        if (br < (64 * 2)) break;
        uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
    }
}

void drawElementName(const char* elementName, bool isSelected) {
    uiSprite.setTextColor(isSelected ? TFT_GREEN : TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(2);
    uiSprite.drawString(elementName, tft.width() / 2, tft.height() - 40);
}

void drawModeName(const char* modeName) {
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(modeName, tft.width() / 2, tft.height() - 15);
}

void drawSelectionCircle(bool isSelected, int startX, int startY) {
    if (isSelected) {
        uiSprite.fillCircle(startX + 64 +5, startY + 5, 5, TFT_GREEN);
    }
}

void drawNavigationArrows() {
    int arrowSize = 20;
    int arrowY = tft.height() / 2;
    
    // Flecha izquierda
    uiSprite.fillTriangle(5, arrowY, 15, arrowY - 10, 15, arrowY + 10, TFT_WHITE);
    
    // Flecha derecha
    uiSprite.fillTriangle(tft.width() - 5, arrowY, tft.width() - 15, arrowY - 10, tft.width() - 15, arrowY + 10, TFT_WHITE);
}

void drawCurrentElement() {

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Mostrar mensaje si no hay elementos
    if (elementFiles.empty()) {
        drawNoElementsMessage();
        return;
    }

    // Corregir límites del índice actual
    currentIndex = constrain(currentIndex, 0, (int)elementFiles.size() - 1);
    String currentFile = elementFiles[currentIndex];

    // Variable para almacenar el modo actual
    byte currentMode = 0;

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        // Opciones dinámicas
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;

        // Leer y usar el modo actual desde la opción dinámica
        currentMode = option->currentMode;

        int startX = (tft.width() - 64) / 2;
        int startY = (tft.height() - 64) / 2 - 20;

        // Dibujar ícono
        for (int y = 0; y < 64; y++) {
            memcpy(lineBuffer, option->icono[y], 64 * 2);
            uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
        }

        // Dibujar nombre y modo
        Serial.println("Nombre elemento: " + String((char*)option->name));
        drawElementName((char*)option->name, selectedStates[currentIndex]);
        drawModeName((char*)option->mode[currentMode].name);

        // Dibujar círculo de selección si está seleccionado
        drawSelectionCircle(selectedStates[currentIndex], startX, startY);
    } else {
        // Elementos cargados desde SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r");
        if (!f) {
            drawErrorMessage("Error leyendo elemento");
            return;
        }

        // Leer datos del archivo
        char elementName[25] = {0};
        char modeName[25] = {0};
        int startX, startY;

        if (!readElementData(f, elementName, modeName, startX, startY)) {
            drawErrorMessage("Datos incompletos");
            return;
        }

        Serial.println("⚡⚡⚡⚡ currentFile - drawCurrentElement: " + String(elementName));

        // Dibujar ícono del elemento
        drawElementIcon(f, startX, startY);

        // Dibujar nombre y modo
        Serial.println("Nombre elemento: " + String(elementName));
        drawElementName(elementName, selectedStates[currentIndex]);
        drawModeName(modeName);

        // Leer el modo actual desde el archivo SPIFFS
        f.seek(OFFSET_CURRENTMODE, SeekSet);
        f.read(&currentMode, 1);
        f.close();

        // Dibujar círculo de selección si está seleccionado
        drawSelectionCircle(selectedStates[currentIndex], startX, startY);
    }

    // Actualizar `currentModeIndex` y reflejar el patrón en los LEDs
    currentModeIndex = currentMode;
    //colorHandler.setPatternBotonera(currentModeIndex);
    // Llamar a setPatternBotonera con el gestor de efectos
    colorHandler.setPatternBotonera(currentModeIndex, ledManager);


    // Mostrar el modo actual en el monitor serial
    Serial.println("Modo actual!: " + String(currentModeIndex));
    drawNavigationArrows();
    uiSprite.pushSprite(0, 0);
}


void animateTransition(int direction) {
    if (elementFiles.size() <= 1) return;
    int nextIndex = currentIndex + direction;
    if (nextIndex < 0) nextIndex = (int)elementFiles.size() - 1;
    if ((size_t)nextIndex >= elementFiles.size()) nextIndex = 0;

    // Aquí se podría implementar una animación si se desea.
    currentIndex = nextIndex;
    drawCurrentElement();
}


// Función para mostrar la pantalla MODOS
void drawModesScreen() {
    static int scrollOffset = 0;
    static int targetScrollOffset = 0;

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(2);
    uiSprite.drawString("MODOS", 64, 5);

    // Obtener modos del elemento actual
    String currentFile = elementFiles[currentIndex];
    totalModes = 0;

    // Arreglo auxiliar para mapear índices visibles a índices reales
    int visibleModesMap[16] = {0};
    memset(visibleModesMap, -1, sizeof(visibleModesMap));  // Inicializar con -1

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;

        // Cargar modos de las opciones dinámicas
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                visibleModesMap[totalModes] = i;  // Mapear índice visible al índice real
                int y = 30 + totalModes * (CARD_HEIGHT + CARD_MARGIN) - scrollOffset;
                if (y > 20 && y < 110) {
                    uiSprite.fillRoundRect(9, y, CARD_WIDTH, CARD_HEIGHT, 5, CARD_COLOR);
                    uiSprite.drawRoundRect(9, y, CARD_WIDTH, CARD_HEIGHT, 5, (totalModes == currentModeIndex) ? HIGHLIGHT_COLOR : TEXT_COLOR);

                    uiSprite.setTextColor((totalModes == currentModeIndex) ? HIGHLIGHT_COLOR : TEXT_COLOR);
                    uiSprite.setTextDatum(CL_DATUM);
                    uiSprite.setTextSize(1);
                    uiSprite.drawString((char*)option->mode[i].name, 15, y + CARD_HEIGHT / 2);
                }
                totalModes++;
            }
        }
    } else {
        fs::File f = SPIFFS.open(currentFile, "r");
        if (!f) {
            Serial.println("Error al abrir archivo para leer modos.");
            uiSprite.drawString("Error leyendo modos", 10, 25);
            uiSprite.pushSprite(0, 0);
            return;
        }

        //Serial.println("⚡⚡⚡⚡ currentFile - drawModesScreen: " + String(currentFile));

        for (int i = 0; i < 16; i++) {
            char modeName[25] = {0};
            char modeDesc[193] = {0};
            byte modeConfig[2] = {0};

            // Leer datos exactamente como `printElementInfo`
            if (f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet)) {
                f.read((uint8_t*)modeName, 24);
                f.read((uint8_t*)modeDesc, 192);
                f.read(modeConfig, 2);

                if (strlen(modeName) > 0) {
                    //Serial.printf("Modo %d:\n", i);
                    //Serial.printf("  Nombre: %s\n", modeName);
                   // Serial.printf("  Descripción: %s\n", modeDesc);
                    //Serial.printf("  Configuración: 0x%02X%02X\n", modeConfig[0], modeConfig[1]);

                    // Verificar el bit más significativo
                    if (checkMostSignificantBit(modeConfig)) {
                        visibleModesMap[totalModes] = i;  // Mapear índice visible al índice real
                        Serial.printf("El bit más significativo del modo %d es 1\n", i);
                        int y = 30 + totalModes * (CARD_HEIGHT + CARD_MARGIN) - scrollOffset;
                        if (y > 20 && y < 110) {
                            uiSprite.fillRoundRect(9, y, CARD_WIDTH, CARD_HEIGHT, 5, CARD_COLOR);
                            uiSprite.drawRoundRect(9, y, CARD_WIDTH, CARD_HEIGHT, 5, (totalModes == currentModeIndex) ? HIGHLIGHT_COLOR : TEXT_COLOR);

                            uiSprite.setTextColor((totalModes == currentModeIndex) ? HIGHLIGHT_COLOR : TEXT_COLOR);
                            uiSprite.setTextDatum(CL_DATUM);
                            uiSprite.setTextSize(1);
                            uiSprite.drawString(modeName, 15, y + CARD_HEIGHT / 2);
                        }
                        totalModes++;
                    } else {
                        //Serial.printf("El bit más significativo del modo %d es 0\n", i);
                    }
                } else {
                    //Serial.printf("Modo %d tiene nombre vacío o inválido.\n", i);
                }
            } else {
                Serial.printf("Error: No se pudo buscar el offset del modo %d\n", i);
            }
        }

        f.close();
    }

    if (totalModes == 0) {
        uiSprite.drawString("No hay modos disponibles", 10, 25);
        Serial.println("⚡ Advertencia: No hay modos disponibles para mostrar.");
        return;
    }

    // Dibujar barra de desplazamiento
    int cardHeightWithMargin = CARD_HEIGHT + CARD_MARGIN;
    int scrollBarHeight = 100 * (100.0 / (totalModes * cardHeightWithMargin));
    int scrollBarY = 25 + (100 - scrollBarHeight) * (scrollOffset / (float)(totalModes * cardHeightWithMargin - 100));

    uiSprite.fillRoundRect(122, 25, SCROLL_BAR_WIDTH, 100, 2, TFT_DARKGREY);
    uiSprite.fillRoundRect(122, scrollBarY, SCROLL_BAR_WIDTH, scrollBarHeight, 2, TEXT_COLOR);

    uiSprite.pushSprite(0, 0);

    // Actualizar desplazamiento objetivo
    targetScrollOffset = currentModeIndex * (CARD_HEIGHT + CARD_MARGIN);
    if (targetScrollOffset > totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100) {
        targetScrollOffset = totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100;
    }
    if (targetScrollOffset < 0) targetScrollOffset = 0;

    // Aplicar desplazamiento suave
    scrollOffset += (targetScrollOffset - scrollOffset) / 4;

    // Actualizar el mapa global
    memcpy(globalVisibleModesMap, visibleModesMap, sizeof(visibleModesMap));
}

void drawHiddenMenu(int selection) {
    uiSprite.fillSprite(BACKGROUND_COLOR);
    uiSprite.setTextColor(selection == 0 ? HIGHLIGHT_COLOR : TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString("1. Idioma", tft.width() / 2, 40);
    uiSprite.setTextColor(selection == 1 ? HIGHLIGHT_COLOR : TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString("2. Volver", tft.width() / 2, 70);
    uiSprite.pushSprite(0, 0);
}















