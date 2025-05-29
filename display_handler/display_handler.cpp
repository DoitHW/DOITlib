#include <display_handler/display_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include "rom/rtc.h"    // Opcional, si quieres info de reset
#include <icons_64x64_DMS/icons_64x64_DMS.h>
#include <Translations_handler/translations.h>
#include <botonera_DMS/botonera_DMS.h>

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

bool brightnessMenuActive = false;
uint8_t currentBrightness = 100;       // Valor actual en porcentaje
uint8_t tempBrightness = currentBrightness;  // Valor temporal mientras se ajusta

void drawBrightnessMenu(uint8_t brightness) {
    // Clear the sprite with background color
    uiSprite.fillSprite(BACKGROUND_COLOR);
    
    // Draw header with proper centering
    uiSprite.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    uiSprite.setTextSize(1);
    // Use text datum to center text properly
    uiSprite.setTextDatum(MC_DATUM); // Middle-Center alignment
    uiSprite.drawString("Ajustar Brillo", 64, 15, 2); // Centered at x=64 (middle of 128px screen)
    
    // Draw sun icon, moved slightly left for better balance
    drawSunIcon(30, 45, TFT_YELLOW);
    
    // Draw rounded background bar, adjusted position
    uiSprite.fillRoundRect(45, 45, 70, 10, 5, TFT_DARKGREY);
    
    // Draw active progress bar with rounded corners
    int barWidth = map(brightness, 0, 100, 0, 70);
    if (barWidth > 0) {
        // Use gradient color based on brightness
        uint16_t barColor = brightness < 30 ? TFT_ORANGE : 
                           (brightness < 70 ? TFT_YELLOW : TFT_WHITE);
        uiSprite.fillRoundRect(45, 45, barWidth, 10, 5, barColor);
    }
    
    // Draw percentage with proper centering
    uiSprite.drawString(String(brightness) + "%", 64, 70, 2); // Centered text
    
    // Draw control hints at bottom, properly centered
    uiSprite.setTextSize(1);
    uiSprite.drawString("- / +", 64, 100, 1); // Control hints
    
    // Draw border around the entire interface, properly centered
    uiSprite.drawRoundRect(4, 4, 120, 120, 8, TFT_DARKGREY);
    
    // Push sprite to display
    uiSprite.pushSprite(0, 0);
}

// Helper function to draw a sun icon
void drawSunIcon(int16_t x, int16_t y, uint16_t color) {
    // Center circle
    uiSprite.fillCircle(x, y, 6, color);
    
    // Sun rays
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        int x1 = x + 8 * cos(angle);
        int y1 = y + 8 * sin(angle);
        int x2 = x + 12 * cos(angle);
        int y2 = y + 12 * sin(angle);
        uiSprite.drawLine(x1, y1, x2, y2, color);
    }
}

void display_init() {
    //DEBUG__________ln("Inicializando display...");
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setSwapBytes(true);
    uiSprite.createSprite(tft.width(), tft.height());
    uiSprite.setSwapBytes(true);
    //DEBUG__________ln("Display inicializado.");
    
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
    DEBUG__________ln(message);
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
    //int drawX = 0 - nameScrollOffset;
    int drawX = (DISPLAY_WIDTH - textWidth) / 2 - nameScrollOffset;

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
    
    //int maxScroll = textWidth - (DISPLAY_WIDTH - 10);  // Ajustamos el rango con el margen
    int centerOffset = (DISPLAY_WIDTH - textWidth) / 2;
    int maxScroll = abs(centerOffset) + textWidth - DISPLAY_WIDTH;
    int minScroll = -maxScroll;

    if (millis() - lastNameScrollUpdate >= NAME_SCROLL_INTERVAL) {
         nameScrollOffset += nameScrollDirection;
         if (nameScrollOffset <= minScroll) {
            nameScrollOffset = minScroll;
            nameScrollDirection = 1;
        } else if (nameScrollOffset >= maxScroll) {
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
const int MODE_SCROLL_INTERVAL = 60;         // Intervalo de actualización (ms)

// Variables globales para el scroll del nombre del modo
bool modeScrollActive = false;             // Se activa si el texto del modo es demasiado largo
String currentModeDisplayName = "";        // Cadena que contiene el nombre del modo (ya procesado)
int modeScrollOffset = 0;                  // Desplazamiento actual en píxeles para el modo
int modeScrollDirection = 1;               // Dirección: 1 para incrementar, -1 para decrementar
unsigned long lastModeScrollUpdate = 0;    // Última actualización en ms

// Actualiza la zona del nombre del modo usando el desplazamiento actual
void updateModeDisplay() {
    // Establecer fuente y tamaño antes de calcular el ancho
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextSize(1);

    int textWidth = uiSprite.textWidth(currentModeDisplayName);
    int centerX = (MODE_DISPLAY_WIDTH - textWidth) / 2;

    // Ajustar desplazamiento desde el centro
    int drawX = centerX - modeScrollOffset;
    int drawY = MODE_Y;

    // Borrar el área donde se dibuja el nombre del modo
    uiSprite.fillRect(0, drawY, MODE_DISPLAY_WIDTH, MODE_AREA_HEIGHT, BACKGROUND_COLOR);

    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextSize(1);
    uiSprite.setTextDatum(TL_DATUM); // Mantener alineación izquierda para el scroll
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString(currentModeDisplayName, drawX, drawY);

    // Refrescar solo la parte afectada en la pantalla
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

    int extraMargin = 10;
    
    // Ajuste del desplazamiento máximo para centrar correctamente
    int maxScroll = (textWidth - MODE_DISPLAY_WIDTH) / 2 + extraMargin;
    int minScroll = -maxScroll; // Permitir que el texto se desplace en ambas direcciones

    if (millis() - lastModeScrollUpdate >= MODE_SCROLL_INTERVAL) {
         modeScrollOffset += modeScrollDirection;

         // Invertir dirección del scroll al alcanzar los límites
         if (modeScrollOffset <= minScroll) {
              modeScrollOffset = minScroll;
              modeScrollDirection = 1;
         } else if (modeScrollOffset >= maxScroll) {
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

    // Cargar el vector de estados alternativos desde el mapa si existe,
    // de lo contrario se inicializará en el bloque correspondiente
    if (elementAlternateStates.find(currentFile) != elementAlternateStates.end()) {
        currentAlternateStates = elementAlternateStates[currentFile];
    } else {
        currentAlternateStates = initializeAlternateStates(currentFile);
        elementAlternateStates[currentFile] = currentAlternateStates;
    }
    
    // Función auxiliar para obtener un nombre "amigable" de un archivo
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
         return (count > 1) ? (name + "(" + String(count) + ")") : name;
    };

    // --- Caso 1: Elementos fijos ("Ambientes" o "Fichas") ---
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
         INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
         currentMode = option->currentMode;
         int startX = (tft.width() - 64) / 2;
         int startY = (tft.height() - 64) / 2 - 20;
         for (int y = 0; y < 64; y++) {
             memcpy(lineBuffer, option->icono[y], 64 * 2);
             uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
         }
         // Dibujar el nombre del elemento
                  // **** MODIFICACIÓN: Obtener el nombre traducido ****
         // Si es "Ambientes" usamos la clave "AMBIENTES", si es "Fichas", "FICHAS"
         String displayName = (currentFile == "Ambientes") ? String(getTranslation("AMBIENTES")) 
                                                           : String(getTranslation("FICHAS"));
         // ******************************************************
         
         uiSprite.setFreeFont(&FreeSansBold12pt7b);
         uiSprite.setTextSize(1);
         //String displayName = String((char*)option->name);
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
         // --- Construcción del vector de estados alternativos visible ---
         int visibleModeIndex = -1;
         if (elementAlternateStates.find(currentFile) == elementAlternateStates.end()) {
             std::vector<bool> tempAlternate;
             int count = 0;
             for (int i = 0; i < 16; i++) {
                 if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                     tempAlternate.push_back(false);
                     if (i == option->currentMode) {
                         visibleModeIndex = count;
                     }
                     count++;
                 }
             }
             currentAlternateStates = tempAlternate;
             elementAlternateStates[currentFile] = currentAlternateStates;
         } else {
             currentAlternateStates = elementAlternateStates[currentFile];
             int count = 0;
             for (int i = 0; i < 16; i++) {
                 if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                     if (i == option->currentMode) {
                         visibleModeIndex = count;
                         break;
                     }
                     count++;
                 }
             }
         }
         // Mostrar el nombre del modo en la pantalla principal usando el índice visible
        // Obtener el nombre del modo en formato String (clave de traducción)
        String modeNameStr = String((char*)option->mode[option->currentMode].name);
        // Traducir el nombre del modo según el idioma actual
        modeNameStr = String(getTranslation(modeNameStr.c_str()));

        String modeDisplay;
        if (visibleModeIndex >= 0 && currentAlternateStates.size() > (size_t)visibleModeIndex) {
            modeDisplay = getModeDisplayName(modeNameStr, currentAlternateStates[visibleModeIndex]);
        } else {
            modeDisplay = modeNameStr;
        }
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
              uiSprite.setFreeFont(&FreeSans9pt7b);
              uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
              uiSprite.setTextDatum(TC_DATUM);
              uiSprite.setTextSize(1);
              uiSprite.drawString(modeDisplay, tft.width() / 2, tft.height() - 15);
         }
         //drawSelectionCircle(selectedStates[currentIndex], startX, startY);
    }
    // --- Caso 2: "Apagar" ---
    else if (currentFile == "Apagar") {
         INFO_PACK_T* option = &apagarSala;
         int startX = (tft.width() - 64) / 2;
         int startY = (tft.height() - 64) / 2 - 20;
         for (int y = 0; y < 64; y++) {
             memcpy(lineBuffer, option->icono[y], 64 * 2);
             uiSprite.pushImage(startX, startY + y, 64, 1, lineBuffer);
         }
         currentAlternateStates.clear();
         currentAlternateStates.push_back(false);
         elementAlternateStates[currentFile] = currentAlternateStates;
         //drawElementName((char*)option->name, false);
        // **** MODIFICACIÓN: Dibujar el nombre traducido para "Apagar" ****
         drawElementName(getTranslation("APAGAR"), false);
        // *********************************************************************
         drawModeName((char*)option->mode[currentMode].name);
         nameScrollActive = false;
         modeScrollActive = false;
    }
    // --- Caso 3: Elemento almacenado en SPIFFS ---
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
         //DEBUG__________ln("DEBUG: Tamaño de elemento '" + displayName + "' = " + String(elementTextWidth));
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
         int realModeIndex = 0;
         f.seek(OFFSET_CURRENTMODE, SeekSet);
         f.read((uint8_t*)&realModeIndex, 1);
         const int OFFSET_ALTERNATE_STATES = OFFSET_CURRENTMODE + 1;
         byte storedStates[16] = {0};
         f.seek(OFFSET_ALTERNATE_STATES, SeekSet);
         size_t bytesRead = f.read(storedStates, 16);
         if (elementAlternateStates.find(currentFile) == elementAlternateStates.end()) {
             std::vector<bool> tempAlternate;
             int visibleModeIndex = -1;
             int count = 0;
             for (int i = 0; i < 16; i++) {
                 char modeBuf[25] = {0};
                 byte modeConfig[2] = {0};
                 f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                 f.read((uint8_t*)modeBuf, 24);
                 f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                 f.read(modeConfig, 2);
                 if (strlen(modeBuf) > 0 && checkMostSignificantBit(modeConfig)) {
                     bool hasAlt = getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE);
                     bool altState = (bytesRead > 0 && count < 16) ? (storedStates[count] > 0) : false;
                     tempAlternate.push_back(hasAlt ? altState : false);
                     if (i == realModeIndex) {
                         visibleModeIndex = count;
                     }
                     count++;
                 }
             }
             currentAlternateStates = tempAlternate;
             elementAlternateStates[currentFile] = currentAlternateStates;
             DEBUG__________ln("DEBUG: [SPIFFS] currentAlternateStates.size() = " + String(currentAlternateStates.size()));
         } else {
             currentAlternateStates = elementAlternateStates[currentFile];
         }
         int visibleIndex = -1;
         {
             int count = 0;
             for (int i = 0; i < 16; i++) {
                 char modeBuf[25] = {0};
                 byte tempConfig[2] = {0};
                 f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                 f.read((uint8_t*)modeBuf, 24);
                 f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                 f.read(tempConfig, 2);
                 if (strlen(modeBuf) > 0 && checkMostSignificantBit(tempConfig)) {
                     if (i == realModeIndex) {
                         visibleIndex = count;
                         break;
                     }
                     count++;
                 }
             }
         }
         String modeDisplay = String(modeName);
         if (visibleIndex >= 0 && currentAlternateStates.size() > (size_t)visibleIndex) {
             modeDisplay = getModeDisplayName(modeDisplay, currentAlternateStates[visibleIndex]);
         }
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
              drawModeName(modeDisplay.c_str());
         }
         f.close();
         //drawSelectionCircle(selectedStates[currentIndex], startX, startY);
         currentMode = realModeIndex;
    }
    currentModeIndex = currentMode;
    colorHandler.setCurrentFile(currentFile);
    colorHandler.setPatternBotonera(currentModeIndex, ledManager);
    drawNavigationArrows();
    
    // Si el sistema está bloqueado, dibujar el icono de candado en la esquina superior izquierda.
    // Se asume que 'device_lock' es la imagen almacenada en RAM y sus dimensiones son 16x16 (ajustar si es necesario).
    // if (!inModesScreen && systemLocked) {
    //      uiSprite.pushImage(0, 0, 16, 16, device_lock);
    // }

    if (isInMainMenu() && systemLocked) {
         uiSprite.pushImage(0, 0, 16, 16, device_lock);
    }

    drawBatteryIconMini(batteryPercentage);

    uiSprite.pushSprite(0, 0);
    lastDisplayInteraction = millis();
    displayOn = true;
}

void display_sleep() {
    tft.fillScreen(TFT_BLACK);
    displayOn = false;
    DEBUG__________ln("Pantalla apagada por inactividad.");
  }
  
void display_wakeup() {
    displayOn = true;
    // Vuelve a dibujar el menú principal para “despertar” la interfaz
    if (inCognitiveMenu) {
        drawCognitiveMenu();
    }else drawCurrentElement();
    lastDisplayInteraction = millis();
    DEBUG__________ln("Pantalla reactivada por interacción.");
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


std::map<String, std::vector<bool>> elementAlternateStates;
std::vector<bool> currentAlternateStates;


// void drawModesScreen() {
//     // Variables para scroll vertical suave.
//     static int scrollOffset = 0;
//     static int targetScrollOffset = 0;
//     const int visibleOptions = 4; // Número máximo de opciones visibles.

//     // Variables para scroll horizontal del texto de la opción seleccionada.
//     static int modeTickerOffset = 0;
//     static int modeTickerDirection = 1;
//     static unsigned long modeLastFrameTime = 0;
//     static int lastSelectedMode = -1;

//     uiSprite.fillSprite(BACKGROUND_COLOR);

//     uiSprite.setFreeFont(&FreeSans12pt7b);
//     uiSprite.setTextColor(TEXT_COLOR);
//     uiSprite.setTextDatum(TC_DATUM);
//     uiSprite.setTextSize(1);
//     uiSprite.drawString(getTranslation("MODOS"), 64, 5);

//     String currentFile = elementFiles[currentIndex];

//     int visibleModesMap[18];
//     visibleModesMap[0] = -3;
//     int count = 1;

//     if (currentFile == "Ambientes" || currentFile == "Fichas") {
//         INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
//         for (int i = 0; i < 16; i++) {
//             if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
//                 visibleModesMap[count] = i;
//                 count++;
//             }
//         }
//     } else if (currentFile != "Apagar") {
//         fs::File f = SPIFFS.open(currentFile, "r");
//         if (f) {
//             for (int i = 0; i < 16; i++) {
//                 char modeName[25] = {0};
//                 byte modeConfig[2] = {0};
//                 f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
//                 f.read((uint8_t*)modeName, 24);
//                 f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
//                 f.read(modeConfig, 2);
//                 if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
//                     visibleModesMap[count] = i;
//                     count++;
//                 }
//             }
//             f.close();
//         }
//     }

//     visibleModesMap[count] = -2;
//     count++;
//     totalModes = count;

//     if (currentModeIndex < 0 || currentModeIndex >= totalModes) {
//         currentModeIndex = 0;
//     }

//     memcpy(globalVisibleModesMap, visibleModesMap, sizeof(int) * totalModes);

//     int startIndex = max(0, min(currentModeIndex - visibleOptions / 2, totalModes - visibleOptions));

//     const int x = 10;
//     const int textAreaW = 110;
//     const int scrollBarX = 120;

//     for (int i = 0; i < visibleOptions && (startIndex + i) < totalModes; i++) {
//         int currentVisibleIndex = startIndex + i;
//         int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN);
//         bool isSelected = (currentVisibleIndex == currentModeIndex);
//         uint32_t textColor = TEXT_COLOR;

//         // Dibujar fondo si está seleccionado
//         if (isSelected) {
//             uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, CARD_HEIGHT + 2, 3, CARD_COLOR);
//             textColor = TEXT_COLOR;
//         }

//         uiSprite.setFreeFont(&FreeSans9pt7b);
//         uiSprite.setTextColor(textColor);
//         uiSprite.setTextSize(1);
//         uiSprite.setTextDatum(TL_DATUM);

//         int modeVal = visibleModesMap[currentVisibleIndex];
//         String label;

//         if (modeVal == -2) {
//             label = getTranslation("VOLVER");
//         } else if (modeVal == -3) {
//             label = selectedStates[currentIndex] ? getTranslation("APAGAR") : getTranslation("ENCENDER");
//         } else {
//             char modeName[25] = {0};
//             if (currentFile == "Ambientes" || currentFile == "Fichas") {
//                 INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
//                 strncpy(modeName, (char*)option->mode[modeVal].name, 24);
//             } else {
//                 fs::File f = SPIFFS.open(currentFile, "r");
//                 if (f) {
//                     f.seek(OFFSET_MODES + modeVal * SIZE_MODE, SeekSet);
//                     f.read((uint8_t*)modeName, 24);
//                     f.close();
//                 }
//             }
//             label = String(modeName);
//             if (currentFile == "Ambientes" || currentFile == "Fichas") {
//                 label = getTranslation(label.c_str());
//             }

//             bool modeAlternateState = false;
//             if (currentAlternateStates.size() > (size_t)(currentVisibleIndex - 1) && visibleModesMap[currentVisibleIndex] != -2) {
//                 modeAlternateState = currentAlternateStates[currentVisibleIndex - 1];
//             }
//             label = getModeDisplayName(label, modeAlternateState);
//         }

//         if (isSelected) {
//             if (currentModeIndex != lastSelectedMode) {
//                 modeTickerOffset = 0;
//                 modeTickerDirection = 1;
//                 lastSelectedMode = currentModeIndex;
//             }

//             int fullTextWidth = uiSprite.textWidth(label);
//             if (fullTextWidth > textAreaW) {
//                 const unsigned long frameInterval = 50;
//                 unsigned long now = millis();
//                 if (now - modeLastFrameTime >= frameInterval) {
//                     modeTickerOffset += modeTickerDirection;
//                     if (modeTickerOffset < 0) {
//                         modeTickerOffset = 0;
//                         modeTickerDirection = 1;
//                     }
//                     if (modeTickerOffset > (fullTextWidth - textAreaW)) {
//                         modeTickerOffset = fullTextWidth - textAreaW;
//                         modeTickerDirection = -1;
//                     }
//                     modeLastFrameTime = now;
//                 }
//                 TFT_eSprite modeSprite = TFT_eSprite(&tft);
//                 modeSprite.createSprite(textAreaW, CARD_HEIGHT);
//                 modeSprite.fillSprite(CARD_COLOR);
//                 modeSprite.setFreeFont(&FreeSans9pt7b);
//                 modeSprite.setTextSize(1);
//                 modeSprite.setTextDatum(TL_DATUM);
//                 modeSprite.setTextColor(TEXT_COLOR, CARD_COLOR);
//                 modeSprite.drawString(label, -modeTickerOffset, 0);
//                 modeSprite.pushToSprite(&uiSprite, x, y);
//                 modeSprite.deleteSprite();
//             } else {
//                 uiSprite.drawString(label, x, y);
//             }
//         } else {
//             uiSprite.drawString(label, x, y);
//         }
//     }

//     const int scrollBarWidth = 5;
//     int scrollBarY = 30;
//     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN, TFT_DARKGREY);
//     if (totalModes > visibleOptions) {
//         float thumbRatio = (float)visibleOptions / (float)totalModes;
//         int thumbHeight = max(20, (int)((visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN) * thumbRatio));
//         int thumbY = scrollBarY + (visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN - thumbHeight)
//                      * (float)(currentModeIndex - startIndex) / (float)(totalModes - visibleOptions);
//         uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
//     } else {
//         uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN, TFT_LIGHTGREY);
//     }

//     uiSprite.pushSprite(0, 0);

//     int visibleCurrentModeIndex = currentModeIndex;
//     if (visibleCurrentModeIndex >= 0) {
//         targetScrollOffset = visibleCurrentModeIndex * (CARD_HEIGHT + CARD_MARGIN);
//         if (targetScrollOffset > totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100) {
//             targetScrollOffset = totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100;
//         }
//         if (targetScrollOffset < 0) targetScrollOffset = 0;
//     }
//     scrollOffset += (targetScrollOffset - scrollOffset) / 4;
// }

void drawModesScreen() {
    // Variables para scroll vertical suave.
    static int scrollOffset = 0;
    static int targetScrollOffset = 0;
    const int visibleOptions = 4; // Número máximo de opciones visibles.

    // Variables para scroll horizontal del texto de la opción seleccionada.
    static int modeTickerOffset = 0;
    static int modeTickerDirection = 1;
    static unsigned long modeLastFrameTime = 0;
    static int lastSelectedMode = -1;

    uiSprite.fillSprite(BACKGROUND_COLOR);

    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(getTranslation("MODOS"), 64, 5);

    String currentFile = elementFiles[currentIndex];

    int visibleModesMap[18];
    visibleModesMap[0] = -3;
    int count = 1;

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                visibleModesMap[count++] = i;
            }
        }
    } else if (currentFile != "Apagar") {
        fs::File f = SPIFFS.open(currentFile, "r");
        if (f) {
            for (int i = 0; i < 16; i++) {
                char modeName[25] = {0};
                byte modeConfig[2] = {0};
                f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                f.read((uint8_t*)modeName, 24);
                f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                f.read(modeConfig, 2);
                if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
                    visibleModesMap[count++] = i;
                }
            }
            f.close();
        }
    }

    visibleModesMap[count++] = -2;
    totalModes = count;

    if (currentModeIndex < 0 || currentModeIndex >= totalModes) {
        currentModeIndex = 0;
    }

    memcpy(globalVisibleModesMap, visibleModesMap, sizeof(int) * totalModes);

    // Ventana rodante: desplaza startIndex uno en vez de centrar
    static int startIndex = 0;
    if ((currentModeIndex - startIndex) > (visibleOptions - 2)
        && startIndex < (totalModes - visibleOptions)) {
        startIndex++;
    }
    else if ((currentModeIndex - startIndex) < 1
             && startIndex > 0) {
        startIndex--;
    }

    const int x = 10;
    const int textAreaW = 110;
    const int scrollBarX = 120;

    // Dibujar cada opción visible
    for (int i = 0; i < visibleOptions && (startIndex + i) < totalModes; i++) {
        int currentVisibleIndex = startIndex + i;
        int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN);
        bool isSelected = (currentVisibleIndex == currentModeIndex);

        // Fondo del recuadro si está seleccionado
        if (isSelected) {
            uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, CARD_HEIGHT + 2, 3, CARD_COLOR);
        }

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextColor(TEXT_COLOR);

        int modeVal = visibleModesMap[currentVisibleIndex];
        String label;

        if (modeVal == -2) {
            label = getTranslation("VOLVER");
        } else if (modeVal == -3) {
            label = selectedStates[currentIndex]
                    ? getTranslation("APAGAR")
                    : getTranslation("ENCENDER");
        } else {
            char modeName[25] = {0};
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes")
                                      ? &ambientesOption
                                      : &fichasOption;
                strncpy(modeName, (char*)option->mode[modeVal].name, 24);
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_MODES + modeVal * SIZE_MODE, SeekSet);
                    f.read((uint8_t*)modeName, 24);
                    f.close();
                }
            }
            label = String(modeName);
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                label = getTranslation(label.c_str());
            }

            bool modeAlternateState = false;
            if (currentAlternateStates.size() > (size_t)(currentVisibleIndex - 1)
                && visibleModesMap[currentVisibleIndex] != -2) {
                modeAlternateState = currentAlternateStates[currentVisibleIndex - 1];
            }
            label = getModeDisplayName(label, modeAlternateState);
        }

        if (isSelected) {
            if (currentModeIndex != lastSelectedMode) {
                modeTickerOffset = 0;
                modeTickerDirection = 1;
                lastSelectedMode = currentModeIndex;
            }

            int fullTextWidth = uiSprite.textWidth(label);
            if (fullTextWidth > textAreaW) {
                const unsigned long frameInterval = 50;
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
                TFT_eSprite modeSprite(&tft);
                modeSprite.createSprite(textAreaW, CARD_HEIGHT);
                modeSprite.fillSprite(CARD_COLOR);
                modeSprite.setFreeFont(&FreeSans9pt7b);
                modeSprite.setTextSize(1);
                modeSprite.setTextDatum(TL_DATUM);
                modeSprite.setTextColor(TEXT_COLOR, CARD_COLOR);
                modeSprite.drawString(label, -modeTickerOffset, 0);
                modeSprite.pushToSprite(&uiSprite, x, y);
                modeSprite.deleteSprite();
            } else {
                uiSprite.drawString(label, x, y);
            }
        } else {
            uiSprite.drawString(label, x, y);
        }
    }

    uiSprite.pushSprite(0, 0);

    // Actualizar scroll vertical suave
    int visibleCurrentModeIndex = currentModeIndex;
    if (visibleCurrentModeIndex >= 0) {
        targetScrollOffset = visibleCurrentModeIndex * (CARD_HEIGHT + CARD_MARGIN);
        if (targetScrollOffset > totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100) {
            targetScrollOffset = totalModes * (CARD_HEIGHT + CARD_MARGIN) - 100;
        }
        if (targetScrollOffset < 0) targetScrollOffset = 0;
    }
    scrollOffset += (targetScrollOffset - scrollOffset) / 4;
}



// Opciones del menú oculto
const char* menuOptions[] = {
    "IDIOMA",
    "SONIDO",
    "BRILLO",
    "CONTROL_SALA_MENU",
    "VOLVER"
};
const int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);

// Número máximo de opciones visibles
const int visibleOptions = 4;

// void drawHiddenMenu(int selection)
// {
//     const int cardWidth = 110;
//     const int cardHeight = CARD_HEIGHT;
//     const int cardMargin = CARD_MARGIN;

//     uiSprite.fillSprite(BACKGROUND_COLOR);

//     // Dibujar el título (ya traducido)
//     uiSprite.setFreeFont(&FreeSans12pt7b);
//     uiSprite.setTextColor(TEXT_COLOR);
//     uiSprite.setTextDatum(TC_DATUM);
//     uiSprite.setTextSize(1);
//     uiSprite.drawString(getTranslation("MENU_AJUSTES"), 64, 5);

//     int startIndex = max(0, min(selection - visibleOptions / 2, numOptions - visibleOptions));

//     // Posición X de inicio del texto
//     int x = 9;
//     const int scrollBarMargin = 3;
//     const int scrollBarWidth = 5;
//     int scrollBarX = 128 - scrollBarWidth - scrollBarMargin;
//     int textAreaW = scrollBarX - x - 2;

//     for (int i = 0; i < visibleOptions && (startIndex + i) < numOptions; i++)
//     {
//         int currentIndex = startIndex + i;
//         int y = 30 + i * (cardHeight + cardMargin);
//         bool isSelected = (currentIndex == selection);

//         // Dibujar recuadro de fondo si está seleccionado
//         if (isSelected) {
//             uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, cardHeight + 2, 3, CARD_COLOR);
//         }

//         uiSprite.setFreeFont(&FreeSans9pt7b);
//         uiSprite.setTextColor(TEXT_COLOR);
//         uiSprite.setTextSize(1);
//         uiSprite.setTextDatum(TL_DATUM);

//         // Obtener y truncar el texto si excede el ancho disponible
//         String tempStr = String(getTranslation(menuOptions[currentIndex]));
//         while (uiSprite.textWidth(tempStr) > textAreaW && tempStr.length() > 0) {
//             tempStr.remove(tempStr.length() - 1);
//         }

//         uiSprite.drawString(tempStr, x, y);
//     }

//     // Dibujar barra de scroll vertical
//     const int scrollBarHeight = visibleOptions * (cardHeight + cardMargin) - cardMargin;
//     const int scrollBarY = 30;
//     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

//     if (numOptions > visibleOptions) {
//         float thumbRatio = (float)visibleOptions / (float)numOptions;
//         int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
//         int thumbY = scrollBarY + (scrollBarHeight - thumbHeight)
//                      * (float)(selection - startIndex) / (float)(numOptions - visibleOptions);
//         uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
//     }
//     else {
//         uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
//     }

//     uiSprite.pushSprite(0, 0);
// }

void drawHiddenMenu(int selection)
{
    const int cardWidth    = 110;
    const int cardHeight   = CARD_HEIGHT;
    const int cardMargin   = CARD_MARGIN;

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Dibujar el título (ya traducido)
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(getTranslation("MENU_AJUSTES"), 64, 5);

    // Ventana rodante: desplaza startIndex de uno en uno
    static int startIndex = 0;
    // Si avanzamos más allá de la penúltima fila visible, desplazamos hacia abajo
    if ((selection - startIndex) > (visibleOptions - 2) 
        && startIndex < (numOptions - visibleOptions)) {
        startIndex++;
    }
    // Si retrocedemos más allá de la primera fila visible, desplazamos hacia arriba
    else if ((selection - startIndex) < 1 
             && startIndex > 0) {
        startIndex--;
    }

    // Posición X y cálculo del área de texto
    int x = 9;
    const int scrollBarMargin = 3;
    const int scrollBarWidth  = 5;
    int scrollBarX = 128 - scrollBarWidth - scrollBarMargin;
    int textAreaW  = scrollBarX - x - 2;

    // Dibujar cada opción
    for (int i = 0; i < visibleOptions && (startIndex + i) < numOptions; i++)
    {
        int currentIndex = startIndex + i;
        int y = 30 + i * (cardHeight + cardMargin);
        bool isSelected = (currentIndex == selection);

        // Resaltar opción seleccionada
        if (isSelected) {
            uiSprite.fillRoundRect(
                x - 3, y - 1,
                textAreaW + 6, cardHeight + 2,
                3,
                CARD_COLOR
            );
        }

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextColor(TEXT_COLOR);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);

        // Obtener y truncar texto si excede el área
        String tempStr = String(getTranslation(menuOptions[currentIndex]));
        while (uiSprite.textWidth(tempStr) > textAreaW && tempStr.length() > 0) {
            tempStr.remove(tempStr.length() - 1);
        }

        uiSprite.drawString(tempStr, x, y);
    }

    // // Dibujar la barra de scroll vertical
    // const int scrollBarHeight = visibleOptions * (cardHeight + cardMargin) - cardMargin;
    // const int scrollBarY      = 30;
    // uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    // if (numOptions > visibleOptions) {
    //     float thumbRatio = (float)visibleOptions / (float)numOptions;
    //     int thumbHeight  = max(20, (int)(scrollBarHeight * thumbRatio));
    //     int thumbY       = scrollBarY
    //                      + (scrollBarHeight - thumbHeight)
    //                      * (float)(selection - startIndex) 
    //                        / (float)(numOptions - visibleOptions);
    //     uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    // } else {
    //     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
    // }

    uiSprite.pushSprite(0, 0);
}


void scrollTextTickerBounce(int selection)
{
    if (!hiddenMenuActive && !soundMenuActive) return;

    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int visibleOptions = 3;
    const unsigned long frameInterval = 10;
    
    static int charOffsets[5] = {0, 0, 0, 0, 0};
    static int scrollDirections[5] = {1, 1, 1, 1, 1};

    static unsigned long lastFrameTime = 0;

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextWrap(false);
    tft.setTextSize(1);

    if (selection < 0 || selection >= numOptions) return;
    String text = String(getTranslation(menuOptions[selection]));

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
    int textAreaX = 10;  // Adjusted to match drawHiddenMenu
    int textAreaY = cardY;
    int textAreaW = textVisibleW - 2;
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

String getModeDisplayName(const String &fullModeName, bool alternateActive) {
    int sepIndex = fullModeName.indexOf('/');
    // Si no hay separador, se devuelve la cadena completa
    if (sepIndex == -1) {
        return fullModeName;
    }
    // Devuelve la primera parte si no está activa la alternativa,
    // o la segunda parte si alternateActive es true.
    return alternateActive ? fullModeName.substring(sepIndex + 1) : fullModeName.substring(0, sepIndex);
}

// Función para dibujar el menú de selección de idioma con scroll vertical
void drawLanguageMenu(int selection) {
    // Opciones de idioma: se definen 8 opciones
    const char* languageOptions[] = {"ES", "ES(MX)", "CA", "EU", "FR", "DE", "EN"};
    const int numLanguages = 7;
    const int visibleOptions = 4;

    int startIndex = 0;
    if (selection >= visibleOptions) {
        startIndex = selection - visibleOptions + 1;
    }

    uiSprite.fillSprite(BACKGROUND_COLOR);

    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(getTranslation("IDIOMA"), 64, 5);

    const int startY = 30;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int x = 10;
    const int textAreaW = 110;

    for (int i = 0; i < visibleOptions; i++) {
        int optionIndex = startIndex + i;
        if (optionIndex >= numLanguages) break;

        int y = startY + i * (cardHeight + cardMargin);
        bool isSelected = (optionIndex == selection);

        if (isSelected) {
            uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, cardHeight + 2, 3, CARD_COLOR);
        }

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextColor(TEXT_COLOR);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.drawString(languageOptions[optionIndex], x, y);
    }

    uiSprite.pushSprite(0, 0);
}

// Índice del primer elemento visible en la ventana
extern int bankMenuVisibleItems;

void drawBankSelectionMenu(const std::vector<byte>& bankList, const std::vector<bool>& selectedBanks, int currentSelection, int windowOffset) {
    // Limpiar el sprite completo.
    uiSprite.fillSprite(BACKGROUND_COLOR);
    
    // Título centrado
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString("Familia", 64, 5);
    
    // Total de opciones: 1 para "Confirmar" + cantidad de banks
    int totalItems = bankList.size() + 1;
    const int visibleOptions = 4; // Número de opciones visibles
    const int x = 10;
    const int textAreaW = 110;
    const int scrollBarX = 120;
    
    // Definir el color celeste para las opciones seleccionadas
    const uint32_t SELECTED_OPTION_COLOR = TFT_CYAN; // Color celeste para opciones marcadas
    
    // Dibujar cada opción visible dentro de la ventana determinada por windowOffset
    for (int i = 0; i < visibleOptions && (windowOffset + i) < totalItems; i++) {
        int menuIndex = windowOffset + i;  // Índice global del item
        int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN); // Usando CARD_HEIGHT=20 y CARD_MARGIN=5, como en drawModesScreen
        bool isSelected = (menuIndex == currentSelection);
        bool isOptionChecked = (menuIndex > 0 && selectedBanks.size() > (size_t)(menuIndex - 1) && selectedBanks[menuIndex - 1]);
        
        // Determinar el color del texto según la selección actual y si está marcado
        uint32_t textColor;
        if (isSelected) {
            textColor = HIGHLIGHT_COLOR; // Prioridad al resaltado de la selección actual
        } else if (isOptionChecked) {
            textColor = SELECTED_OPTION_COLOR; // Color celeste para opciones marcadas pero no seleccionadas
        } else {
            textColor = TEXT_COLOR; // Color normal para opciones no marcadas
        }
        
        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextColor(textColor);
        
        String label;
        if (menuIndex == 0) {
            label = "Confirmar";
        } else {
            // Obtener el nombre de la familia asociado al bank
            byte bank = bankList[menuIndex - 1];
            String familyName = getFamilyNameFromBank(bank);
            label = familyName;
            
            // Ya no añadimos [X] porque se usa el color para indicar la selección
        }
        
        // Si la opción seleccionada tiene texto muy largo, aplicar ticker horizontal similar a drawModesScreen.
        int fullTextWidth = uiSprite.textWidth(label);
        if (isSelected && fullTextWidth > textAreaW) {
            static int tickerOffset = 0;
            static int tickerDirection = 1;
            static unsigned long lastFrameTime = 0;
            const unsigned long frameInterval = 50;
            unsigned long now = millis();
            if (now - lastFrameTime >= frameInterval) {
                tickerOffset += tickerDirection;
                if (tickerOffset < 0) {
                    tickerOffset = 0;
                    tickerDirection = 1;
                }
                if (tickerOffset > (fullTextWidth - textAreaW)) {
                    tickerOffset = fullTextWidth - textAreaW;
                    tickerDirection = -1;
                }
                lastFrameTime = now;
            }
            TFT_eSprite tickerSprite = TFT_eSprite(&tft);
            tickerSprite.createSprite(textAreaW, CARD_HEIGHT);
            tickerSprite.fillSprite(BACKGROUND_COLOR);
            tickerSprite.setFreeFont(&FreeSans9pt7b);
            tickerSprite.setTextSize(1);
            tickerSprite.setTextDatum(TL_DATUM);
            tickerSprite.setTextColor(textColor, BACKGROUND_COLOR);
            tickerSprite.drawString(label, -tickerOffset, 0);
            tickerSprite.pushToSprite(&uiSprite, x, y);
            tickerSprite.deleteSprite();
        } else {
            uiSprite.drawString(label, x, y);
        }
    }

    
    uiSprite.pushSprite(0, 0);
}

void drawSoundMenu(int selection) {
    const int scrollBarWidth = 5;
    const int visibleOptions = 4;
    const int totalOptions = 10;
    const char* options[] = {
        getTranslation("MUJER"),
        getTranslation("HOMBRE"),
        "---------",
        getTranslation("CON_NEG"),
        getTranslation("SIN_NEG"),
        "---------",
        getTranslation("VOL_NORMAL"),
        getTranslation("VOL_ATENUADO"),
        "---------",
        getTranslation("CONFIRMAR")
    };

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(getTranslation("SONIDO"), 64, 5);

    // Scroll vertical
    int startIndex = max(0, min(selection - visibleOptions / 2, totalOptions - visibleOptions));
    const int x = 9;
    const int scrollBarX = 128 - scrollBarWidth - 3;
    int textAreaW = scrollBarX - x - 2;

    // Actualizar índice visible para scrollTextTickerBounce

    for (int i = 0; i < visibleOptions && (startIndex + i) < totalOptions; i++) {
        int idx = startIndex + i;
        int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN);
        const char* label = options[idx];

        // -------- delimitador
        if (strcmp(label, "---------") == 0) {
            uiSprite.setFreeFont(&FreeSans9pt7b);
            uiSprite.setTextSize(1);
            uiSprite.setTextDatum(TL_DATUM);
            uiSprite.setTextColor(TFT_DARKGREY);
            uiSprite.drawString("---------", x, y);
            continue;
        }

        // ¿Está seleccionada por el usuario?
        bool isLogicallySelected =
            (idx == 0 && selectedVoiceGender == 0) ||
            (idx == 1 && selectedVoiceGender == 1) ||
            (idx == 3 && negativeResponse) ||
            (idx == 4 && !negativeResponse) ||
            (idx == 6 && selectedVolume == 0) ||
            (idx == 7 && selectedVolume == 1);

        // ¿Está siendo navegada con el cursor?
        bool isCursor = (idx == selection);

        // Color de fondo si está bajo el cursor
        if (isCursor) {
            uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, CARD_HEIGHT + 2, 3, CARD_COLOR);
        }

        // Color del texto
        uint16_t textColor = (isCursor && isLogicallySelected) ? HIGHLIGHT_COLOR :
                     isCursor ? TEXT_COLOR :
                     isLogicallySelected ? HIGHLIGHT_COLOR : TEXT_COLOR;

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextColor(textColor);

        String text = String(label);

        // Verifica si el texto sobrepasa el área visible para hacer scroll
        if (isCursor && uiSprite.textWidth(text) > textAreaW) {
            scrollTextTickerBounce(selection); // <-- llamada directa como pediste
        }

        // Recorte para evitar desbordes si no se va a scrollear
        while (!isCursor && uiSprite.textWidth(text) > textAreaW && text.length() > 0) {
            text.remove(text.length() - 1);
        }

        uiSprite.drawString(text, x, y);
    }

    // Barra de scroll
    int scrollBarY = 30;
    int scrollBarHeight = visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN;
    uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    if (totalOptions > visibleOptions) {
        float thumbRatio = (float)visibleOptions / (float)totalOptions;
        int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
        int thumbY = scrollBarY + (scrollBarHeight - thumbHeight)
                    * (float)(selection - startIndex) / (float)(totalOptions - visibleOptions);
        uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    } else {
        uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
    }

    uiSprite.pushSprite(0, 0);
}

void scrollTextTickerBounceSound(int selection)
{
    if (!soundMenuActive) return;

    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int visibleOptions = 4;
    const unsigned long frameInterval = 10;
    const int scrollMargin = 10; // Permite mostrar margen extra para el final del texto

    static int pixelOffsets[10] = {0}; 
    static int scrollDirections[10] = {1}; 
    static unsigned long lastFrameTime = 0;

    const char* options[] = {
        getTranslation("MUJER"),
        getTranslation("HOMBRE"),
        "---------",
        getTranslation("CON_NEG"),
        getTranslation("SIN_NEG"),
        "---------",
        getTranslation("VOL_NORMAL"),
        getTranslation("VOL_ATENUADO"),
        "---------",
        getTranslation("CONFIRMAR")
    };

    if (selection < 0 || selection >= 10) return;

    String text = String(options[selection]);


    TFT_eSprite measureSprite(&tft);
    measureSprite.setFreeFont(&FreeSans9pt7b);
    measureSprite.setTextWrap(false);
    measureSprite.setTextSize(1);
    measureSprite.setTextDatum(TL_DATUM);

    int fullTextWidth = measureSprite.textWidth(text.c_str());
    if (fullTextWidth <= cardWidth) return;

    int startIndex = max(0, min(selection - visibleOptions / 2, 10 - visibleOptions));
    int cardIndex = selection - startIndex;
    if (cardIndex < 0 || cardIndex >= visibleOptions) return;

    int cardY = 30 + cardIndex * (cardHeight + cardMargin);
    int textAreaX = 10;
    int textAreaY = cardY;
    int textAreaW = cardWidth;      // Ahora igual al ancho total de la tarjeta
    int textAreaH = cardHeight - 2;

    bool isLogicallySelected =
        (selection == 0 && selectedVoiceGender == 0) ||
        (selection == 1 && selectedVoiceGender == 1) ||
        (selection == 3 && negativeResponse) ||
        (selection == 4 && !negativeResponse) ||
        (selection == 6 && selectedVolume == 0) ||
        (selection == 7 && selectedVolume == 1);

    uint16_t textColor = isLogicallySelected ? HIGHLIGHT_COLOR : TEXT_COLOR;

    // Scroll automático
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
        pixelOffsets[selection] += scrollDirections[selection];

        if (pixelOffsets[selection] <= 0) {
            pixelOffsets[selection] = 0;
            scrollDirections[selection] = 1;
        }

        int maxOffset = fullTextWidth - cardWidth + scrollMargin;
        if (pixelOffsets[selection] >= maxOffset) {
            pixelOffsets[selection] = maxOffset;
            scrollDirections[selection] = -1;
        }

        lastFrameTime = now;
    }

    TFT_eSprite tickerSprite(&tft);
    if (tickerSprite.createSprite(textAreaW, textAreaH) == nullptr) return;

    tickerSprite.setFreeFont(&FreeSans9pt7b);
    tickerSprite.setTextWrap(false);
    tickerSprite.setTextSize(1);
    tickerSprite.setTextDatum(TL_DATUM);
    tickerSprite.setTextColor(textColor);
    tickerSprite.fillSprite(CARD_COLOR); // Fondo azul completo

    tickerSprite.drawString(text, -pixelOffsets[selection], 0);  // Mueve el texto dentro del sprite

    tickerSprite.pushSprite(textAreaX, textAreaY);
    tft.fillRect(textAreaX + textAreaW +5, textAreaY, 128 - (textAreaX + textAreaW), textAreaH, BACKGROUND_COLOR);
    tickerSprite.deleteSprite();
}

void drawFormatSubmenu(int selected) {
    tft.fillScreen(TFT_BLACK);
    String opciones[] = {"Eliminar elemento", "Restaurar", getTranslation("VOLVER")};
    for (int i = 0; i < 3; i++) {
        tft.setTextColor(i == selected ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
        tft.setCursor(10, 20 + i * 30);
        tft.print(opciones[i]);
    }
}

// void drawFormatMenu(int selection) {
//     const int scrollBarWidth = 6;
//     const int visibleOptions = 4;
//     const int totalOptions = 6;

//     const char* options[] = {
//         getTranslation("UPDATE_SALA"),
//         getTranslation("DELETE_ELEMENT"),
//         getTranslation("FORMATEAR"),
//         getTranslation("SHOW_ID"),
//         getTranslation("RESTORE_ELEM"),
//         getTranslation("VOLVER")
//     };

//     uiSprite.fillSprite(BACKGROUND_COLOR);

//     // Título
//     uiSprite.setFreeFont(&FreeSans12pt7b);
//     uiSprite.setTextColor(TEXT_COLOR);
//     uiSprite.setTextDatum(TC_DATUM);
//     uiSprite.setTextSize(1);
//     uiSprite.drawString(String(getTranslation("CONTROL_SALA_MENU")), 64, 5);

//     const int x = 9;
//     const int scrollBarX = 128 - scrollBarWidth - 3;
//     const int textAreaW = scrollBarX - x - 2;
//     int yOffset = 30;

//     // Cálculo del índice de inicio para scroll vertical
//     int startIndex = max(0, min(selection - visibleOptions / 2, totalOptions - visibleOptions));

//     for (int i = 0; i < visibleOptions && (startIndex + i) < totalOptions; i++) {
//         int idx = startIndex + i;
//         int y = yOffset + i * (CARD_HEIGHT + CARD_MARGIN);
//         bool isCursor = (idx == selection);

//         if (isCursor) {
//             uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, CARD_HEIGHT + 2, 3, CARD_COLOR);
//         }

//         uiSprite.setFreeFont(&FreeSans9pt7b);
//         uiSprite.setTextSize(1);
//         uiSprite.setTextDatum(TL_DATUM);
//         uiSprite.setTextColor(TEXT_COLOR);

//         String fullText = options[idx];
//         String displayText = fullText;

//         int textWidth = uiSprite.textWidth(fullText);
//         if (textWidth > textAreaW) {
//             displayText = "";
//             for (int c = 0; c < fullText.length(); c++) {
//                 String temp = displayText + fullText[c];
//                 if (uiSprite.textWidth(temp) > textAreaW) break;
//                 displayText += fullText[c];
//             }
//         }

//         uiSprite.drawString(displayText, x, y);
//     }

//     // Dibujar barra de scroll
//     int scrollBarY = yOffset;
//     int scrollBarHeight = visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN;
//     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

//     if (totalOptions > visibleOptions) {
//         float thumbRatio = (float)visibleOptions / totalOptions;
//         int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
//         int divisor = totalOptions - visibleOptions;
//         int thumbY = scrollBarY + (scrollBarHeight - thumbHeight) * ((divisor != 0) ? (float)(selection - startIndex) / divisor : 0.0f);
//         uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
//     } else {
//         uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
//     }

//     uiSprite.pushSprite(0, 0);
// }

void drawFormatMenu(int selection) {
    const int scrollBarWidth  = 6;
    const int visibleOptions  = 4;
    const int totalOptions    = 6;

    const char* options[] = {
        getTranslation("UPDATE_SALA"),
        getTranslation("DELETE_ELEMENT"),
        getTranslation("FORMATEAR"),
        getTranslation("SHOW_ID"),
        getTranslation("RESTORE_ELEM"),
        getTranslation("VOLVER")
    };

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(String(getTranslation("CONTROL_SALA_MENU")), 64, 5);

    const int x          = 9;
    const int scrollBarX = 128 - scrollBarWidth - 3;
    const int textAreaW  = scrollBarX - x - 2;
    const int yOffset    = 30;

    // Ventana rodante: desplaza startIndex uno en vez de centrar
    static int startIndex = 0;
    // Bajar: si el cursor llega a la última fila visible (índice visibleOptions-1)
    // y aún hay más opciones por debajo
    if ((selection - startIndex) > (visibleOptions - 2)
        && startIndex < (totalOptions - visibleOptions)) {
        startIndex++;
    }
    // Subir: si el cursor llega a la primera fila visible (índice 0)
    // y aún hay más opciones por encima
    else if ((selection - startIndex) < 1
             && startIndex > 0) {
        startIndex--;
    }

    // Dibujar cada opción
    for (int i = 0; i < visibleOptions && (startIndex + i) < totalOptions; i++) {
        int idx = startIndex + i;
        int y   = yOffset + i * (CARD_HEIGHT + CARD_MARGIN);
        bool isCursor = (idx == selection);

        if (isCursor) {
            uiSprite.fillRoundRect(
                x - 3,
                y - 1,
                textAreaW + 6,
                CARD_HEIGHT + 2,
                3,
                CARD_COLOR
            );
        }

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextColor(TEXT_COLOR);

        // Truncar si el texto es demasiado largo
        String fullText    = options[idx];
        String displayText = fullText;
        if (uiSprite.textWidth(fullText) > textAreaW) {
            displayText = "";
            for (int c = 0; c < fullText.length(); c++) {
                String tmp = displayText + fullText[c];
                if (uiSprite.textWidth(tmp) > textAreaW) break;
                displayText = tmp;
            }
        }

        uiSprite.drawString(displayText, x, y);
    }

    // // Barra de scroll vertical
    // int scrollBarY      = yOffset;
    // int scrollBarHeight = visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN;
    // uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    // if (totalOptions > visibleOptions) {
    //     float thumbRatio = (float)visibleOptions / totalOptions;
    //     int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
    //     int divisor     = totalOptions - visibleOptions;
    //     int thumbY      = scrollBarY
    //                       + (scrollBarHeight - thumbHeight)
    //                       * ((divisor != 0)
    //                          ? (float)(selection - startIndex) / divisor
    //                          : 0.0f);
    //     uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    // } else {
    //     uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
    // }

    uiSprite.pushSprite(0, 0);
}


bool forceDrawDeleteElementMenu = false;

void drawDeleteElementMenu(int selection) {
    const int visibleOptions = 4;
    const int scrollBarWidth = 5;

    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(String(getTranslation("DELETE_ELEMENT_MENU")), 64, 5);

    const int x = 9;
    const int scrollBarX = 128 - scrollBarWidth - 3;
    const int textAreaW = scrollBarX - x - 2;

    int totalOptions = deletableElementFiles.size();
    if (totalOptions == 0) {
        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextDatum(TC_DATUM);
        uiSprite.setTextColor(HIGHLIGHT_COLOR);
        uiSprite.drawString("No hay elementos", 64, 64);
        uiSprite.pushSprite(0, 0);
        return;
    }

    int startIndex = max(0, min(selection - visibleOptions / 2, totalOptions - visibleOptions));
    int yOffset = 30;

    for (int i = 0; i < visibleOptions && (startIndex + i) < totalOptions; i++) {
        int idx = startIndex + i;
        int y = yOffset + i * (CARD_HEIGHT + CARD_MARGIN);
        bool isCursor = (idx == selection);

        if (isCursor) {
            uiSprite.fillRoundRect(x - 3, y - 1, textAreaW + 6, CARD_HEIGHT + 2, 3, CARD_COLOR);
        }

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextSize(1);
        uiSprite.setTextColor(isCursor ? TEXT_COLOR : TEXT_COLOR);
        // Truncar el texto si es demasiado largo para el área visible
        String fullText = deletableElementFiles[idx];
        String displayText = fullText;

        int textWidth = uiSprite.textWidth(fullText);
        if (textWidth > textAreaW) {
            displayText = "";
            for (int c = 0; c < fullText.length(); c++) {
                String temp = displayText + fullText[c];
                if (uiSprite.textWidth(temp) > textAreaW) break;
                displayText += fullText[c];
            }
        }

        uiSprite.drawString(displayText, x, y);

    }

    // Barra de scroll
    int scrollBarY = yOffset;
    int scrollBarHeight = visibleOptions * (CARD_HEIGHT + CARD_MARGIN) - CARD_MARGIN;
    uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_DARKGREY);

    if (totalOptions > visibleOptions) {
        float thumbRatio = (float)visibleOptions / totalOptions;
        int thumbHeight = max(20, (int)(scrollBarHeight * thumbRatio));
        int thumbY = scrollBarY + (scrollBarHeight - thumbHeight)
                   * (float)(selection - startIndex) / (totalOptions - visibleOptions);
        uiSprite.fillRect(scrollBarX, thumbY, scrollBarWidth, thumbHeight, TFT_LIGHTGREY);
    } else {
        uiSprite.fillRect(scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight, TFT_LIGHTGREY);
    }

    uiSprite.pushSprite(0, 0);
}

void drawConfirmDelete(const String& fileName) {
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(HIGHLIGHT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString(getTranslation("CONFIRM_DELETE"), 64, 5);

    // Nombre del elemento
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.drawString(fileName, 64, 35);

    // Opciones
    const char* opciones[] = {
        getTranslation("YES_DELETE"),
        getTranslation("CANCEL")
    };

    const int x = 20;
    const int yBase = 64;
    const int boxW = 88;
    const int boxH = 22;
    const int spacing = 28;

    for (int i = 0; i < 2; i++) {
        int y = yBase + i * spacing;
        if (i == confirmSelection) {
            uiSprite.fillRoundRect(x - 4, y - 2, boxW + 8, boxH + 4, 4, CARD_COLOR);
        }
        uiSprite.setTextDatum(TL_DATUM);
        uiSprite.setTextColor(i == confirmSelection ? TEXT_COLOR : TEXT_COLOR);
        uiSprite.drawString(opciones[i], x, y);
    }

    uiSprite.pushSprite(0, 0);
}

void scrollTextTickerBounceFormat(int selection) {
    if (!formatSubMenuActive) return;

    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int visibleOptions = 4;
    const unsigned long frameInterval = 10;
    const int scrollMargin = 10;
    const int totalOptions = 6;


    static int pixelOffsets[6] = {0};
    static int scrollDirections[6] = {1};
    static unsigned long lastFrameTime = 0;

    const char* options[] = {
        getTranslation("UPDATE_SALA"),
        getTranslation("DELETE_ELEMENT"),
        getTranslation("FORMATEAR"),
        getTranslation("SHOW_ID"),
        getTranslation("RESTORE_ELEM"),
        getTranslation("VOLVER")
    };

    if (selection < 0 || selection >= 5) return;
    String text = String(options[selection]);

    TFT_eSprite measureSprite(&tft);
    measureSprite.setFreeFont(&FreeSans9pt7b);
    measureSprite.setTextWrap(false);
    measureSprite.setTextSize(1);
    measureSprite.setTextDatum(TL_DATUM);
    int fullTextWidth = measureSprite.textWidth(text.c_str());

    if (fullTextWidth <= cardWidth) return;

    int startIndex = max(0, min(selection - visibleOptions / 2, totalOptions - visibleOptions));
    int cardIndex = selection - startIndex;
    if (cardIndex < 0 || cardIndex >= visibleOptions) return;

    int cardY = 30 + cardIndex * (cardHeight + cardMargin);
    int textAreaX = 10;
    int textAreaY = cardY;
    int textAreaW = cardWidth;
    int textAreaH = cardHeight - 2;

    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
        pixelOffsets[selection] += scrollDirections[selection];

        if (pixelOffsets[selection] <= 0) {
            pixelOffsets[selection] = 0;
            scrollDirections[selection] = 1;
        }
        if (pixelOffsets[selection] >= fullTextWidth - cardWidth + scrollMargin) {
            pixelOffsets[selection] = fullTextWidth - cardWidth + scrollMargin;
            scrollDirections[selection] = -1;
        }
        lastFrameTime = now;
    }

    TFT_eSprite tickerSprite(&tft);
    if (tickerSprite.createSprite(textAreaW, textAreaH) == nullptr) return;

    tickerSprite.setFreeFont(&FreeSans9pt7b);
    tickerSprite.setTextWrap(false);
    tickerSprite.setTextSize(1);
    tickerSprite.setTextDatum(TL_DATUM);
    tickerSprite.setTextColor(TEXT_COLOR);
    tickerSprite.fillSprite(CARD_COLOR);

    tickerSprite.drawString(text, -pixelOffsets[selection], 0);
    tickerSprite.pushSprite(textAreaX, textAreaY);

    // Recorte derecho
    tft.fillRect(textAreaX + textAreaW +5, textAreaY, 128 - (textAreaX + textAreaW), textAreaH, BACKGROUND_COLOR);

    tickerSprite.deleteSprite();
}

void scrollTextTickerBounceDelete(int selection) {
    if (!deleteElementMenuActive) return;

    const int cardWidth = 110;
    const int cardHeight = CARD_HEIGHT;
    const int cardMargin = CARD_MARGIN;
    const int visibleOptions = 4;
    const unsigned long frameInterval = 10;
    const int scrollMargin = 10;

    static int pixelOffsets[30] = {0}; // Soporta hasta 30 elementos
    static int scrollDirections[30] = {1};
    static unsigned long lastFrameTime = 0;

    if (selection < 0 || selection >= deletableElementFiles.size()) return;
    String text = deletableElementFiles[selection];

    TFT_eSprite measureSprite(&tft);
    measureSprite.setFreeFont(&FreeSans9pt7b);
    measureSprite.setTextWrap(false);
    measureSprite.setTextSize(1);
    measureSprite.setTextDatum(TL_DATUM);
    int fullTextWidth = measureSprite.textWidth(text.c_str());
    if (fullTextWidth <= cardWidth) return;

    int startIndex = max(0, min(selection - visibleOptions / 2, (int)deletableElementFiles.size() - visibleOptions));
    int cardIndex = selection - startIndex;
    if (cardIndex < 0 || cardIndex >= visibleOptions) return;

    int cardY = 30 + cardIndex * (cardHeight + cardMargin);
    int textAreaX = 10;
    int textAreaY = cardY;
    int textAreaW = cardWidth;
    int textAreaH = cardHeight - 2;

    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
        pixelOffsets[selection] += scrollDirections[selection];

        if (pixelOffsets[selection] <= 0) {
            pixelOffsets[selection] = 0;
            scrollDirections[selection] = 1;
        }
        if (pixelOffsets[selection] >= fullTextWidth - cardWidth + scrollMargin) {
            pixelOffsets[selection] = fullTextWidth - cardWidth + scrollMargin;
            scrollDirections[selection] = -1;
        }
        lastFrameTime = now;
    }

    TFT_eSprite tickerSprite(&tft);
    if (tickerSprite.createSprite(textAreaW, textAreaH) == nullptr) return;

    tickerSprite.setFreeFont(&FreeSans9pt7b);
    tickerSprite.setTextWrap(false);
    tickerSprite.setTextSize(1);
    tickerSprite.setTextDatum(TL_DATUM);
    tickerSprite.setTextColor(TEXT_COLOR);
    tickerSprite.fillSprite(CARD_COLOR);

    tickerSprite.drawString(text, -pixelOffsets[selection], 0);
    tickerSprite.pushSprite(textAreaX, textAreaY);

    // Recorte derecho
    tft.fillRect(textAreaX + textAreaW +5, textAreaY, 128 - (textAreaX + textAreaW), textAreaH, BACKGROUND_COLOR);

    tickerSprite.deleteSprite();
}

void showCriticalBatteryMessage() {
    uiSprite.fillSprite(TFT_BLACK);
    uiSprite.setTextDatum(MC_DATUM);
    uiSprite.setTextColor(TFT_RED, TFT_BLACK);
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.drawString("Cargar bateria", 64, 10);
    uiSprite.pushImage(48, 40, 32, 61, vbat32x61_0_);
    uiSprite.pushSprite(0, 0);
    
}

const int vbatallArray_LEN = 5;
const uint16_t* vbatallArray[5] = {
	vbat30x15_horizontal_10_25_1,
	vbat30x15_horizontal_10_25_2,
	vbat30x15_horizontal_25_66,
	vbat30x15_horizontal_66_100,
	vbat32x61_0_
};
unsigned long lastBatteryToggleTime = 0;
bool batteryToggleState = false;

void drawBatteryIconMini(float percentage) {
    const int x = 128 - 30; // esquina superior derecha, adaptado al tamaño 30x15
    const int y = 2;

    const uint16_t* icon = nullptr;

    if (percentage < 25.0) {
        // Toggle entre 1 y 0 barra
        if (millis() - lastBatteryToggleTime > 500) {
            batteryToggleState = !batteryToggleState;
            lastBatteryToggleTime = millis();
        }
        icon = batteryToggleState ? vbatallArray[1] : vbatallArray[0];
    }
    else if (percentage < 66.0) {
        icon = vbatallArray[2];  // 2 barras
    }
    else {
        icon = vbatallArray[3];  // 3 barras
    }

    // Mostrar icono
    uiSprite.pushImage(x, y, 30, 15, icon);
}

void drawCognitiveMenu() {
    uiSprite.fillSprite(BACKGROUND_COLOR);
    uiSprite.setTextDatum(MC_DATUM);
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    uiSprite.drawString(getTranslation("ACTIVIDADES"), 64, 20); //
    uiSprite.drawString(getTranslation("COGNITIVAS"), 64, 40);
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.drawString(getTranslation("SALIR"), 64, 80);
    uiSprite.drawRoundRect(34, 70, 60, 20, 4, TFT_LIGHTGREY); // marco
    uiSprite.pushSprite(0, 0);
}

void drawConfirmRestoreMenu(int selection) {
    uiSprite.fillSprite(BACKGROUND_COLOR);
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.setTextColor(TEXT_COLOR);

    uiSprite.drawString("Restaurar sala?", 64, 10);

    const char* options[] = {"Si", "No"};
    for (int i = 0; i < 2; i++) {
        int y = 40 + i * 40;
        if (i == selection) {
            uiSprite.fillRoundRect(20, y - 5, 88, 30, 5, CARD_COLOR);
        }
        uiSprite.setTextDatum(TC_DATUM);
        uiSprite.drawString(options[i], 64, y);
    }

    uiSprite.pushSprite(0, 0);
}

void drawConfirmRestoreElementMenu(int selection) {
    uiSprite.fillSprite(BACKGROUND_COLOR);
    uiSprite.setFreeFont(&FreeSans9pt7b);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.setTextColor(TEXT_COLOR);

    uiSprite.drawString("Restaurar elementos?", 64, 10);

    const char* options[] = {"Si", "No"};
    for (int i = 0; i < 2; i++) {
        int y = 40 + i * 40;
        if (i == selection) {
            uiSprite.fillRoundRect(20, y - 5, 88, 30, 5, CARD_COLOR);
        }
        uiSprite.setTextDatum(TC_DATUM);
        uiSprite.drawString(options[i], 64, y);
    }

    uiSprite.pushSprite(0, 0);
}

// Muestra por pantalla el número de serie y el ID de un elemento durante un tiempo
// y luego vuelve al menú principal.
// Debes tener disponible la función mostrarTextoAjustado tal como la definiste antes.

void mostrarTextoAjustado(TFT_eSPI& tft,
                                 const char* texto,
                                 uint16_t xCentro,
                                 uint16_t yInicio,
                                 uint16_t maxWidth)
{
    uint8_t prevFont = tft.textfont;
    uint8_t prevDatum = tft.getTextDatum();

    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);

    const uint16_t lineHeight   = tft.fontHeight();
    const uint16_t screenHeight = 128;
    const uint16_t maxTextHeight = 55;

    tft.fillRect(5, yInicio - lineHeight/2, 118, maxTextHeight, TFT_BLACK);


    const char* ptr = texto;
    uint16_t y = yInicio;
    while (*ptr && y + lineHeight <= yInicio + maxTextHeight) {
        const char* scan = ptr;
        size_t lastSpace = 0, charsCount = 0;
        uint16_t w = 0;
        while (*scan) {
            ++charsCount;
            if (charsCount >= 128) break;
            char buf[128];
            memcpy(buf, ptr, charsCount);
            buf[charsCount] = '\0';
            w = tft.textWidth(buf);
            if (w > maxWidth) { --charsCount; break; }
            if (*scan == ' ') lastSpace = charsCount;
            ++scan;
        }
        size_t lineLen = (*scan == '\0') ? charsCount
                          : (lastSpace > 0 ? lastSpace : charsCount);
        char lineBuf[128];
        memcpy(lineBuf, ptr, lineLen);
        lineBuf[lineLen] = '\0';
        tft.drawString(lineBuf, xCentro, y);
        ptr += lineLen;
        while (*ptr == ' ') ++ptr;
        y += lineHeight;
    }

    tft.setTextFont(prevFont);
    tft.setTextDatum(prevDatum);
}


void showElemInfo(unsigned long delayTime,
                  const String& serialNumber,
                  const String& elementID) {
    // 1) Limpiar toda la pantalla
    tft.fillScreen(TFT_BLACK);

    // 2) Preparar cadenas a mostrar
    String idLine     = "ID: " + elementID;
    String serialLine = "Serial: " + serialNumber;

    // 3) Parámetros para centrar y ajustar el texto
    const uint16_t xCentro   = tft.width() / 2;  // centro horizontal
    const uint16_t maxWidth  = tft.width()  - 10; // margen de 5px a cada lado

    // 4) Mostrar ID (empezando en y = 30)
    mostrarTextoAjustado(tft,
                         idLine.c_str(),
                         xCentro,
                         30,         // yInicio para la primera línea
                         maxWidth);

    // 5) Mostrar Serial (empezando en y = 30 + altura de bloque)
    //    Como en mostrarTextoAjustado el bloque máximo de texto es 55px,
    //    desplazamos 30 + 55 + 10 de separación = 95px
    mostrarTextoAjustado(tft,
                         serialLine.c_str(),
                         xCentro,
                         60,         // yInicio para la segunda línea
                         maxWidth);

    // 6) Mantener la pantalla durante delayTime ms
    unsigned long start = millis();
    while (millis() - start < delayTime) {
        // Aquí podrías refrescar watchdog u otras tareas si fuera necesario
    }

    // 7) Volver al menú principal
    drawCurrentElement();
}


