#include <display_handler/display_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include "rom/rtc.h"    // Opcional, si quieres info de reset

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


// void drawHiddenMenu(int selection) {
//     // Opciones del menú oculto
//     const char* menuOptions[] = {
//         "Buscar elemento",
//         "Idioma",
//         "Sonido",
//         "Brillo",
//         "Respuestas muy muy largas",
//         "Volver al menú principal"
//     };
//     const int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);

//     // Número de opciones visibles a la vez (simulamos scroll vertical con startIndex)
//     const int visibleOptions = 4;

//     // Medidas de la “tarjeta” y la barra de scroll vertical
//     const int cardWidth       = 110; 
//     const int cardHeight      = CARD_HEIGHT;     // Asegúrate de tenerlo definido
//     const int cardMargin      = CARD_MARGIN;     // Asegúrate de tenerlo definido
//     const int scrollBarWidth  = 5;
//     const int scrollBarMargin = 3;

//     // Parámetros del scroll horizontal (vaivén)
//     const int scrollSpeed    = 1;      // Velocidad de desplazamiento (px por “tick”)
//     const int scrollUpdateMs = 40;     // Intervalo de refresco (ms) para la animación
//     static unsigned long lastScrollTime = 0;

//     // Variables estáticas para conservar estado entre llamadas
//     // - scrollOffsets[]: posición actual (en píxeles) del desplazamiento
//     // - scrollDirections[]: dirección de desplazamiento (+1 o -1)
//     static int scrollOffsets[6]    = {0};
//     static int scrollDirections[6] = {1,1,1,1,1,1};

//     // Cálculo del índice inicial (startIndex) para “scroll” vertical
//     int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));

//     // Limpiar sprite principal con color de fondo
//     uiSprite.fillSprite(BACKGROUND_COLOR);

//     // Dibujar título (centrado arriba)
//     uiSprite.setTextColor(TEXT_COLOR);
//     uiSprite.setTextDatum(TC_DATUM);
//     uiSprite.setTextSize(2);
//     uiSprite.drawString("MENU OCULTO", 64, 5);

//     // === DIBUJAR LAS OPCIONES VISIBLES ===
//     for (int i = 0; i < visibleOptions && (startIndex + i) < numOptions; i++) {
//         int currentIndex = startIndex + i;
//         int y = 30 + i * (cardHeight + cardMargin);

//         // Dibuja el fondo de la tarjeta
//         uiSprite.fillRoundRect(9, y, cardWidth, cardHeight, 5, CARD_COLOR);

//         // Borde resaltado si es la opción seleccionada
//         bool isSelected = (currentIndex == selection);
//         uint32_t borderColor = isSelected ? HIGHLIGHT_COLOR : TEXT_COLOR;
//         uiSprite.drawRoundRect(9, y, cardWidth, cardHeight, 5, borderColor);

//         // Zona disponible para el texto dentro de la tarjeta
//         int textAreaX = 9 + 2;                // margen izquierdo dentro de la tarjeta
//         int textAreaY = y + 2;                // margen superior
//         int textAreaW = cardWidth - 4;        // ancho interno para el texto
//         int textAreaH = cardHeight - 4;       // alto interno

//         // Cambiar ajuste del texto para medir correctamente
//         uiSprite.setTextColor(borderColor);
//         uiSprite.setTextSize(1);
//         uiSprite.setTextDatum(TL_DATUM);  // top-left para medir el ancho en pixeles

//         // Medimos el ancho del texto completo con la misma fuente, tamaño, etc.
//         int fullTextWidth = uiSprite.textWidth(menuOptions[currentIndex]);

//         // Si NO está seleccionada o el texto cabe en el ancho => dibujar estático
//         if (!isSelected || fullTextWidth <= textAreaW) {
//             // Para evitar que se desborde, si es más largo y no está seleccionado:
//             // recortamos con "..."
//             String tempStr = menuOptions[currentIndex];
//             if (!isSelected && fullTextWidth > textAreaW) {
//                 // Reemplazar el final con "..." hasta que quepa
//                 while (uiSprite.textWidth(tempStr + "...") > textAreaW && tempStr.length() > 0) {
//                     tempStr.remove(tempStr.length() - 1);
//                 }
//                 tempStr += "...";
//             }

//             // Centrado vertical y algo aproximado horizontal (o simplemente left-justified)
//             int yCenter = textAreaY + (textAreaH - 8) / 2;  
//             // 8 px es aprox la altura de la fuente a textSize=1 (Fuente 1)

//             uiSprite.drawString(tempStr.c_str(), textAreaX, yCenter);

//             // Reset scroll si NO está seleccionada
//             if (!isSelected) {
//                 scrollOffsets[currentIndex]    = 0;
//                 scrollDirections[currentIndex] = 1;
//             }
//         }
//         else {
//             // === Está seleccionada Y el texto excede el ancho => SCROLL HORIZONTAL (vaivén) ===

//             // Calculamos el máximo desplazamiento
//             // Si el texto mide 150 px y el área es 100 px => maxScroll = 50
//             int maxScroll = fullTextWidth - textAreaW;

//             // Actualizamos desplazamiento cada X ms
//             unsigned long now = millis();
//             if (now - lastScrollTime >= scrollUpdateMs) {
//                 scrollOffsets[currentIndex] += scrollDirections[currentIndex] * scrollSpeed;
//                 // Rebotar a izquierda
//                 if (scrollOffsets[currentIndex] < 0) {
//                     scrollOffsets[currentIndex] = 0;
//                     scrollDirections[currentIndex] = 1;
//                 }
//                 // Rebotar a derecha
//                 else if (scrollOffsets[currentIndex] > maxScroll) {
//                     scrollOffsets[currentIndex] = maxScroll;
//                     scrollDirections[currentIndex] = -1;
//                 }
//                 lastScrollTime = now;
//             }

//             // Creamos un sprite auxiliar del tamaño de textAreaW x textAreaH
//             TFT_eSprite textSprite(&tft);
//             if (textSprite.createSprite(textAreaW, textAreaH) == nullptr) {
//                 // Si no hay memoria, dibuja estático para evitar reset
//                 uiSprite.drawString(menuOptions[currentIndex], textAreaX, textAreaY);
//                 continue; 
//             }

//             // Fondo igual al color de la tarjeta
//             textSprite.fillSprite(CARD_COLOR);

//             // Ajustamos el sprite para dibujar el texto
//             textSprite.setTextColor(borderColor);
//             textSprite.setTextSize(1);
//             textSprite.setTextDatum(TL_DATUM);  // Arriba-izq

//             // Calcular posición X dentro del sprite para “mover” el texto
//             int xPos = -scrollOffsets[currentIndex];
//             // Centrado vertical aproximado
//             int yPos = (textAreaH - 8) / 2;  // 8 px = altura aproximada de la fuente

//             // Dibujamos la cadena en el sprite
//             textSprite.drawString(menuOptions[currentIndex], xPos, yPos);

//             // Copiamos el sprite al uiSprite en la posición textAreaX, textAreaY
//             textSprite.pushSprite(textAreaX, textAreaY);

//             // Liberamos memoria
//             textSprite.deleteSprite();
//         }
//     }

//     // === BARRA DE DESPLAZAMIENTO VERTICAL ===
//     const int scrollBarHeight = visibleOptions * (cardHeight + cardMargin) - cardMargin;
//     const int scrollBarY = 30;
//     const int scrollBarX = 128 - scrollBarWidth - scrollBarMargin;

//     // Fondo de la barra
//     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

//     // “Pulgar” de la barra
//     if (numOptions > visibleOptions) {
//         float thumbRatio = (float)visibleOptions / numOptions;
//         int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
//         int thumbY = scrollBarY + (scrollBarHeight - thumbHeight)
//                      * ((float)startIndex / (numOptions - visibleOptions));
//         uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
//     }
//     else {
//         // Si hay menos o igual opciones que el área visible, no hace falta “pulgar”
//         uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
//     }

//     // Empujar todo a pantalla
//     uiSprite.pushSprite(0, 0);
// }

    // Opciones del menú oculto
    const char* menuOptions[] = {
        " Buscar elemento",
        " Idioma",
        " Sonido",
        " Brillo",
        " Respuestas muy muy largas",
        " Volver al menu principal"
    };
    const int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);

    // Número máximo de opciones visibles
    const int visibleOptions = 4;

void drawHiddenMenu(int selection)
{
    // Dimensiones de las tarjetas
    const int cardWidth  = 110;
    const int cardHeight = CARD_HEIGHT;  // ajusta con tu valor
    const int cardMargin = CARD_MARGIN;  // ajusta con tu valor

    // Limpiar sprite principal con color de fondo
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Dibujar título
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(2);
    uiSprite.drawString("AJUSTES", 64, 5);

    // Cálculo de startIndex para scroll vertical
    int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));

    // Dibujar las opciones visibles
    for (int i = 0; i < visibleOptions && (startIndex + i) < numOptions; i++)
    {
        int currentIndex = startIndex + i;
        int y = 30 + i * (cardHeight + cardMargin);

        // Fondo de la tarjeta
        uiSprite.fillRoundRect(9, y, cardWidth, cardHeight, 5, CARD_COLOR);

        // Color del borde (resaltar si es la seleccionada)
        bool isSelected = (currentIndex == selection);
        uint32_t borderColor = isSelected ? HIGHLIGHT_COLOR : TEXT_COLOR;
        uiSprite.drawRoundRect(9, y, cardWidth, cardHeight, 5, borderColor);

        // Área interna de texto
        int textAreaX = 9 + 2;
        int textAreaY = y + 2;
        int textAreaW = cardWidth  - 4;
        int textAreaH = cardHeight - 4;

        // Ajustes de texto
        uiSprite.setTextColor(borderColor);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);

        // Medir el ancho total
        int fullTextWidth = uiSprite.textWidth(menuOptions[currentIndex]);
        // Copiamos el texto a un String para recortarlo si excede
        String tempStr = menuOptions[currentIndex];

        // Si sobrepasa el ancho, lo truncamos sin añadir "..."
        if (fullTextWidth > textAreaW) {
            while (uiSprite.textWidth(tempStr) > textAreaW && tempStr.length() > 0) {
                tempStr.remove(tempStr.length() - 1);
            }
        }

        // Centrado vertical aproximado
        int yCenter = textAreaY + (textAreaH - 8) / 2;
        uiSprite.drawString(tempStr, textAreaX, yCenter);
    }

    // Barra de scroll vertical
    const int scrollBarWidth  = 5;
    const int scrollBarMargin = 3;
    const int scrollBarHeight = visibleOptions * (cardHeight + cardMargin) - cardMargin;
    const int scrollBarY      = 30;
    const int scrollBarX      = 128 - scrollBarWidth - scrollBarMargin;

    uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    // “Pulgar” en caso de haber más opciones que las visibles
    if (numOptions > visibleOptions) {
        float thumbRatio = (float)visibleOptions / (float)numOptions;
        int thumbHeight  = max(20, (int)(scrollBarHeight * thumbRatio));
        int thumbY       = scrollBarY + (scrollBarHeight - thumbHeight)
                           * (float)(selection - startIndex) / (float)(numOptions - visibleOptions);
        uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    }
    else {
        // Si no hay scroll vertical
        uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
    }

    // Enviar todo a la pantalla
    uiSprite.pushSprite(0, 0);
}

void scrollTextTickerBounce(int selection)
{
    // ==========================
    // 1. CONFIGURACIÓN
    // ==========================
    const int cardWidth   = 110;
    const int cardHeight  = CARD_HEIGHT;
    const int cardMargin  = CARD_MARGIN;
    const int visibleOptions = 4;

    // Intervalo de “salto” (ms) para moverse 1 carácter
    const unsigned long frameInterval = 400;

    // Tamaño máximo del substring (nº de caracteres visibles a la vez)
    const int chunkSize = 17;

    // Offsets y dirección de scroll (+1 o -1)
    static int charOffsets[6]         = {0,0,0,0,0,0}; 
    static int scrollDirections[6]    = {1,1,1,1,1,1};

    // Control de tiempo de la última actualización
    static unsigned long lastFrameTime = 0;

    // ==========================
    // 2. FUENTE MONO Y WRAP OFF
    // ==========================
    tft.setFreeFont(nullptr);
    tft.setTextFont(1);
    tft.setTextWrap(false);
    tft.setTextSize(1);

    // ==========================
    // 3. CADENA Y MEDIR LONGITUD
    // ==========================
    if (selection < 0 || selection >= numOptions) return;
    String text = menuOptions[selection];

    int fullTextWidth = tft.textWidth(text.c_str());
    int textVisibleW  = cardWidth - 4; 
    if (fullTextWidth <= textVisibleW) {
        // Cabe sin scroll => no hacemos nada
        return;
    }

    // ==========================
    // 4. CÁLCULO DE POSICIÓN
    // ==========================
    int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));
    int cardIndex  = selection - startIndex;
    if (cardIndex < 0 || cardIndex >= visibleOptions) return;

    int cardY      = 30 + cardIndex * (cardHeight + cardMargin);
    int textAreaX  = 9 + 2;  // = 11
    int textAreaY  = cardY + 2;
    int textAreaW  = textVisibleW;      // cardWidth - 4
    int textAreaH  = cardHeight - 4;

    // ==========================
    // 5. CONTROL DE TIEMPO Y “VAIVÉN”
    // ==========================
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
        // Avanzamos / retrocedemos 1 carácter
        charOffsets[selection] += scrollDirections[selection];

        // Límite izquierdo (0)
        if (charOffsets[selection] < 0) {
            charOffsets[selection] = 0;
            scrollDirections[selection] = +1; // rebote
        }

        // Límite derecho (text.length() - chunkSize)
        int maxOffset = text.length() - chunkSize;
        if (maxOffset < 0) maxOffset = 0; // chunkSize mayor o igual a la longitud

        if (charOffsets[selection] > maxOffset) {
            charOffsets[selection] = maxOffset;
            scrollDirections[selection] = -1; // rebote
        }

        lastFrameTime = now;
    }

    // ==========================
    // 6. CREAR SPRITE PEQUEÑO
    // ==========================
    TFT_eSprite tickerSprite(&tft);
    if (tickerSprite.createSprite(textAreaW, textAreaH) == nullptr) {
        // Sin memoria => no scroll
        return;
    }

    // ==========================
    // 7. AJUSTES EN SPRITE
    // ==========================
    tickerSprite.setFreeFont(nullptr);
    tickerSprite.setTextFont(1);
    tickerSprite.setTextWrap(false);
    tickerSprite.setTextSize(1);
    tickerSprite.setTextColor(HIGHLIGHT_COLOR);
    tickerSprite.setTextDatum(TL_DATUM);

    // Fondo con tu color de tarjeta
    tickerSprite.fillSprite(CARD_COLOR);

    // ==========================
    // 8. POSICIÓN VERTICAL (centrado)
    // ==========================
    int yCenter = (textAreaH - 8) / 2;
    if (yCenter < 0) yCenter = 0;
    if (yCenter >= textAreaH) yCenter = textAreaH - 1;

    // ==========================
    // 9. SUBSTRING SIN “...”
    // ==========================
    int offset = charOffsets[selection];
    if (offset < 0) offset = 0;
    if (offset >= (int)text.length()) offset = text.length() - 1;

    int endIndex = offset + chunkSize;
    if (endIndex > (int)text.length()) {
        endIndex = text.length();
    }

    String sliceStr = text.substring(offset, endIndex);

    // Si está vacío, mejor un string vacío que “...”
    if (sliceStr.isEmpty()) {
        sliceStr = "";
    }

    // ==========================
    // 10. DIBUJAR SUBSTRING
    // ==========================
    tickerSprite.drawString(sliceStr, 0, yCenter);

    // ==========================
    // 11. Mostrar en pantalla
    // ==========================
    tickerSprite.pushSprite(textAreaX, textAreaY);
    tickerSprite.deleteSprite();
}