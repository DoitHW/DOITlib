#include <display_handler/display_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include "rom/rtc.h"    // Opcional, si quieres info de reset
#include <icons_64x64_DMS/icons_64x64_DMS.h>

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
         String modeDisplay;
         if (visibleModeIndex >= 0 && currentAlternateStates.size() > (size_t)visibleModeIndex) {
             modeDisplay = getModeDisplayName(String((char*)option->mode[option->currentMode].name), currentAlternateStates[visibleModeIndex]);
         } else {
             modeDisplay = String((char*)option->mode[option->currentMode].name);
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
         drawSelectionCircle(selectedStates[currentIndex], startX, startY);
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
         drawElementName((char*)option->name, false);
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
         Serial.println("DEBUG: Tamaño de elemento '" + displayName + "' = " + String(elementTextWidth));
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
             Serial.println("DEBUG: [SPIFFS] currentAlternateStates.size() = " + String(currentAlternateStates.size()));
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
         drawSelectionCircle(selectedStates[currentIndex], startX, startY);
         currentMode = realModeIndex;
    }
    currentModeIndex = currentMode;
    colorHandler.setCurrentFile(currentFile);
    colorHandler.setPatternBotonera(currentModeIndex, ledManager);
    drawNavigationArrows();
    
    // Si el sistema está bloqueado, dibujar el icono de candado en la esquina superior izquierda.
    // Se asume que 'device_lock' es la imagen almacenada en RAM y sus dimensiones son 16x16 (ajustar si es necesario).
    if (!inModesScreen && systemLocked) {
         uiSprite.pushImage(0, 0, 16, 16, device_lock);
    }
    
    uiSprite.pushSprite(0, 0);
    lastDisplayInteraction = millis();
    displayOn = true;
}

void display_sleep() {
    tft.fillScreen(TFT_BLACK);
    displayOn = false;
    Serial.println("Pantalla apagada por inactividad.");
  }
  
void display_wakeup() {
    displayOn = true;
    // Vuelve a dibujar el menú principal para “despertar” la interfaz
    drawCurrentElement();
    lastDisplayInteraction = millis();
    Serial.println("Pantalla reactivada por interacción.");
  }


//  void display_sleep() {
//     // Asumimos que la pantalla ya muestra el fondo actual (por drawCurrentElement())
//     int cx = tft.width() / 2;
//     int cy = tft.height() / 2;
//     int maxRadius = sqrt(cx * cx + cy * cy);
//     // Animación iris-out: el "hueco" (la zona visible) se reduce hasta desaparecer.
//     for (int r = maxRadius; r >= 0; r -= 5) {
//         // Para cada fila, se dibuja negro en las zonas que quedan fuera del círculo de radio r
//         for (int y = 0; y < tft.height(); y++) {
//             int dy = abs(y - cy);
//             if (dy > r) {
//                 // Toda la fila queda fuera del círculo: se pinta toda de negro.
//                 tft.drawFastHLine(0, y, tft.width(), TFT_BLACK);
//             } else {
//                 // Dentro de la fila, calcular la distancia horizontal máxima para quedar dentro del círculo.
//                 int dx = sqrt((float)r * r - (float)dy * dy);
//                 int left = cx - dx;
//                 int right = cx + dx;
//                 // Pinta a la izquierda y a la derecha de la zona visible.
//                 if (left > 0) tft.drawFastHLine(0, y, left, TFT_BLACK);
//                 if (right < tft.width()) tft.drawFastHLine(right, y, tft.width() - right, TFT_BLACK);
//             }
//         }
//         delay(10); // Retardo para suavizar la animación.
//     }
//     displayOn = false;
//     Serial.println("Pantalla apagada por inactividad.");
// }

// void display_wakeup() {
//     displayOn = true;
    
//     int cx = tft.width() / 2;
//     int cy = tft.height() / 2;
//     int maxRadius = sqrt(cx * cx + cy * cy);
    
//     // Dibujar primero el fondo completo
//     drawCurrentElement();
//     lastDisplayInteraction = millis();
    
//     // Comenzamos con la pantalla negra
//     tft.fillScreen(TFT_BLACK);
    
//     // Animación iris-in: se "abre" el iris mostrando el fondo en el centro que crece hasta revelar toda la imagen
//     for (int r = 0; r <= maxRadius; r += 5) {
//         // Redraw the background to prevent flickering
//         drawCurrentElement();
        
//         // Pintar de negro todo lo que esté fuera del círculo
//         for (int y = 0; y < tft.height(); y++) {
//             int dy = abs(y - cy);
//             if (dy > r) {
//                 // Línea completa negra si está fuera del radio
//                 tft.drawFastHLine(0, y, tft.width(), TFT_BLACK);
//             } else {
//                 // Calcular ancho del círculo en este punto
//                 int dx = sqrt((float)r * r - (float)dy * dy);
//                 int left = cx - dx;
//                 int right = cx + dx;
                
//                 // Pintar de negro las zonas fuera del círculo
//                 if (left > 0) 
//                     tft.drawFastHLine(0, y, left, TFT_BLACK);
//                 if (right < tft.width()) 
//                     tft.drawFastHLine(right, y, tft.width() - right, TFT_BLACK);
//             }
//         }
        
//         delay(10);
//     }
    
//     lastDisplayInteraction = millis();
//     Serial.println("Pantalla reactivada por interacción.");
// }

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

    // Limpiar el sprite completo.
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Título.
    uiSprite.setFreeFont(&FreeSans12pt7b);
    uiSprite.setTextColor(TEXT_COLOR);
    uiSprite.setTextDatum(TC_DATUM);
    uiSprite.setTextSize(1);
    uiSprite.drawString("MODOS", 64, 5);

    String currentFile = elementFiles[currentIndex];
    
    // Construir el mapa auxiliar de modos visibles.
    // La primera opción siempre será "Encender/Apagar" (valor especial -3).
    int visibleModesMap[18]; // Espacio para: "Encender/Apagar" + hasta 16 modos + "Regresar".
    visibleModesMap[0] = -3; // -3 indica "Encender/Apagar".
    int count = 1;
    
    // Para elementos fijos ("Ambientes" o "Fichas").
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                visibleModesMap[count] = i;
                count++;
            }
        }
    }
    // Para elementos almacenados en SPIFFS (excepto "Apagar").
    else if (currentFile != "Apagar") {
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
                    visibleModesMap[count] = i;
                    count++;
                }
            }
            f.close();
        }
    }
    // Agregar la opción "Regresar" al final.
    visibleModesMap[count] = -2; // -2 indica "Regresar".
    count++;
    totalModes = count;

    // Validar currentModeIndex.
    if (currentModeIndex < 0 || currentModeIndex >= totalModes) {
        currentModeIndex = 0;
    }
    
    // Copiar el mapa visible a la variable global.
    memcpy(globalVisibleModesMap, visibleModesMap, sizeof(int) * totalModes);
    
    // Calcular el índice de inicio para centrar la opción seleccionada.
    int startIndex = max(0, min(currentModeIndex - visibleOptions / 2, totalModes - visibleOptions));

    // Parámetros de layout.
    const int x = 10;
    const int textAreaW = 110;
    const int scrollBarX = 120;

    // Dibujar cada opción visible.
    for (int i = 0; i < visibleOptions && (startIndex + i) < totalModes; i++) {
        int currentVisibleIndex = startIndex + i;
        int y = 30 + i * (CARD_HEIGHT + CARD_MARGIN);
        bool isSelected = (currentVisibleIndex == currentModeIndex);
        uint32_t textColor = isSelected ? HIGHLIGHT_COLOR : TEXT_COLOR;

        uiSprite.setFreeFont(&FreeSans9pt7b);
        uiSprite.setTextColor(textColor);
        uiSprite.setTextSize(1);
        uiSprite.setTextDatum(TL_DATUM);

        int modeVal = visibleModesMap[currentVisibleIndex];
        if (modeVal == -2) {
            // Dibujar la opción "Regresar" como un icono de flecha izquierda.
            int arrowWidth = 12;
            int arrowHeight = 12;
            int arrowX = x + (textAreaW - arrowWidth) / 2;
            int arrowY = y + (CARD_HEIGHT - arrowHeight) / 2;
            uiSprite.fillTriangle(arrowX + arrowWidth, arrowY, arrowX, arrowY + arrowHeight / 2, arrowX + arrowWidth, arrowY + arrowHeight, textColor);
        }
        else if (modeVal == -3) {
            // Dibujar la opción "Encender/Apagar".
            String label = selectedStates[currentIndex] ? "Apagar" : "Encender";
            if (isSelected) {
                if (currentModeIndex != lastSelectedMode) {
                    modeTickerOffset = 0;
                    modeTickerDirection = 1;
                    lastSelectedMode = currentModeIndex;
                }
                uiSprite.setFreeFont(&FreeSans9pt7b);
                uiSprite.setTextSize(1);
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
                    TFT_eSprite modeSprite = TFT_eSprite(&tft);
                    modeSprite.createSprite(textAreaW, CARD_HEIGHT);
                    modeSprite.fillSprite(BACKGROUND_COLOR);
                    modeSprite.setFreeFont(&FreeSans9pt7b);
                    modeSprite.setTextSize(1);
                    modeSprite.setTextDatum(TL_DATUM);
                    modeSprite.setTextColor(textColor, BACKGROUND_COLOR);
                    modeSprite.drawString(label, -modeTickerOffset, 0);
                    modeSprite.pushToSprite(&uiSprite, x, y);
                    modeSprite.deleteSprite();
                } else {
                    uiSprite.drawString(label, x, y);
                }
            } else {
                uiSprite.drawString(label, x, y);
                uiSprite.fillRect(0, y, x, CARD_HEIGHT, BACKGROUND_COLOR);
                uiSprite.fillRect(120, y, 128 - 120, CARD_HEIGHT, BACKGROUND_COLOR);
            }
        }
        else {
            // Dibujar una opción normal de modo.
            char modeName[25] = {0};
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                strncpy(modeName, (char*)option->mode[modeVal].name, 24);
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_MODES + modeVal * SIZE_MODE, SeekSet);
                    f.read((uint8_t*)modeName, 24);
                    f.close();
                }
            }
            String modeStr = String(modeName);
            // Para opciones normales, se accede al estado alternativo usando (índice visible - 1).
            bool modeAlternateState = false;
            if (currentAlternateStates.size() > (size_t)(currentVisibleIndex - 1) && visibleModesMap[currentVisibleIndex] != -2) {
                modeAlternateState = currentAlternateStates[currentVisibleIndex - 1];
            }
            modeStr = getModeDisplayName(modeStr, modeAlternateState);
            if (isSelected) {
                if (currentModeIndex != lastSelectedMode) {
                    modeTickerOffset = 0;
                    modeTickerDirection = 1;
                    lastSelectedMode = currentModeIndex;
                }
                uiSprite.setFreeFont(&FreeSans9pt7b);
                uiSprite.setTextSize(1);
                int fullTextWidth = uiSprite.textWidth(modeStr);
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
                    TFT_eSprite modeSprite = TFT_eSprite(&tft);
                    modeSprite.createSprite(textAreaW, CARD_HEIGHT);
                    modeSprite.fillSprite(BACKGROUND_COLOR);
                    modeSprite.setFreeFont(&FreeSans9pt7b);
                    modeSprite.setTextSize(1);
                    modeSprite.setTextDatum(TL_DATUM);
                    modeSprite.setTextColor(textColor, BACKGROUND_COLOR);
                    modeSprite.drawString(modeStr, -modeTickerOffset, 0);
                    modeSprite.pushToSprite(&uiSprite, x, y);
                    modeSprite.deleteSprite();
                } else {
                    uiSprite.drawString(modeStr, x, y);
                }
            } else {
                uiSprite.drawString(modeStr, x, y);
                uiSprite.fillRect(0, y, x, CARD_HEIGHT, BACKGROUND_COLOR);
                uiSprite.fillRect(120, y, 128 - 120, CARD_HEIGHT, BACKGROUND_COLOR);
            }
        }
    }
    
    // Dibujar la barra de desplazamiento vertical.
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
    
    uiSprite.pushSprite(0, 0);
    
    // Actualizar scroll vertical suave.
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
