#include <encoder_handler/encoder_handler.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <botonera_DMS/botonera_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include <ADXL345_handler/ADXL345_handler.h>
#include <microphone_DMS/microphone_DMS.h>

#define MODE_BACK -2
// Variables globales para el manejo del encoder
ESP32Encoder encoder;
int32_t lastEncoderValue = 0;
int currentIndex = 0;
unsigned long buttonPressStart = 0; // Marca el inicio de la pulsación larga
bool isLongPress = false;          // Bandera para la pulsación larga
bool inModesScreen = false;  
int currentModeIndex = 0;  // Índice del modo actual dentro del menú MODOS
int totalModes = 0;    
unsigned long buttonReleaseTime = 0;  // Track when button is released
bool modeScreenEnteredByLongPress = false;  // Flag to track how modes screen was entered
bool longPressDetected = false;
std::vector<String> elementFiles;
std::vector<bool> selectedStates;
int globalVisibleModesMap[17] = {0};  // Definición e inicialización




void encoder_init_func() {
    //Serial.println("Inicializando encoder...");
    pinMode(ENC_BUTTON, INPUT_PULLUP);
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachSingleEdge(ENC_B, ENC_A); //original encoder.attachSingleEdge(ENC_A, ENC_B);
    encoder.clearCount();
    encoder.setCount(0);
    encoder.setFilter(1023);
    //Serial.println("Encoder inicializado.");
}

// void handleEncoder() {
//     int32_t newEncoderValue = encoder.getCount();
//     if (newEncoderValue != lastEncoderValue) {
//         int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
//         lastEncoderValue = newEncoderValue;

//         // Navegar por elementos
//         if (!inModesScreen && elementFiles.size() > 1) {
//             Serial.println("Encoder girado, cambiando de elemento...");
//             currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();

//             // Sincronizar el modo del elemento actual
//             //requestAndSyncElementMode();

//             // Redibujar el elemento actual
//             drawCurrentElement();  
//         }
//         // Navegar por modos visibles
//         else if (inModesScreen && totalModes > 0) {
//             // currentModeIndex se maneja como índice 'visible'
//             currentModeIndex = (currentModeIndex + direction + totalModes) % totalModes;

//             // Obtener el índice real a partir del índice visible
//             int realModeIndex = globalVisibleModesMap[currentModeIndex];
//             if (realModeIndex >= 0) {
//                 // Actualizar patrón en la botonera
//                 String currentFile = elementFiles[currentIndex];
//                 colorHandler.setCurrentFile(currentFile);
//                 colorHandler.setPatternBotonera(realModeIndex, ledManager);
//             }
//             // Redibujar la pantalla de modos
//             drawModesScreen();
//         }
//     }

//     // Lectura del botón del encoder
//     if (digitalRead(ENC_BUTTON) == LOW) {
//         if (buttonPressStart == 0) {
//             // Registrar inicio de la pulsación
//             buttonPressStart = millis();
//         }
//         // Pulsación larga para mostrar menú de modos
//         else if (!hiddenMenuActive && millis() - buttonPressStart > 2000 && !isLongPress) {
//             isLongPress = true;
//             modeScreenEnteredByLongPress = true;

//             // --- SINCRONIZACIÓN ANTES DE ENTRAR EN LA PANTALLA DE MODOS ---
//             String currentFile = elementFiles[currentIndex];

//             // Si es "Apagar", no queremos abrir la pantalla de modos
//             if (currentFile == "Apagar") {
//                 return;
//             }

//             // 1) Obtener el modo real actualmente almacenado
//             int realModeIndex = 0;
//             if (currentFile == "Ambientes" || currentFile == "Fichas") {
//                 INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
//                 realModeIndex = option->currentMode;
//             } else {
//                 fs::File f = SPIFFS.open(currentFile, "r");
//                 if (f) {
//                     f.seek(OFFSET_CURRENTMODE, SeekSet);
//                     realModeIndex = f.read();
//                     f.close();
//                 }
//             }

//             // 2) Encontrar cuál es su índice visible (currentModeIndex) en la lista filtrada
//             int tempCurrentModeIndex = 0;
//             int foundVisibleIndex = -1;

//             if (currentFile == "Ambientes" || currentFile == "Fichas") {
//                 INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
//                 for (int i = 0; i < 16; i++) {
//                     if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
//                         if (i == realModeIndex) {
//                             foundVisibleIndex = tempCurrentModeIndex;
//                             break;
//                         }
//                         tempCurrentModeIndex++;
//                     }
//                 }
//             } else {
//                 fs::File f = SPIFFS.open(currentFile, "r");
//                 if (f) {
//                     for (int i = 0; i < 16; i++) {
//                         char modeName[25] = {0};
//                         char modeDesc[193] = {0};
//                         byte modeConfig[2] = {0};

//                         if (f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet)) {
//                             f.read((uint8_t*)modeName, 24);
//                             f.read((uint8_t*)modeDesc, 192);
//                             f.read(modeConfig, 2);

//                             if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
//                                 if (i == realModeIndex) {
//                                     foundVisibleIndex = tempCurrentModeIndex;
//                                     break;
//                                 }
//                                 tempCurrentModeIndex++;
//                             }
//                         }
//                     }
//                     f.close();
//                 }
//             }

//             // 3) Ajustar currentModeIndex para que la pantalla de modos lo muestre en verde
//             if (foundVisibleIndex >= 0) {
//                 currentModeIndex = foundVisibleIndex;
//             } else {
//                 currentModeIndex = 0;
//             }
//             // --- FIN DE LA SINCRONIZACIÓN ---

//             inModesScreen = true;
//             drawModesScreen();
//         }
//     }
//     // Cuando se suelta el botón
//     else {
//         if (!hiddenMenuActive && buttonPressStart > 0 && millis() - buttonPressStart < 2000) {
//             String currentFile = elementFiles[currentIndex];

//             // --- CASO ESPECIAL: Apagar como "botón" que hace broadcast BLACKOUT ---
//             if (currentFile == "Apagar") {
//                 // Deseleccionar todos los elementos
//                 for (size_t i = 0; i < selectedStates.size(); i++) {
//                     selectedStates[i] = false;
//                 }

//                 // Enviar broadcast BLACKOUT
//                 std::vector<byte> elementID;
//                 elementID.push_back(0xFF);
//                 send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));

//                 // Mostrar mensaje de apagado durante 5 segundos
//                 uiSprite.fillSprite(TFT_BLACK);
//                 uiSprite.setTextDatum(MC_DATUM);
//                 uiSprite.setTextSize(2);
//                 uiSprite.setTextColor(TFT_WHITE);
//                 uiSprite.drawString("APAGANDO", tft.width() / 2, tft.height() / 2 - 10);
//                 uiSprite.drawString("Sala...", tft.width() / 2, tft.height() / 2 + 10);
//                 uiSprite.pushSprite(0, 0);

//                 delay(5000);

//                 // Volver al menú principal
//                 currentIndex = 0;
//                 inModesScreen = false;
//                 drawCurrentElement();

//                 // Reset pulsación
//                 buttonPressStart = 0;
//                 isLongPress = false;
//                 return;
//             }

//             // --- 1) Si estamos en la pantalla de modos (inModesScreen = true) ---
//             if (inModesScreen) {
//                 handleModeSelection(currentFile);
//             }
//             // --- 2) Si NO estamos en la pantalla de modos ---
//             else {
//                 toggleElementSelection(currentFile);
//             }
//         }

//         // Resetear variables de pulsación
//         buttonPressStart = 0;
//         isLongPress = false;
//     }
// }
bool ignoreInputs = false;
bool ignoreEncoderClick = false;


void handleEncoder() {
        // Si se debe ignorar el click residual, chequea si ya se soltó el botón.
        if (ignoreEncoderClick) {
            if (digitalRead(ENC_BUTTON) == HIGH) {
                // El botón ya se soltó: se restablece la bandera.
                ignoreEncoderClick = false;
            } else {
                // Mientras el botón esté presionado, no se procesan entradas.
                return;
            }
        }
        
    if (ignoreInputs) return;
    int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue = newEncoderValue;

        // Navegar por elementos
        if (!inModesScreen && elementFiles.size() > 1) {
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();

            // Obtener el archivo actual
            String currentFile = elementFiles[currentIndex];

            // Obtener el modo actual almacenado
            int realModeIndex = 0;
            byte modeConfig[2] = {0};

            if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                realModeIndex = option->currentMode;
                memcpy(modeConfig, option->mode[realModeIndex].config, 2);
            } else {
               
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    realModeIndex = f.read();
                    f.seek(OFFSET_MODES + realModeIndex * SIZE_MODE + 216, SeekSet);
                    f.read(modeConfig, 2);
                    f.close();
                }
            }

            // Extraer los flags del modo actual del nuevo elemento
            ModeFlags flags = extractModeFlags(modeConfig);

            // Actualizar adxl y useMic
            adxl = flags.acceptsSensVal1;
            useMic = flags.acceptsSensVal2;
                                                                    #ifdef DEBUG
                                                                    Serial.println("🚀 Cambió de elemento, actualizando flags:");
                                                                    Serial.println("adxl status: " + String(adxl ? "true" : "false"));
                                                                    Serial.println("useMic status: " + String(useMic ? "true" : "false"));                                                                        
                                                                    #endif


            // Redibujar el elemento actual
            drawCurrentElement();  
        }
        // Navegar por modos visibles
        else if (inModesScreen && totalModes > 0) {
            // currentModeIndex se maneja como índice 'visible'
            currentModeIndex = (currentModeIndex + direction + totalModes) % totalModes;

            // Obtener el índice real a partir del índice visible
            int realModeIndex = globalVisibleModesMap[currentModeIndex];
            if (realModeIndex >= 0) {
                // Actualizar patrón en la botonera
                String currentFile = elementFiles[currentIndex];
                colorHandler.setCurrentFile(currentFile);
                colorHandler.setPatternBotonera(realModeIndex, ledManager);
            }
            // Redibujar la pantalla de modos
            drawModesScreen();
        }
    }

    // Lectura del botón del encoder
    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            // Registrar inicio de la pulsación
            buttonPressStart = millis();
        }
        // Pulsación larga para mostrar menú de modos
        else if (!hiddenMenuActive && millis() - buttonPressStart > 2000 && !isLongPress) {
            isLongPress = true;
            modeScreenEnteredByLongPress = true;

            // --- SINCRONIZACIÓN ANTES DE ENTRAR EN LA PANTALLA DE MODOS ---
            String currentFile = elementFiles[currentIndex];

            // Si es "Apagar", no queremos abrir la pantalla de modos
            if (currentFile == "Apagar") {
                return;
            }

            // 1) Obtener el modo real actualmente almacenado
            int realModeIndex = 0;
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                realModeIndex = option->currentMode;
            } else {
             
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    realModeIndex = f.read();
                    f.close();
                }
            }

            // 2) Encontrar cuál es su índice visible (currentModeIndex) en la lista filtrada
            int tempCurrentModeIndex = 0;
            int foundVisibleIndex = -1;

            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                for (int i = 0; i < 16; i++) {
                    if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                        if (i == realModeIndex) {
                            foundVisibleIndex = tempCurrentModeIndex;
                            break;
                        }
                        tempCurrentModeIndex++;
                    }
                }
            } else {
              
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    for (int i = 0; i < 16; i++) {
                        char modeName[25] = {0};
                        char modeDesc[193] = {0};
                        byte modeConfig[2] = {0};

                        if (f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet)) {
                            f.read((uint8_t*)modeName, 24);
                            f.read((uint8_t*)modeDesc, 192);
                            f.read(modeConfig, 2);

                            if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
                                if (i == realModeIndex) {
                                    foundVisibleIndex = tempCurrentModeIndex;
                                    break;
                                }
                                tempCurrentModeIndex++;
                            }
                        }
                    }
                    f.close();
                }
            }

            // 3) Ajustar currentModeIndex para que la pantalla de modos lo muestre en verde
            if (foundVisibleIndex >= 0) {
                currentModeIndex = foundVisibleIndex;
            } else {
                currentModeIndex = 0;
            }
            // --- FIN DE LA SINCRONIZACIÓN ---

            inModesScreen = true;
            drawModesScreen();
        }
    }
    // Cuando se suelta el botón
    else {
        if (!hiddenMenuActive && buttonPressStart > 0 && millis() - buttonPressStart < 2000) {
            String currentFile = elementFiles[currentIndex];

            // --- CASO ESPECIAL: Apagar como "botón" que hace broadcast BLACKOUT ---
            if (currentFile == "Apagar") {
                for (size_t i = 0; i < selectedStates.size(); i++) {
                    selectedStates[i] = false;
                }
                std::vector<byte> elementID;
                elementID.push_back(0xFF);
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));
                setAllElementsToBasicMode();

                showMessageWithLoading("Apagando Sala...", 5000);

                currentIndex = 0;
                inModesScreen = false;
                drawCurrentElement();
                buttonPressStart = 0;
                isLongPress = false;
                return;
            }

            if (inModesScreen) {
                handleModeSelection(currentFile);
            } else {
                toggleElementSelection(currentFile);
            }
        }

        buttonPressStart = 0;
        isLongPress = false;
    }
}



void requestAndSyncElementMode() {
    String currentFile = elementFiles[currentIndex];

    // Obtener la ID del elemento actual
    byte elementID = BROADCAST;  // Valor predeterminado
    if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        elementID = option->ID;  // ID para elementos fijos
    } else {
        // Leer la ID desde SPIFFS
       
        fs::File f = SPIFFS.open(currentFile, "r");
        if (f) {
            f.seek(OFFSET_ID, SeekSet);
            f.read(&elementID, 1);
            f.close();
        }
            // Enviar trama de petición de modo
    //send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, elementID, SPANISH_LANG, ELEM_CMODE_SECTOR));

    // Esperar respuesta del sector ELEM_CMODE_SECTOR
    // if (!element->esperar_respuesta(100)) {
    //     Serial.printf("No llegó respuesta del sector ELEM_CMODE_SECTOR para el elemento con ID %d\n", elementID);
    //     return;
    // }
    }



    // Si llega la respuesta, el modo será procesado en RX_main_handler
                                                                                                    #ifdef DEBUG
                                                                                                    Serial.printf("Respuesta del sector ELEM_CMODE_SECTOR recibida para el elemento con ID %d\n", elementID);    
                                                                                                    #endif
    
}

void handleModeSelection(const String& currentFile) {
    // Si se selecciona la opción de regresar (icono de flecha), salimos inmediatamente
    if (globalVisibleModesMap[currentModeIndex] == -2) {
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
  
    // Variables necesarias para procesar el modo seleccionado
    std::vector<byte> elementID;
    char elementName[25] = {0};
    String modeName;
    uint8_t modeConfig[2] = {0};

    // Obtener el índice real del modo a partir del índice visible
    int realModeIndex = globalVisibleModesMap[currentModeIndex];
    int modeNumber = currentModeIndex + 1; // Número del modo en la lista visible

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        // Procesar elementos dinámicos
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        option->currentMode = realModeIndex;
        // Se asume que en la estructura ya está el nombre completo del modo
        modeName = String((char*)option->mode[realModeIndex].name);
        elementID.push_back(0); // ID predeterminada para dinámicos
        // Si en elementos dinámicos también se requiere la configuración,
        // se debe asignar modeConfig con los datos correspondientes.
    } else {
        // Procesar elementos cargados desde SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            // Guardar el nuevo modo seleccionado
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write((uint8_t*)&realModeIndex, 1);

            // Leer el nombre del elemento
            f.seek(OFFSET_NAME, SeekSet);
            f.read((uint8_t*)elementName, 24);

            // Leer la ID del elemento
            byte id = 0;
            f.seek(OFFSET_ID, SeekSet);
            f.read(&id, 1);
            elementID.push_back(id);

            // Leer el registro del modo desde SPIFFS
            // Cada registro de modo ocupa SIZE_MODE bytes (218 bytes en este caso)
            // y comienza en OFFSET_MODES + (SIZE_MODE * realModeIndex)
            int modoOffset = OFFSET_MODES + (SIZE_MODE * realModeIndex);

            // Primero, leer el nombre del modo (los primeros 24 bytes)
            f.seek(modoOffset, SeekSet);
            char modeNameBuf[25] = {0};
            f.read((uint8_t*)modeNameBuf, 24);
            modeName = String(modeNameBuf);

            // Saltar la descripción (192 bytes) y posicionarse al comienzo de la configuración
            f.seek(modoOffset + 24 + 192, SeekSet);

            // Leer los 2 bytes de configuración
            f.read(modeConfig, 2);

            f.close();
        }
    }

    // Extraer los flags usando la función previamente adaptada
    ModeFlags flags = extractModeFlags(modeConfig);
#ifdef DEBUG
    Serial.println("📋 Mode Name: " + String(modeName));  
#endif

    // Ajustar las variables globales según los flags
    adxl = flags.acceptsSensVal1;
    useMic = flags.acceptsSensVal2;
    
#ifdef DEBUG
    Serial.println("adxl status " + String(adxl ? "true" : "false"));
    Serial.println("useMic status " + String(useMic ? "true" : "false"));    
#endif

    // **Nueva lógica**: Si el elemento no está seleccionado, seleccionarlo automáticamente
    if (!selectedStates[currentIndex]) {
        selectedStates[currentIndex] = true;
#ifdef DEBUG
        Serial.println("✅ Elemento seleccionado automáticamente.");
#endif
    }

    // Mostrar en Serial otros datos
#ifdef DEBUG
    Serial.printf("Nombre del elemento: %s\n", elementName[0] ? elementName : currentFile.c_str());
    Serial.printf("ID del elemento: %d\n", elementID[0]);
    Serial.printf("Modo seleccionado: %s (Modo %d)\n", modeName.c_str(), modeNumber);    
#endif

    // Enviar la trama con el modo seleccionado
    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, START_TEST));
    delay(300);
    send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, elementID, realModeIndex));

    // Salir de la pantalla de modos y redibujar el elemento actual
    inModesScreen = false;
    drawCurrentElement();
}

void toggleElementSelection(const String& currentFile) {
    // Alternar el estado de selección del elemento actual
    selectedStates[currentIndex] = !selectedStates[currentIndex];

    // Crear el vector para almacenar la ID del elemento
    std::vector<byte> elementID;
    // Determinar si el elemento proviene de SPIFFS
    bool isElementFromSPIFFS = !currentFile.startsWith("Ambientes") &&
                               !currentFile.startsWith("Fichas") &&
                               !currentFile.startsWith("Apagar");

    if (isElementFromSPIFFS) {
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            byte id = 0;
            f.seek(OFFSET_ID, SeekSet);
            f.read(&id, 1);
            elementID.push_back(id);
            f.close();
        } else {
            Serial.println("Error al leer la ID del archivo.");
            elementID.push_back(0); // Valor por defecto en caso de error
        }
    } else {
        // Si es "Ambientes", "Fichas" o "Apagar", no hacemos nada con SPIFFS
        elementID.push_back(0);
    }

    // Solo si es un elemento de SPIFFS se envía el comando y se actualiza el modo
    if (isElementFromSPIFFS) {
        // Determinar el comando a enviar
        byte command = selectedStates[currentIndex] ? START_TEST : BLACKOUT;
        Serial.printf("Enviando comando %s a la ID %d\n",
                      command == START_TEST ? "START_TEST" : "BLACKOUT",
                      elementID[0]);
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, command));

        if (command == BLACKOUT) {
            showMessageWithLoading("Apagando Elemento", 3000);
        }

        // Actualizar el modo del elemento a "básico"
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            byte basicMode = DEFAULT_BASIC_MODE;  // modo 1
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write(&basicMode, 1);
            f.close();
            Serial.printf("Modo actualizado a básico (1) en SPIFFS para el elemento %s\n", currentFile.c_str());
        } else {
            Serial.println("Error al abrir el archivo para actualizar el modo.");
        }
    }
    
    // En el caso de "Ambientes", "Fichas" o "Apagar", únicamente se alterna el estado de selección

    // Redibujar el elemento actual para reflejar la selección/deselección
    drawCurrentElement();
}

void handleHiddenMenuNavigation(int &hiddenMenuSelection) {
    int32_t newEncoderValue = encoder.getCount();
    static bool encoderButtonPressed = false;
    static bool initialEntry = true;
    static bool menuJustOpened = true;  // Nueva variable para controlar la confirmación automática

    // Al entrar al menú oculto por primera vez, resalta la primera opción sin confirmarla
    if (initialEntry) {
        hiddenMenuSelection = 0;  // Preselección visual sin confirmar
        drawHiddenMenu(hiddenMenuSelection);
        initialEntry = false;
        menuJustOpened = true;  // Bloquea la confirmación inmediata
    }

    // Navegación por el menú con el encoder
    if (newEncoderValue != lastEncoderValue) {
        hiddenMenuSelection += (newEncoderValue > lastEncoderValue) ? 1 : -1;
        hiddenMenuSelection = constrain(hiddenMenuSelection, 0, 5); // Ahora hay 6 opciones (índices 0-5)
        lastEncoderValue = newEncoderValue;
        drawHiddenMenu(hiddenMenuSelection);
    }

    // Confirmación con el botón del encoder
    if (digitalRead(ENC_BUTTON) == HIGH) {
        menuJustOpened = false;  // Solo ahora permite confirmaciones
        encoderButtonPressed = false;
    }

    if (digitalRead(ENC_BUTTON) == LOW && !encoderButtonPressed && !menuJustOpened) {
    encoderButtonPressed = true;
    ignoreEncoderClick = true;
    byte respuesta = 0;
    switch (hiddenMenuSelection) {
        case 0: {// Añadir elemento
            element->validar_elemento();
            hiddenMenuActive = false;
            break;
        }
        case 1: // Cambiar idioma
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Cambiando idioma...");
                                                                                            #endif
            // Lógica para cambiar idioma
            formatSPIFFS();
            loadElementsFromSPIFFS();
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 2: // Sonido
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Ajustando sonido...");
                                                                                            #endif
            // Lógica para ajustar sonido
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 3: // Brillo
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Ajustando brillo...");
                                                                                            #endif
            // Lógica para ajustar brillo
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 4: // Respuestas
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Configurando respuestas...");
                                                                                            #endif
            // Lógica para configurar respuestas
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 5: // Volver
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Volviendo al menú principal");
                                                                                            #endif
            
            PulsadoresHandler::limpiarEstados();
            drawCurrentElement();
            initialEntry = false;
            hiddenMenuActive = false;
            break;
        default:
            break;
    }
}

}

ModeFlags extractModeFlags(const uint8_t modeConfig[2]) {
    ModeFlags flags = {
        // Ahora se asume que modeConfig[0] es el byte alto (bits 15 a 8)
        .modeExist             = (modeConfig[0] >> 7) & 1,  // Bit 15
        .nop2                  = (modeConfig[0] >> 6) & 1,  // Bit 14
        .nop1                  = (modeConfig[0] >> 5) & 1,  // Bit 13
        .acceptsPatterns       = (modeConfig[0] >> 4) & 1,  // Bit 12
        .acceptsBankFile       = (modeConfig[0] >> 3) & 1,  // Bit 11
        .canAnswer             = (modeConfig[0] >> 2) & 1,  // Bit 10
        .hasPassive            = (modeConfig[0] >> 1) & 1,  // Bit 9
        .situatedHigh          = (modeConfig[0] >> 0) & 1,  // Bit 8

        // modeConfig[1] es el byte bajo (bits 7 a 0)
        .acceptsSensVal2       = (modeConfig[1] >> 7) & 1,  // Bit 7
        .acceptsSensVal1       = (modeConfig[1] >> 6) & 1,  // Bit 6
        .hasRelay4             = (modeConfig[1] >> 5) & 1,  // Bit 5
        .hasRelay3             = (modeConfig[1] >> 4) & 1,  // Bit 4
        .hasRelay2             = (modeConfig[1] >> 3) & 1,  // Bit 3
        .hasRelay1             = (modeConfig[1] >> 2) & 1,  // Bit 2
        .acceptsAdvancedColor  = (modeConfig[1] >> 1) & 1,  // Bit 1
        .acceptsBasicColor     = (modeConfig[1] >> 0) & 1   // Bit 0
    };
    return flags;
}



