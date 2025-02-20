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

void showMessageWithLoading(const char* message, unsigned long delayTime) {
    ignoreInputs = true;
    unsigned long startTime = millis();
    int frameCount = 0;
    const int maxWidth = tft.width() - 20; // Margen de 10 píxeles a cada lado
    const int lineHeight = 20; // Altura aproximada de una línea de texto
  
    while (millis() - startTime < delayTime) {
      uiSprite.fillSprite(BACKGROUND_COLOR);
      
      // Dibuja el mensaje con wrap
      uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
      uiSprite.setTextDatum(TL_DATUM); // Alineación arriba-izquierda
      uiSprite.setFreeFont(&FreeSansBold9pt7b);
      uiSprite.setTextSize(1);
  
      int cursorY = 10; // Empezamos 10 píxeles desde arriba
      String currentLine = "";
      String remainingText = String(message);
  
      while (remainingText.length() > 0) {
        int spaceIndex = remainingText.indexOf(' ');
        String word = (spaceIndex == -1) ? remainingText : remainingText.substring(0, spaceIndex);
        
        if (uiSprite.textWidth(currentLine + word) <= maxWidth) {
          currentLine += word + " ";
          remainingText = (spaceIndex == -1) ? "" : remainingText.substring(spaceIndex + 1);
        } else {
          uiSprite.drawString(currentLine, 10, cursorY);
          cursorY += lineHeight;
          currentLine = word + " ";
          remainingText = (spaceIndex == -1) ? "" : remainingText.substring(spaceIndex + 1);
        }
  
        if (spaceIndex == -1) {
          uiSprite.drawString(currentLine, 10, cursorY);
          break;
        }
      }
  
      // Animación de puntos girando
      int centerX = tft.width() / 2;
      int centerY = tft.height() - 30;
      int radius = 10;
      for (int i = 0; i < 8; i++) {
        float angle = frameCount * 0.2 + i * PI / 4;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        uiSprite.fillCircle(x, y, 2, TEXT_COLOR); // Puntos más grandes
      }
  
      uiSprite.pushSprite(0, 0);
  
      frameCount++;
      delay(33); // Aproximadamente 30 FPS
    }
    ignoreInputs = false;
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
    uiSprite.setFreeFont(&FreeSansBold9pt7b);  // Selecciona la fuente FreeSans12pt7b
    uiSprite.setTextColor(isSelected ? TFT_GREEN : TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);                // Aplica el tamaño 2
    uiSprite.drawString(elementName, tft.width() / 2, tft.height() - 40);
}

void drawModeName(const char* modeName) {
    uiSprite.setFreeFont(&FreeSans9pt7b);
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

// Parámetros para el área del nombre (ajusta según necesites)
const int DISPLAY_WIDTH = 128;            // Ancho total del display
const int NAME_Y = tft.height() - 40;       // Posición vertical del área del nombre
const int NAME_AREA_HEIGHT = 20;          // Altura del área donde se dibuja el nombre (más reducido)
const int NAME_SCROLL_INTERVAL = 30;      // Intervalo de actualización en ms

// Variables globales para el scroll del nombre
bool nameScrollActive = false;            // Se activa si el nombre es demasiado largo
String currentDisplayName = "";           // Nombre del elemento (ya procesado)
int nameScrollOffset = 0;                 // Desplazamiento actual (en píxeles)
int nameScrollDirection = 1;              // 1: incrementa; -1: decrementa
unsigned long lastNameScrollUpdate = 0;   // Última actualización (ms)

// Función que actualiza (redibuja) la zona del nombre según el desplazamiento actual.
// Se configura la fuente y se limpia toda la línea horizontal de x = 0 a x = DISPLAY_WIDTH.
void updateNameDisplay() {
    // Establece la fuente y el tamaño antes de calcular el ancho
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextSize(1);
    int textWidth = uiSprite.textWidth(currentDisplayName);
    // Usamos alineación izquierda: cuando nameScrollOffset es 0, el extremo izquierdo se dibuja en x=0.
    int drawX = 0 - nameScrollOffset;
    int drawY = NAME_Y;
    
    // Limpiar toda la línea horizontal donde se muestra el nombre (de x = 0 a DISPLAY_WIDTH)
    uiSprite.fillRect(0, drawY, DISPLAY_WIDTH, NAME_AREA_HEIGHT, BACKGROUND_COLOR);
    
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextSize(1);
    uiSprite.setTextDatum(TL_DATUM);  // Alinea a la izquierda
    uiSprite.setTextColor(selectedStates[currentIndex] ? TFT_GREEN : TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString(currentDisplayName, drawX, drawY);
    
    // Actualizar la región completa a la TFT.
    uiSprite.pushSprite(0, drawY, 0, drawY, DISPLAY_WIDTH, NAME_AREA_HEIGHT);
}

// Función que actualiza el desplazamiento (vaivén) del nombre
void updateNameScroll() {
    // Asegurarse de usar la fuente y tamaño correctos
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextSize(1);
    int textWidth = uiSprite.textWidth(currentDisplayName);
    
    // Si el texto cabe, desactivar scroll.
    // Se añade un margen de 10 píxeles para compensar la diferencia.
    if (textWidth <= DISPLAY_WIDTH) {
         nameScrollActive = false;
         nameScrollOffset = 0;
         return;
    }
    
    int maxScroll = textWidth - (DISPLAY_WIDTH - 10);  // Ajustamos el rango con el margen
    if (millis() - lastNameScrollUpdate >= NAME_SCROLL_INTERVAL) {
         nameScrollOffset += nameScrollDirection;
         if (nameScrollOffset < 0) {
              nameScrollOffset = 0;
              nameScrollDirection = 1;
         } else if (nameScrollOffset > maxScroll) {
              nameScrollOffset = maxScroll;
              nameScrollDirection = -1;
         }
         lastNameScrollUpdate = millis();
         updateNameDisplay();
    }
}

// Parámetros para el área del nombre del modo
const int MODE_DISPLAY_WIDTH = 128;        // Ancho total del display (o zona asignada)
const int MODE_Y = tft.height() - 15;        // Posición vertical donde se dibuja el nombre del modo (igual que drawModeName)
const int MODE_AREA_HEIGHT = 20;             // Altura de la zona a actualizar (ajusta para que no se solape)
const int MODE_SCROLL_INTERVAL = 30;         // Intervalo de actualización (ms)

// Variables globales para el scroll del nombre del modo
bool modeScrollActive = false;             // Se activa si el texto del modo es demasiado largo
String currentModeDisplayName = "";        // Cadena que contiene el nombre del modo (ya procesado)
int modeScrollOffset = 0;                  // Desplazamiento actual en píxeles para el modo
int modeScrollDirection = 1;               // Dirección: 1 para incrementar, -1 para decrementar
unsigned long lastModeScrollUpdate = 0;    // Última actualización en ms

// Actualiza la zona del nombre del modo usando el desplazamiento actual
void updateModeDisplay() {
    // Establecer la fuente y tamaño para obtener el ancho correcto
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextSize(1);
    int textWidth = uiSprite.textWidth(currentModeDisplayName);
    // Usamos alineación izquierda para que cuando el desplazamiento sea 0, se vea el principio (la "T" o lo que corresponda)
    int drawX = 0 - modeScrollOffset;
    int drawY = MODE_Y;
    
    // Borrar la zona completa donde se dibuja el nombre del modo (de 0 a MODE_DISPLAY_WIDTH)
    uiSprite.fillRect(0, drawY, MODE_DISPLAY_WIDTH, MODE_AREA_HEIGHT, BACKGROUND_COLOR);
    
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextSize(1);
    uiSprite.setTextDatum(TL_DATUM); // Alineación izquierda
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString(currentModeDisplayName, drawX, drawY);
    
    // Actualizar la región completa en la TFT
    uiSprite.pushSprite(0, drawY, 0, drawY, MODE_DISPLAY_WIDTH, MODE_AREA_HEIGHT);
}

// Actualiza el desplazamiento del nombre del modo y llama a updateModeDisplay()
void updateModeScroll() {
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextSize(1);
    int textWidth = uiSprite.textWidth(currentModeDisplayName);
    
    // Si el texto cabe en el área, desactivar scroll
    if (textWidth <= MODE_DISPLAY_WIDTH) {
         modeScrollActive = false;
         modeScrollOffset = 0;
         return;
    }

    int extraMargin = 5;
    
    int maxScroll = textWidth - (MODE_DISPLAY_WIDTH - extraMargin);  // Rango máximo de desplazamiento
    if (millis() - lastModeScrollUpdate >= MODE_SCROLL_INTERVAL) {
         modeScrollOffset += modeScrollDirection;
         if (modeScrollOffset < 0) {
              modeScrollOffset = 0;
              modeScrollDirection = 1;
         } else if (modeScrollOffset > maxScroll) {
              modeScrollOffset = maxScroll;
              modeScrollDirection = -1;
         }
         lastModeScrollUpdate = millis();
         updateModeDisplay();
    }
}

// Modificación de drawCurrentElement para configurar el scroll del nombre cuando corresponda
void drawCurrentElement() {
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Mostrar mensaje si no hay elementos
    if (elementFiles.empty()) {
        drawNoElementsMessage();
        return;
    }
    currentIndex = constrain(currentIndex, 0, (int)elementFiles.size() - 1);
    String currentFile = elementFiles[currentIndex];
    byte currentMode = 0;

    // Función auxiliar para obtener un nombre "amigable"
    auto getDisplayName = [](const String &fileName) -> String {
         String baseName = fileName.substring(fileName.lastIndexOf("/") + 1, fileName.lastIndexOf(".bin"));
         baseName.replace("element_", "");
         int underscoreIndex = baseName.lastIndexOf("_");
         String name;
         if (underscoreIndex != -1) {
             String possibleNumber = baseName.substring(underscoreIndex + 1);
             if (possibleNumber.toInt() > 0)
                 name = baseName.substring(0, underscoreIndex);
             else
                 name = baseName;
         } else {
             name = baseName;
         }
         int count = 0;
         for (const auto &file : elementFiles) {
             if (file.startsWith("/element_" + name)) {
                 count++;
                 if (file == fileName) break;
             }
         }
         if (count > 1)
             return name + "(" + String(count) + ")";
         else
             return name;
    };

    // Caso 1: "Ambientes" o "Fichas" (elementos fijos)
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
         INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
         currentMode = option->currentMode;
         int startX = (tft.width() - 64) / 2;
         int startY = (tft.height() - 64) / 2 - 20;
         for (int y = 0; y < 64; y++) {
             memcpy(lineBuffer, option->icono[y], 64 * 2);
             uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
         }
         // Dibujo del nombre del elemento
         uiSprite.setFreeFont(&FreeSansBold12pt7b);
         uiSprite.setTextSize(1);
         String displayName = String((char*)option->name);
         int elementTextWidth = uiSprite.textWidth(displayName);
         if (elementTextWidth > DISPLAY_WIDTH) {
              nameScrollActive = true;
              currentDisplayName = displayName;
              nameScrollOffset = 0;
              nameScrollDirection = 1;
              lastNameScrollUpdate = millis();
              updateNameDisplay();
         } else {
              nameScrollActive = false;
              drawElementName(displayName.c_str(), selectedStates[currentIndex]);
         }
         // Dibujo del nombre del modo (scroll si es necesario)
         uiSprite.setFreeFont(&FreeSansBold9pt7b);
         uiSprite.setTextSize(1);
         String modeDisplay = String((char*)option->mode[currentMode].name);
         int modeTextWidth = uiSprite.textWidth(modeDisplay);
         if (modeTextWidth > MODE_DISPLAY_WIDTH) {
              modeScrollActive = true;
              currentModeDisplayName = modeDisplay;
              modeScrollOffset = 0;
              modeScrollDirection = 1;
              lastModeScrollUpdate = millis();
              updateModeDisplay();
         } else {
              modeScrollActive = false;
              drawModeName((char*)option->mode[currentMode].name);
         }
         drawSelectionCircle(selectedStates[currentIndex], startX, startY);
    }
    // Caso 2: "Apagar"
    else if (currentFile == "Apagar") {
         INFO_PACK_T* option = &apagarSala;
         int startX = (tft.width() - 64) / 2;
         int startY = (tft.height() - 64) / 2 - 20;
         for (int y = 0; y < 64; y++) {
             memcpy(lineBuffer, option->icono[y], 64 * 2);
             uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
         }
         drawElementName((char*)option->name, false);
         drawModeName((char*)option->mode[currentMode].name);
         nameScrollActive = false;
         modeScrollActive = false;
    }
    // Caso 3: Elemento almacenado en SPIFFS
    else {
         fs::File f = SPIFFS.open(currentFile, "r");
         if (!f) {
             drawErrorMessage("Error leyendo elemento");
             return;
         }
         char elementName[25] = {0};
         char modeName[25] = {0};
         int startX, startY;
         if (!readElementData(f, elementName, modeName, startX, startY)) {
             drawErrorMessage("Datos incompletos");
             return;
         }
         drawElementIcon(f, startX, startY);
         uiSprite.setFreeFont(&FreeSansBold12pt7b);
         uiSprite.setTextSize(1);
         String displayName = getDisplayName(currentFile);
         int elementTextWidth = uiSprite.textWidth(displayName);
         if (elementTextWidth > DISPLAY_WIDTH) {
              nameScrollActive = true;
              currentDisplayName = displayName;
              nameScrollOffset = 0;
              nameScrollDirection = 1;
              lastNameScrollUpdate = millis();
              updateNameDisplay();
         } else {
              nameScrollActive = false;
              drawElementName(displayName.c_str(), selectedStates[currentIndex]);
         }
         // Para el nombre del modo, usamos la cadena ya leída en modeName
         uiSprite.setFreeFont(&FreeSansBold9pt7b);
         uiSprite.setTextSize(1);
         String modeDisplay = String(modeName);
         int modeTextWidth = uiSprite.textWidth(modeDisplay);
         if (modeTextWidth > MODE_DISPLAY_WIDTH) {
              modeScrollActive = true;
              currentModeDisplayName = modeDisplay;
              modeScrollOffset = 0;
              modeScrollDirection = 1;
              lastModeScrollUpdate = millis();
              updateModeDisplay();
         } else {
              modeScrollActive = false;
              drawModeName(modeName);
         }
         f.seek(OFFSET_CURRENTMODE, SeekSet);
         f.read(&currentMode, 1);
         f.close();
         drawSelectionCircle(selectedStates[currentIndex], startX, startY);
    }
    currentModeIndex = currentMode;
    colorHandler.setCurrentFile(currentFile);
    colorHandler.setPatternBotonera(currentModeIndex, ledManager);
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

void drawModesScreen() {
    // Variables para scroll vertical suave
    static int scrollOffset = 0;
    static int targetScrollOffset = 0;
    const int visibleOptions = 4; // Número máximo de opciones visibles

    // Variables para el scroll horizontal de la opción seleccionada (pixel-based)
    static int modeTickerOffset = 0;
    static int modeTickerDirection = 1;
    static unsigned long modeLastFrameTime = 0;
    static int lastSelectedMode = -1;

    // Limpiar el sprite completo
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título (usando la fuente FreeSans12pt7b)
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString("MODOS", 64, 5);

    // Obtener el archivo del elemento actual
    String currentFile = elementFiles[currentIndex];
    totalModes = 0;

    // Mapa auxiliar: índice visible -> índice real (máximo 16 modos + 1 extra para regresar)
    int visibleModesMap[17] = {0};
    memset(visibleModesMap, -1, sizeof(visibleModesMap));

    int visibleCurrentModeIndex = -1; // Índice visible de la opción seleccionada

    // Determinar de dónde obtener los modos: de opciones fijas (Ambientes/Fichas) o desde SPIFFS
    INFO_PACK_T* option = nullptr;
    fs::File f;
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
    } else if (currentFile != "Apagar") {
        f = SPIFFS.open(currentFile, "r");
        if (!f) {
            Serial.println("Error al abrir archivo para leer modos.");
            uiSprite.drawString("Error leyendo modos", 10, 25);
            uiSprite.pushSprite(0, 0);
            return;
        }
    }

    // Recolectar los modos válidos (máximo 16) y llenar el mapa auxiliar
    for (int i = 0; i < 16; i++) {
        char modeName[25] = {0};
        byte modeConfig[2] = {0};

        if (option) {
            strncpy(modeName, (char*)option->mode[i].name, 24);
            memcpy(modeConfig, option->mode[i].config, 2);
        } else {
            f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
            f.read((uint8_t*)modeName, 24);
            f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
            f.read(modeConfig, 2);
        }
        if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
            visibleModesMap[totalModes] = i;
            if (totalModes == currentModeIndex) {
                visibleCurrentModeIndex = totalModes;
            }
            totalModes++;
        }
    }
    if (!option) {
        f.close();
    }

    // Agregar la opción extra para regresar al menú principal
    visibleModesMap[totalModes] = -2; // Usamos -2 como indicador de la opción "Regresar"
    if (totalModes == currentModeIndex) {
        visibleCurrentModeIndex = totalModes;
    }
    totalModes++;

    if (totalModes == 0) { // Esto no debería ocurrir, ya que siempre habrá al menos la opción de regresar
        uiSprite.drawString("No hay modos disponibles", 10, 25);
        Serial.println("⚡ Advertencia: No hay modos disponibles para mostrar.");
        uiSprite.pushSprite(0, 0);
        return;
    }

    // Calcular el índice de inicio (startIndex) para centrar la opción seleccionada
    int startIndex = max(0, min(currentModeIndex - visibleOptions / 2, totalModes - visibleOptions));

    // Parámetros de layout:
    // La zona de texto se limita entre x = 10 y x = 120 (ancho disponible = 110 píxeles)
    const int x = 10;
    const int textAreaW = 110;
    // La barra vertical se dibuja a partir de x = 120.
    const int scrollBarX = 120;

    // Dibujar cada opción visible
    for (int i = 0; i < visibleOptions && (startIndex + i) < totalModes; i++) {
        int currentVisibleIndex = startIndex + i;
        int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN);
        bool isSelected = (currentVisibleIndex == currentModeIndex);
        uint32_t textColor = isSelected ? HIGHLIGHT_COLOR : TEXT_COLOR;

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextColor(textColor);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);

        // Obtener el valor del modo (índice real o indicador especial)
        int realModeIndex = visibleModesMap[currentVisibleIndex];

        if (realModeIndex == -2) {
            // Dibujar la opción de regresar con un icono de flecha izquierda
            int arrowWidth = 12;
            int arrowHeight = 12;
            int arrowX = x + (textAreaW - arrowWidth) / 2;
            int arrowY = y + (CARD_HEIGHT - arrowHeight) / 2;
            // Dibujar la flecha hacia la izquierda usando un triángulo
            uiSprite.fillTriangle(arrowX + arrowWidth, arrowY, arrowX, arrowY + arrowHeight / 2, arrowX + arrowWidth, arrowY + arrowHeight, textColor);
        } else {
            // Obtener el nombre del modo (índice real)
            char modeName[25] = {0};
            if (option) {
                strncpy(modeName, (char*)option->mode[realModeIndex].name, 24);
            } else {
                f = SPIFFS.open(currentFile, "r");
                f.seek(OFFSET_MODES + realModeIndex * SIZE_MODE, SeekSet);
                f.read((uint8_t*)modeName, 24);
                f.close();
            }
            String modeStr = String(modeName);

            if (isSelected) {
                // Si se cambia la opción seleccionada, reiniciamos el ticker
                if (currentModeIndex != lastSelectedMode) {
                    modeTickerOffset = 0;
                    modeTickerDirection = 1;
                    lastSelectedMode = currentModeIndex;
                }
                uiSprite.setFreeFont(&FreeSans9pt7b);
                uiSprite.setTextSize(1);
                int fullTextWidth = uiSprite.textWidth(modeStr);
                // Actualizar el desplazamiento horizontal suave
                if (fullTextWidth > textAreaW) {
                    const unsigned long frameInterval = 50; // 50ms para un movimiento suave
                    unsigned long now = millis();
                    if (now - modeLastFrameTime >= frameInterval) {
                        modeTickerOffset += modeTickerDirection;
                        if (modeTickerOffset < 0) {
                            modeTickerOffset = 0;
                            modeTickerDirection = 1;
                        }
                        if (modeTickerOffset > (fullTextWidth - textAreaW)) {
                            modeTickerOffset = fullTextWidth - textAreaW;
                            modeTickerDirection = -1;
                        }
                        modeLastFrameTime = now;
                    }
                    // Dibujar la opción seleccionada en un sprite temporal
                    TFT_eSprite modeSprite = TFT_eSprite(&tft);
                    modeSprite.createSprite(textAreaW, CARD_HEIGHT);
                    modeSprite.fillSprite(BACKGROUND_COLOR);
                    modeSprite.setFreeFont(&FreeSans9pt7b);
                    modeSprite.setTextSize(1);
                    modeSprite.setTextDatum(TL_DATUM);
                    modeSprite.setTextColor(textColor, BACKGROUND_COLOR);
                    // Dibujar el texto desplazado: se dibuja en x = -modeTickerOffset para que el extremo izquierdo sea visible
                    modeSprite.drawString(modeStr, -modeTickerOffset, 0);
                    // Empujar el sprite temporal sobre el sprite principal
                    modeSprite.pushToSprite(&uiSprite, x, y);
                    modeSprite.deleteSprite();
                } else {
                    // Si cabe, dibujarlo de forma normal
                    uiSprite.drawString(modeStr, x, y);
                }
            } else {
                // Para las opciones no seleccionadas, dibujar el texto y cubrir los extremos fuera del área [10,120]
                uiSprite.drawString(modeStr, x, y);
                uiSprite.fillRect(0, y, x, CARD_HEIGHT, BACKGROUND_COLOR);
                uiSprite.fillRect(120, y, 128 - 120, CARD_HEIGHT, BACKGROUND_COLOR);
            }
        }
    }

    // Dibujar la barra de desplazamiento vertical
    const int scrollBarMargin = 3;
    const int scrollBarWidth = 5;
    int scrollBarY = 30;
    uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN, TFT_DARKGREY);
    if (totalModes > visibleOptions) {
        float thumbRatio = (float)visibleOptions / (float)totalModes;
        int thumbHeight = max(20, (int)((visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN) * thumbRatio));
        int thumbY = scrollBarY + (visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN - thumbHeight)
                     * (float)(currentModeIndex - startIndex) / (float)(totalModes - visibleOptions);
        uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    } else {
        uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN, TFT_LIGHTGREY);
    }

    // Empujar el sprite principal a la pantalla (actualización única)
    uiSprite.pushSprite(0, 0);

    // Actualizar scroll vertical suave (manteniendo la lógica original)
    if (visibleCurrentModeIndex >= 0) {
        targetScrollOffset = visibleCurrentModeIndex * (CARD_HEIGHT + CARD_MARGIN);
        if (targetScrollOffset > totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100) {
            targetScrollOffset = totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100;
        }
        if (targetScrollOffset < 0) targetScrollOffset = 0;
    }
    scrollOffset += (targetScrollOffset - scrollOffset) / 4;

    memcpy(globalVisibleModesMap, visibleModesMap, sizeof(visibleModesMap));
}


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
const int visibleOptions = 3;

void drawHiddenMenu(int selection)
{
    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Draw title
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString("AJUSTES", 64, 5);

    int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));

    // Posición X de inicio del texto
    int x = 9;  // margen izquierdo
    // Calcular el límite derecho a partir de la posición de la barra vertical
    const int scrollBarMargin = 3;
    const int scrollBarWidth = 5;
    int scrollBarX = 128 - scrollBarWidth - scrollBarMargin;
    // El ancho de la zona de texto es el espacio entre x y la barra vertical (dejando un pequeño margen)
    int textAreaW = scrollBarX - x - 2; // 2 píxeles de margen extra

    for (int i = 0; i < visibleOptions && (startIndex + i) < numOptions; i++)
    {
        int currentIndex = startIndex + i;
        int y = 30 + i * (cardHeight + cardMargin);

        bool isSelected = (currentIndex == selection);
        uint32_t textColor = isSelected ? HIGHLIGHT_COLOR : TEXT_COLOR;

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextColor(textColor);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);

        // Siempre recortar el texto para que no se dibuje por debajo de la barra
        String tempStr = menuOptions[currentIndex];
        while (uiSprite.textWidth(tempStr) > textAreaW && tempStr.length() > 0) {
            tempStr.remove(tempStr.length() - 1);
        }
        uiSprite.drawString(tempStr, x, y);
    }

    // Dibujar la barra vertical de scroll
    const int scrollBarHeight = visibleOptions * (cardHeight + cardMargin) - cardMargin;
    const int scrollBarY = 30;
    uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    if (numOptions > visibleOptions) {
        float thumbRatio = (float)visibleOptions / (float)numOptions;
        int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
        int thumbY = scrollBarY + (scrollBarHeight - thumbHeight)
                     * (float)(selection - startIndex) / (float)(numOptions - visibleOptions);
        uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    }
    else {
        uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
        
    }
    uiSprite.pushSprite(0, 0);
}

void scrollTextTickerBounce(int selection)
{
    if (!hiddenMenuActive) return;

    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int visibleOptions = 3;
    const unsigned long frameInterval = 200;
    
    static int charOffsets[6] = {0,0,0,0,0,0}; 
    static int scrollDirections[6] = {1,1,1,1,1,1};
    static unsigned long lastFrameTime = 0;

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextWrap(false);
    tft.setTextSize(1);

    if (selection < 0 || selection >= numOptions) return;
    String text = menuOptions[selection];

    int fullTextWidth = tft.textWidth(text.c_str());
    int textVisibleW = cardWidth;  // Adjusted to match drawHiddenMenu
    if (fullTextWidth <= textVisibleW) return;

    int chunkSize = 0;
    while (chunkSize < text.length() && tft.textWidth(text.substring(0, chunkSize + 1)) <= textVisibleW) {
        chunkSize++;
    }
    if(chunkSize == 0) chunkSize = 1;

    int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));
    int cardIndex = selection - startIndex;
    if (cardIndex < 0 || cardIndex >= visibleOptions) return;

    int cardY = 30 + cardIndex * (cardHeight + cardMargin);
    int textAreaX = 15;  // Adjusted to match drawHiddenMenu
    int textAreaY = cardY + 1;
    int textAreaW = textVisibleW - 9;
    int textAreaH = cardHeight - 2;

    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
        charOffsets[selection] += scrollDirections[selection];

        if (charOffsets[selection] < 0) {
            charOffsets[selection] = 0;
            scrollDirections[selection] = +1;
        }

        int maxOffset = text.length() - chunkSize;
        if (maxOffset < 0) maxOffset = 0;
        if (charOffsets[selection] > maxOffset) {
            charOffsets[selection] = maxOffset;
            scrollDirections[selection] = -1;
        }
        lastFrameTime = now;
    }

    TFT_eSprite tickerSprite(&tft);
    if (tickerSprite.createSprite(textAreaW, textAreaH) == nullptr) return;

    tickerSprite.setFreeFont(&FreeSans9pt7b);
    tickerSprite.setTextWrap(false);
    tickerSprite.setTextSize(1);
    tickerSprite.setTextColor(HIGHLIGHT_COLOR);
    tickerSprite.setTextDatum(TL_DATUM);

    tickerSprite.fillSprite(BACKGROUND_COLOR);

    int offset = charOffsets[selection];
    if (offset < 0) offset = 0;
    if (offset >= (int)text.length()) offset = text.length() - 1;
    int endIndex = offset + chunkSize;
    if (endIndex > (int)text.length()) {
        endIndex = text.length();
    }
    String sliceStr = text.substring(offset, endIndex);
    if (sliceStr.isEmpty()) {
        sliceStr = "";
    }

    tickerSprite.drawString(sliceStr, -charOffsets[selection], 0);

    tickerSprite.pushSprite(textAreaX, textAreaY);
    tickerSprite.deleteSprite();
}