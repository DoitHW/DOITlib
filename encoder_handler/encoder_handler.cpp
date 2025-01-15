#include <encoder_handler/encoder_handler.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <botonera_DMS/botonera_DMS.h>


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

        if (!inModesScreen && elementFiles.size() > 1) {
            Serial.println("Encoder girado, cambiando de elemento...");
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();

            // Actualizar el elemento actual y el modo actual
            drawCurrentElement();  // Llama a `setPatternBotonera` con el modo actualizado
        } else if (inModesScreen && totalModes > 0) {
            // Navegar entre modos en la pantalla de modos
            currentModeIndex = (currentModeIndex + direction + totalModes) % totalModes;
            colorHandler.setPatternBotonera(currentModeIndex); // Actualizar patrones con el modo actual
            drawModesScreen();
        }
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis(); // Registrar inicio
        } else if (!hiddenMenuActive && millis() - buttonPressStart > 2000 && !isLongPress) {
            isLongPress = true;
            inModesScreen = true;
            modeScreenEnteredByLongPress = true;

            drawModesScreen(); // Muestra la pantalla de modos
        }
    } else {
        if (!hiddenMenuActive && buttonPressStart > 0 && millis() - buttonPressStart < 2000) {
            if (inModesScreen) {
                // Seleccionar modo actual
                String currentFile = elementFiles[currentIndex];
                std::vector<byte> elementID;
                char elementName[25] = {0};
                String modeName;
                int modeNumber = currentModeIndex + 1; // Número del modo

                if (currentFile == "Ambientes" || currentFile == "Fichas") {
                    INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                    option->currentMode = currentModeIndex;
                    modeName = String((char*)option->mode[currentModeIndex].name);
                    elementID.push_back(0); // ID predeterminada
                } else {
                    // Leer y actualizar el archivo SPIFFS
                    fs::File f = SPIFFS.open(currentFile, "r+");
                    if (f) {
                        // Guardar el nuevo modo seleccionado en el archivo
                        f.seek(OFFSET_CURRENTMODE, SeekSet);
                        f.write((uint8_t*)&currentModeIndex, 1);

                        // Leer el nombre del elemento
                        f.seek(OFFSET_NAME, SeekSet);
                        f.read((uint8_t*)elementName, 24);

                        // Leer la ID del elemento
                        byte id = 0;
                        f.seek(OFFSET_ID, SeekSet);
                        f.read(&id, 1);
                        elementID.push_back(id);

                        // Leer el nombre del modo actual
                        f.seek(OFFSET_MODES + (SIZE_MODE * currentModeIndex), SeekSet);
                        char modeNameBuf[25] = {0};
                        f.read((uint8_t*)modeNameBuf, 24);
                        modeName = String(modeNameBuf);

                        f.close();
                    }
                }

                // Mostrar información por Serial
                Serial.printf("Nombre del elemento: %s\n", elementName[0] ? elementName : currentFile.c_str());
                Serial.printf("ID del elemento: %d\n", elementID[0]);
                Serial.printf("Modo seleccionado: %s (Modo %d)\n", modeName.c_str(), modeNumber);
                send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, elementID, currentModeIndex + 1));
                inModesScreen = false;
                drawCurrentElement(); // Redibuja el elemento actual con el nuevo modo
            } else {
                // Cambiar selección del elemento actual
                selectedStates[currentIndex] = !selectedStates[currentIndex];

                // Obtener la ID del elemento como std::vector<byte>
                std::vector<byte> elementID;
                String currentFile = elementFiles[currentIndex];

                if (!currentFile.startsWith("Ambientes") && !currentFile.startsWith("Fichas")) {
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
                    elementID.push_back(0); // Valor por defecto para opciones dinámicas
                }

                // Enviar el estado del color basado en la selección
                if (selectedStates[currentIndex]) {
                    Serial.printf("Enviando color blanco a la ID %d\n", elementID[0]);
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, elementID, 0x00)); // Enviar color blanco
                } else {
                    Serial.printf("Enviando color negro a la ID %d\n", elementID[0]);
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, elementID, 0x08)); // Enviar color negro
                }

                drawCurrentElement(); // Redibuja el elemento actual
            }
        }

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
        hiddenMenuSelection = constrain(hiddenMenuSelection, 0, 1);
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

        if (hiddenMenuSelection == 0) {
            Serial.println("Opción: Cambiar idioma (sin implementar aún)");
            byte respuesta = element->buscar_elemento_nuevo();
            element->mostrarMensajeTemporal(respuesta, 3000);
        } 
        else if (hiddenMenuSelection == 1) {
            Serial.println("Volviendo al menú principal");
            hiddenMenuActive = false;
            initialEntry = true;  // Restablecer para la próxima vez
            PulsadoresHandler::limpiarEstados();
            drawCurrentElement();
        }
    }
}



