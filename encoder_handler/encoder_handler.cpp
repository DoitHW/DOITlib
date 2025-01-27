#include <encoder_handler/encoder_handler.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <botonera_DMS/botonera_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>


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
int globalVisibleModesMap[16] = {0};  // Definición e inicialización


void encoder_init_func() {
    //Serial.println("Inicializando encoder...");
    pinMode(ENC_BUTTON, INPUT_PULLUP);
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachSingleEdge(ENC_A, ENC_B);
    encoder.clearCount();
    encoder.setCount(0);
    encoder.setFilter(1023);
    //Serial.println("Encoder inicializado.");
}

void handleEncoder() {
    int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue = newEncoderValue;

        // Navegar por elementos
        if (!inModesScreen && elementFiles.size() > 1) {
            Serial.println("Encoder girado, cambiando de elemento...");
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();

            // Redibujar el elemento actual y actualizar patrón
            drawCurrentElement();  // Llama a setPatternBotonera() con el modo actualizado
        }
        // Navegar por modos visibles
        else if (inModesScreen && totalModes > 0) {
            // currentModeIndex se maneja como índice 'visible'
            currentModeIndex = (currentModeIndex + direction + totalModes) % totalModes;

            // Obtener el índice real a partir del índice visible
            int realModeIndex = globalVisibleModesMap[currentModeIndex];
            if (realModeIndex >= 0) {
                // Actualizar patrón en la botonera
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

            // 1) Obtener el modo real actualmente almacenado
            int realModeIndex = 0; // Por defecto
            if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
                INFO_PACK_T* option = nullptr;
                if (currentFile == "Ambientes") {
                    option = &ambientesOption;
                } else if (currentFile == "Fichas") {
                    option = &fichasOption;
                } else { // "Apagar"
                    option = &apagarSala;
                }
                realModeIndex = option->currentMode; 
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    realModeIndex = f.read(); // Lee un byte (modo real)
                    f.close();
                }
            }

            // 2) Encontrar cuál es su índice visible (currentModeIndex) en la lista filtrada
            int tempCurrentModeIndex = 0;
            int foundVisibleIndex = -1;
            
            if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
                INFO_PACK_T* option = nullptr;
                if (currentFile == "Ambientes") {
                    option = &ambientesOption;
                } else if (currentFile == "Fichas") {
                    option = &fichasOption;
                } else { // "Apagar"
                    option = &apagarSala;
                }

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
                // Si no se encontró, arrancamos la lista en 0
                currentModeIndex = 0;
            }
            // --- FIN DE LA SINCRONIZACIÓN ---

            inModesScreen = true;
            drawModesScreen(); // Muestra la pantalla de modos con el modo sincronizado
        }
    } 
    // Cuando se suelta el botón
    else {
        if (!hiddenMenuActive && buttonPressStart > 0 && millis() - buttonPressStart < 2000) {
            if (inModesScreen) {
                // Seleccionar modo actual
                String currentFile = elementFiles[currentIndex];
                std::vector<byte> elementID;
                char elementName[25] = {0};
                String modeName;

                // Obtener el índice real a partir del índice visible (currentModeIndex)
                int realModeIndex = globalVisibleModesMap[currentModeIndex];
                int modeNumber = currentModeIndex + 1; // Número del modo en la lista visible

                if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
                    INFO_PACK_T* option = nullptr;
                    if (currentFile == "Ambientes") {
                        option = &ambientesOption;
                    } else if (currentFile == "Fichas") {
                        option = &fichasOption;
                    } else { // "Apagar"
                        option = &apagarSala;
                    }
                    option->currentMode = realModeIndex; 
                    modeName = String((char*)option->mode[realModeIndex].name);
                    elementID.push_back(0); // ID predeterminada
                } else {
                    // Leer y actualizar el archivo SPIFFS
                    fs::File f = SPIFFS.open(currentFile, "r+");
                    if (f) {
                        // Guardar el nuevo modo seleccionado
                        f.seek(OFFSET_CURRENTMODE, SeekSet);
                        f.write((uint8_t*)&realModeIndex, 1);

                        // Leer nombre del elemento
                        f.seek(OFFSET_NAME, SeekSet);
                        f.read((uint8_t*)elementName, 24);

                        // Leer la ID del elemento
                        byte id = 0;
                        f.seek(OFFSET_ID, SeekSet);
                        f.read(&id, 1);
                        elementID.push_back(id);

                        // Leer el nombre del modo
                        f.seek(OFFSET_MODES + (SIZE_MODE * realModeIndex), SeekSet);
                        char modeNameBuf[25] = {0};
                        f.read((uint8_t*)modeNameBuf, 24);
                        modeName = String(modeNameBuf);

                        f.close();
                    }
                }

                // Mostrar en Serial
                Serial.printf("Nombre del elemento: %s\n", elementName[0] ? elementName : currentFile.c_str());
                Serial.printf("ID del elemento: %d\n", elementID[0]);
                Serial.printf("Modo seleccionado: %s (Modo %d)\n", modeName.c_str(), modeNumber);

                send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, elementID, realModeIndex));
                inModesScreen = false;
                drawCurrentElement(); // Redibuja el elemento actual con el nuevo modo
            } else {
                // Cambiar selección del elemento actual
                selectedStates[currentIndex] = !selectedStates[currentIndex];

                // Obtener la ID del elemento como std::vector<byte>
                std::vector<byte> elementID;
                String currentFile = elementFiles[currentIndex];

                // Si NO es Ambientes, Fichas ni Apagar, se asume que es un elemento de SPIFFS
                if (!currentFile.startsWith("Ambientes")
                    && !currentFile.startsWith("Fichas")
                    && !currentFile.startsWith("Apagar"))
                {
                    fs::File f = SPIFFS.open(currentFile, "r");
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
                    // Para los elementos fijos (Ambientes, Fichas, Apagar) usamos ID=0
                    elementID.push_back(0);
                }

                // Enviar el estado del color basado en la selección
                if (selectedStates[currentIndex]) {
                    Serial.printf("Enviando color blanco a la ID %d\n", elementID[0]);
                    //send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, elementID, 0x00));
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, START_TEST));
                } else {
                    Serial.printf("Enviando color negro a la ID %d\n", elementID[0]);
                    //send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, elementID, 0x08));
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));
                }

                drawCurrentElement(); // Redibuja el elemento actual
            }
        }

        // Resetear variables de pulsaciones
        buttonPressStart = 0;
        isLongPress = false;
    }
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
    byte respuesta = 0;
    switch (hiddenMenuSelection) {
        case 0: {// Añadir elemento
            element->validar_elemento();
            hiddenMenuActive = false;
            break;
        }
        case 1: // Cambiar idioma
            Serial.println("Cambiando idioma...");
            // Lógica para cambiar idioma
            formatSPIFFS();
            hiddenMenuActive = false;
            break;
        case 2: // Sonido
            Serial.println("Ajustando sonido...");
            // Lógica para ajustar sonido
            hiddenMenuActive = false;
            break;
        case 3: // Brillo
            Serial.println("Ajustando brillo...");
            // Lógica para ajustar brillo
            hiddenMenuActive = false;
            break;
        case 4: // Respuestas
            Serial.println("Configurando respuestas...");
            // Lógica para configurar respuestas
            hiddenMenuActive = false;
            break;
        case 5: // Volver
            Serial.println("Volviendo al menú principal");
            hiddenMenuActive = false;
            initialEntry = true;
            PulsadoresHandler::limpiarEstados();
            drawCurrentElement();
            break;
        default:
            break;
    }
}

}


