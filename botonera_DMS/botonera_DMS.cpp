#include <botonera_DMS/botonera_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <encoder_handler/encoder_handler.h>
#include <display_handler/display_handler.h>

#define MIN_VALID_ELEMENT_SIZE (OFFSET_ID + 1) // Se espera que el archivo tenga al menos OFFSET_ID+1 bytes


extern std::vector<uint8_t> printTargetID;
byte currentCognitiveCommand = COG_ACT_OFF;

BOTONERA_::BOTONERA_() : ELEMENT_() {
            set_type(TYPE_BOTONERA);
            

        }

void BOTONERA_::botonera_begin(){}

void BOTONERA_::printFrameInfo(LAST_ENTRY_FRAME_T LEF) {
    Serial.println("\n==== 📨 Trama Recibida 📨 ====");

    // Determinar origen
    String origenStr;
    if (LEF.origin == 0xDB) origenStr = "BOTONERA";
    else if (LEF.origin == 0xDC) origenStr = "CONSOLA";
    else if (LEF.origin == 0xFF) origenStr = "BROADCAST";
    else if (LEF.origin == 0xDF) origenStr = "HACKING BOX";
    else origenStr = "DESCONOCIDO";

    Serial.printf("📌 Origen: %s (0x%02X)\n", origenStr.c_str(), LEF.origin);

    Serial.print("🎯 Targets: ");
    if (printTargetID.empty()) {
        Serial.println("Ninguno (posible broadcast)");
    } else {
        for (size_t i = 0; i < printTargetID.size(); i++) {
            Serial.printf("0x%02X ", printTargetID[i]);
        }
        Serial.println();
    }

    // Determinar función
    String functionStr;
    switch (LEF.function) {
        case 0xA0: functionStr = "F_REQ_ELEM_SECTOR"; break;
        case 0xA1: functionStr = "F_REQ_ELEM_INFO"; break;
        case 0xA2: functionStr = "F_REQ_ELEM_STATE"; break;
        case 0xA3: functionStr = "F_REQ_ELEM_ICON"; break;
        case 0xB1: functionStr = "F_SET_ELEM_ID"; break;
        case 0xB2: functionStr = "F_SET_ELEM_MODE"; break;
        case 0xB3: functionStr = "F_SET_ELEM_DEAF"; break;
        case 0xC1: functionStr = "F_SEND_COLOR"; break;
        case 0xC2: functionStr = "F_SEND_RGB"; break;
        case 0xC3: functionStr = "F_SEND_BRIGHTNESS"; break;
        case 0xCA: functionStr = "F_SEND_SENSOR_VALUE"; break;
        case 0xCB: functionStr = "F_SEND_SENSOR_VALUE_2"; break;
        case 0xCC: functionStr = "F_SEND_FILE_NUM"; break;
        case 0xCD: functionStr = "F_SEND_PATTERN_NUM"; break;
        case 0xCE: functionStr = "F_SEND_FLAG_BYTE"; break;
        case 0xCF: functionStr = "F_SEND_COMMAND"; break;
        case 0xD0: functionStr = "F_RETURN_ELEM_SECTOR"; break;
        case 0xD1: functionStr = "F_RETURN_ELEM_INFO"; break;
        case 0xD2: functionStr = "F_RETURN_ELEM_STATE"; break;
        case 0xD3: functionStr = "F_RETURN_ELEM_ICON"; break;
        case 0xDE: functionStr = "F_RETURN_ELEM_ERROR"; break;
        default: functionStr = "FUNCIÓN DESCONOCIDA";
    }

    Serial.printf("🛠️  Función: %s (0x%02X)\n", functionStr.c_str(), LEF.function);

    // Interpretación de datos
    Serial.print("📦 Data: ");
    if (LEF.data.empty()) {
        Serial.println("No hay datos para esta función.");
    } else {
        if (LEF.function == 0xCA || LEF.function == 0xCB) { // Sensores
            int minVal = (LEF.data[0] << 8) | LEF.data[1];
            int maxVal = (LEF.data[2] << 8) | LEF.data[3];
            int sensedVal = (LEF.data[4] << 8) | LEF.data[5];

            Serial.printf("MIN = %d, MAX= %d, VAL= %d\n", minVal, maxVal, sensedVal);
        } 
        else if (LEF.function == 0xC1) { // Color recibido
            String colorName;
            switch (LEF.data[0]) {
                case 0: colorName = "BLANCO"; break;
                case 1: colorName = "AMARILLO"; break;
                case 2: colorName = "NARANJA"; break;
                case 3: colorName = "ROJO"; break;
                case 4: colorName = "VIOLETA"; break;
                case 5: colorName = "AZUL"; break;
                case 6: colorName = "CELESTE"; break;
                case 7: colorName = "VERDE"; break;
                case 8: colorName = "NEGRO"; break;
                default: colorName = "COLOR DESCONOCIDO";
            }
            Serial.printf("%s (0x%02X)\n", colorName.c_str(), LEF.data[0]);
        } 
        else if (LEF.function == 0xCE) { // Cambio en el relé
            Serial.println("Cambio de estado en el relé.");
        } 
        else if (LEF.function == 0xA0) { // Petición de sector
            String idioma = (LEF.data[0] == 1) ? "ES" : "OTRO";
            Serial.printf("Idioma: %s, Sector: %d\n", idioma.c_str(), LEF.data[1]);
        } 
        else if (LEF.function == 0xCC) { // Petición de archivo
            Serial.printf("Carpeta (Bank): %d, Archivo (File): %d\n", LEF.data[0], LEF.data[1]);
        } 
        else {
            // Imprimir todos los datos si no hay interpretación específica
            for (size_t i = 0; i < LEF.data.size(); i++) {
                Serial.printf("0x%02X ", LEF.data[i]);
            }
            Serial.println();
        }
    }

    Serial.println("=============================");
}

void BOTONERA_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }
    printFrameInfo(LEF);
    // Depuración del estado de la pila
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                               // Serial.println("Stack restante: " + String(stackSize));
                                                            #endif

    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

        case F_RETURN_ELEM_SECTOR: {
        Serial.println("Ha llegado un F_RETURN_ELEM_SECTOR");
        element->sectorIn_handler(LEF.data, LEF.origin);

            break;
        }

        break;

        case F_SET_ELEM_MODE:{
          
            break;
        }
        case F_SEND_COLOR: {
            
            
            break;
        }
        case F_SEND_FILE_NUM: {
            Serial.println("Recibido un play sound");

            doitPlayer.play_file(LEF.data[0],LEF.data[1]);
            
            break;
        }

        case F_SEND_COMMAND: {
            byte receivedCommand = LEF.data[0];
            currentCognitiveCommand = receivedCommand;
            Serial.println("Comando recibido: " + String(receivedCommand, HEX));
            
            if (receivedCommand == COG_ACT_ON) {
                Serial.println("Activando modo cognitivo...");
                activateCognitiveMode();
            } else if (receivedCommand == COG_ACT_OFF) {
                Serial.println("Desactivando modo cognitivo...");
                deactivateCognitiveMode();
            } else if (receivedCommand == WIN_CMD)
            {
                byte res= rand() % 4;
                doitPlayer.play_file(WIN_RESP_BANK, 11 + res);
            } else if (receivedCommand == FAIL_CMD)
            {
                byte res= rand() % 4;
                doitPlayer.play_file(FAIL_RESP_BANK, 11 + res);
            }
            break;
        }

        default: {
                                                                #ifdef DEBUG
                                                                    Serial.println("Se ha recibido una función desconocida.");
                                                                #endif
            break;
        }
    }

    // Depuración al final de la función
    stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                                #ifdef DEBUG
                                                                   // Serial.println("Stack restante al final: " + String(stackSize));
                                                                #endif
}

extern bool adxl;
extern bool useMic;
void BOTONERA_::sectorIn_handler(std::vector<byte> data, byte targetin) {
    byte sector = data[0];
    
    switch (sector)
    {
    case ELEM_NAME_SECTOR:{
    
        //Aquí copiar data a partir de data[1] a INFO_PACK_T
        break;
    }
    case ELEM_DESC_SECTOR:  {
        //Aquí copiar data a partir de data[1] a INFO_PACK_T
        break;
    }
     case ELEM_CMODE_SECTOR:{
        
        
                    // Procesar modo actual recibido
            byte receivedMode = data[1];
            Serial.println("📢 ID: "+String(targetin) + " con MODO " +String(receivedMode));

            // Leer el modo almacenado en SPIFFS
     
            fs::File file = SPIFFS.open(getCurrentFilePath(targetin), "r+");
            if (!file) {
                Serial.println("Error: No se pudo abrir el archivo en SPIFFS.");
                break;
            }

            // Obtener el modo actual almacenado
            file.seek(OFFSET_CURRENTMODE, SeekSet);
            byte storedMode;
            file.read(&storedMode, 1);

            // Comparar y actualizar si es necesario
            if (storedMode != receivedMode) {
                Serial.printf("Actualizando el modo en SPIFFS: %d -> %d\n", storedMode, receivedMode);
                file.seek(OFFSET_CURRENTMODE, SeekSet);
                file.write(&receivedMode, 1);
            } else {
                Serial.println("El modo recibido coincide con el almacenado en SPIFFS.");
            }

            file.close();

            // Redibujar la pantalla para reflejar los cambios
            drawCurrentElement();

            String currentFile = elementFiles[currentIndex];
            fs::File f = SPIFFS.open(currentFile, "r");
            if (f) {
                f.seek(OFFSET_ID, SeekSet);
                byte currentElementID;
                f.read(&currentElementID, 1);
                f.close();

                if (currentElementID == targetin) {
                    // --- Releer configuración del nuevo modo ---
                    byte currentMode = 0;
                    byte modeConfig[2] = {0};

                    fs::File f2 = SPIFFS.open(currentFile, "r");
                    if (f2) {
                        f2.seek(OFFSET_CURRENTMODE, SeekSet);
                        f2.read(&currentMode, 1);
                        f2.seek(OFFSET_MODES + (currentMode * SIZE_MODE) + 216, SeekSet); // OFFSET_CONFIG dentro del modo
                        f2.read(modeConfig, 2);
                        f2.close();
                    }

                    // Actualizar sensores según los flags
                    adxl = getModeFlag(modeConfig, HAS_SENS_VAL_1);
                    useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

                    // Actualizar patrón de color
                    colorHandler.setCurrentFile(currentFile);
                    colorHandler.setPatternBotonera(currentMode, ledManager);

                    Serial.println("🔁 Configuración de modo actual actualizada tras recibir ELEM_CMODE_SECTOR.");
                }
            }
        break;
    }
    case ELEM_ICON_ROW_63_SECTOR:{

    break;
    }
    default:
    
        break;
    }
}

String BOTONERA_::getCurrentFilePath(byte elementID) {
    for (const String& fileName : elementFiles) {

        fs::File file = SPIFFS.open(fileName, "r");
        if (!file) continue;

        file.seek(OFFSET_ID, SeekSet);
        byte id;
        file.read(&id, 1);
        file.close();

        if (id == elementID) {
            return fileName;  // Devolver el archivo que coincide con la ID
        }
    }

    Serial.printf("Error: No se encontró un archivo para el elemento ID %d.\n", elementID);
    return String();  // Retornar cadena vacía si no se encuentra
}


byte tempID;
byte BOTONERA_::validar_serial() {
    const int max_reintentos = 5;
    
    iniciarEscaneoElemento("Buscando elementos...");
    for (int intento = 0; intento < max_reintentos; intento++) {
        // Mostrar algo en la interfaz de usuario, p.ej. "Escaneando..."
        
        frameReceived = false;
        // Petición de ELEM_SERIAL_SECTOR al DEFAULT_DEVICE
        send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                              DEFAULT_DEVICE,
                                              SPANISH_LANG,
                                              ELEM_SERIAL_SECTOR));

        // unsigned long startTime = millis();

        // // Espera hasta 2.5s a que frameReceived se ponga en true
        // while (!frameReceived && (millis() - startTime < 2500)) {
        //     delay(10);
        // }
        if (!esperar_respuesta(2500)) {
            Serial.println("No llegó respuesta de ELEM_SERIAL_SECTOR");
        }

        // Si hubo respuesta (frameReceived = true), procesamos
        if (frameReceived) {
            frameReceived = false; // Reiniciamos el flag
            LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

            // Verificamos que la trama sea del sector correcto y tenga tamaño
            if (LEF.data.size() >= 3 && LEF.data[0] == ELEM_SERIAL_SECTOR) {
                // Guardamos el serial recibido
                memcpy(lastSerial, &LEF.data[1], 5);

                // Decidimos si es un elemento existente o nuevo
                if (serialExistsInSPIFFS(lastSerial)) {
                    // Elemento existente
                    return 1;
                } else {
                    // Elemento nuevo
                    return 2;
                }
            }
        }

        // Si llegamos aquí, este intento falló
        Serial.printf("⚠️ Intento %d/%d fallido\n", intento+1, max_reintentos);
        delay(500);
    }

    // Si terminamos el bucle, no hubo respuesta válida
    return 0;
}

void BOTONERA_::procesar_datos_sector(LAST_ENTRY_FRAME_T &LEF, int sector, INFO_PACK_T* infoPack) {
    switch (sector) {
        case ELEM_NAME_SECTOR:
            memcpy(infoPack->name, &LEF.data[1], 
                   min(sizeof(infoPack->name), LEF.data.size() - 1));
            break;

        case ELEM_DESC_SECTOR:
            memcpy(infoPack->desc, &LEF.data[1], 
                   min(sizeof(infoPack->desc), LEF.data.size() - 1));
            break;
        
        case ELEM_LOCATION_SECTOR:
            infoPack->situacion = LEF.data[1];
            break;

        case ELEM_SERIAL_SECTOR:
            memcpy(infoPack->serialNum, &LEF.data[1], 
                   min(sizeof(infoPack->serialNum), LEF.data.size() - 1));
            break;

        case ELEM_ID_SECTOR:
            infoPack->ID = LEF.data[1];
            break;

        case ELEM_CMODE_SECTOR:
            infoPack->currentMode = LEF.data[1];
            break;

        default:
            // Procesar modos
            if (sector >= ELEM_MODE_0_NAME_SECTOR && sector <= ELEM_MODE_15_FLAG_SECTOR) {
                int modeIndex  = (sector - ELEM_MODE_0_NAME_SECTOR) / 3;
                int fieldIndex = (sector - ELEM_MODE_0_NAME_SECTOR) % 3;

                if (fieldIndex == 0) {
                    memcpy(infoPack->mode[modeIndex].name, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].name), LEF.data.size()-1));
                }
                else if (fieldIndex == 1) {
                    memcpy(infoPack->mode[modeIndex].desc, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].desc), LEF.data.size()-1));
                }
                else {
                    memcpy(infoPack->mode[modeIndex].config, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].config), LEF.data.size()-1));
                }
            }
            // Procesar iconos
            else if (sector >= ELEM_ICON_ROW_0_SECTOR && sector <= ELEM_ICON_ROW_63_SECTOR) {
                int rowIndex = sector - ELEM_ICON_ROW_0_SECTOR;
                if (LEF.data.size() == 129) {
                    for (int col = 0; col < ICON_COLUMNS; ++col) {
                        uint8_t msb = LEF.data[2 * col + 1];
                        uint8_t lsb = LEF.data[2 * col + 2];
                        infoPack->icono[rowIndex][col] = (uint16_t(msb) << 8) | lsb;
                    }
                } else {
                    Serial.printf("❌ Fila de icono incompleta: Sector %d\n", sector);
                }
            }
            break;
    }
}

void BOTONERA_::actualizar_elemento_existente() {

    fs::File root = SPIFFS.open("/");
    fs::File file = root.openNextFile();

    while(file) {
        byte existingSerial[5];
        file.seek(OFFSET_SERIAL);
        file.read(existingSerial, 5);

        // Si coincide con el "último serial" leído
        if(memcmp(existingSerial, lastSerial, 5) == 0) {
            // Actualizamos la ID en el archivo
            file.seek(OFFSET_ID);
            file.write(&lastAssignedID, 1);
            file.close();
            break;
        }
        file = root.openNextFile();
    }
    root.close();

    // Recargamos la lista en RAM
    loadElementsFromSPIFFS();
}

bool BOTONERA_::guardar_elemento(INFO_PACK_T* infoPack) {
    // Generamos un nombre de archivo único
    String uniqueFileName = generateUniqueFileName((char*)infoPack->name);
 
    fs::File file = SPIFFS.open(uniqueFileName, "w");
    if (!file) return false;

    bool success = true;
    success &= writeBytesChecked(file, infoPack->name,      sizeof(infoPack->name));
    success &= writeBytesChecked(file, infoPack->desc,      sizeof(infoPack->desc));
    success &= writeBytesChecked(file, infoPack->serialNum, sizeof(infoPack->serialNum));
    success &= writeBytesChecked(file, &infoPack->ID,       1);
    success &= writeBytesChecked(file, &infoPack->currentMode, 1);

    // Guardamos los 16 modos
    for (int i = 0; i < 16 && success; ++i) {
        success &= writeBytesChecked(file, infoPack->mode[i].name,   sizeof(infoPack->mode[i].name));
        success &= writeBytesChecked(file, infoPack->mode[i].desc,   sizeof(infoPack->mode[i].desc));
        success &= writeBytesChecked(file, infoPack->mode[i].config, sizeof(infoPack->mode[i].config));
    }

    // Guardamos icono (64 filas, N columnas * 2 bytes)
    for (int y = 0; y < ICON_ROWS && success; ++y) {
        success &= writeBytesChecked(file, (uint8_t*)infoPack->icono[y], ICON_COLUMNS * 2);
    }

    // Después del icono
    success &= writeBytesChecked(file, &infoPack->situacion, 1);


    file.close();

    if (success) {
        // Recargamos la lista en memoria
        loadElementsFromSPIFFS();
        return true;
    }

    // Si falló algo, borramos el archivo incompleto
    SPIFFS.remove(uniqueFileName);
    return false;
}

void BOTONERA_::reasignar_id_elemento(INFO_PACK_T* infoPack) {
    byte newID = getNextAvailableID();
    byte currentID = 0;

    if(infoPack) {
        // Caso 2: se nos ha pasado la info de un elemento nuevo
        if(infoPack->ID == 0xDD) {
            infoPack->ID = newID;
            Serial.printf("🆔 Nueva ID asignada: %02X\n", newID);
        }
    } else {
        // Caso 1: reasignación en un elemento ya existente, sin un infoPack
    
        fs::File root = SPIFFS.open("/");
        fs::File file = root.openNextFile();

        while(file) {
            byte existingSerial[5];
            file.seek(OFFSET_SERIAL);
            file.read(existingSerial, 5);

            if(memcmp(existingSerial, element->lastSerial, 5) == 0) {
                file.seek(OFFSET_ID);
                file.read(&currentID, 1);

                if(currentID == 0xDD) {
                    file.seek(OFFSET_ID);
                    file.write(&newID, 1);
                    Serial.printf("🆔 ID actualizada: %02X\n", newID);
                }
                file.close();
                break;
            }
            file = root.openNextFile();
        }
        root.close();
    }

    Serial.println("currentID: " + String(currentID));
    Serial.println("newID: "     + String(newID));

    // Se envía el frame para fijar la nueva ID
    send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, newID));
    delay(100);

    Serial.println("🆙🆙🆙🆙 ID reasignada");
}

// void BOTONERA_::validar_elemento() {
//     // Llama a validar_serial(), que retorna:
//     //   0 -> No se recibió respuesta (error)
//     //   1 -> Elemento existente en SPIFFS
//     //   2 -> Elemento no existente (nuevo)
//     byte resultado = validar_serial();
//     switch (resultado) {
//         case 0: {
//             // Error al leer el serial
//             Serial.println("❌ No se obtuvo respuesta");
//             mostrarMensajeTemporal(0, 3000);
//             byte existingID = getIdFromSPIFFS(lastSerial);
//             send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, existingID, 0xDD));
//             return;
//         }

//         case 1: {
//             // El elemento ya existe. 
//             // 1) Recuperar ID desde SPIFFS
//             byte existingID = getIdFromSPIFFS(lastSerial);
//             if (existingID == 0xFF) {
//                 // Algo falló al buscar la ID
//                 Serial.println("❌ No se encontró la ID en SPIFFS, aunque el serial existe.");
//                 mostrarMensajeTemporal(0, 3000);
//                 return;
//             }

//             // 2) Reasignar la ID al elemento que responde en la ID por defecto
//             lastAssignedID = existingID; 
//             send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA,
//                                               DEFAULT_DEVICE,
//                                               lastAssignedID));
//             delay(2000);
//             // 3) Verificar confirmación de ID (petición de ELEM_ID_SECTOR a la new ID)
//             if (confirmarCambioID(lastAssignedID)) {
//                 // Éxito: ID confirmada
//                 Serial.printf("✅ ID reasignada y confirmada: 0x%02X\n", lastAssignedID);
//                 // Mostramos mensaje "elemento existente" => 1
//                 mostrarMensajeTemporal(1, 3000);
//             } else {
//                 // Falló la confirmación 
//                 Serial.println("❌ Falló la confirmación de la ID reasignada.");
//                 mostrarMensajeTemporal(0, 3000);
//             }

//             return;
//         }

//         case 2: {
//             // Elemento nuevo. 
//             // 1) Obtenemos la próxima ID disponible
//             lastAssignedID = getNextAvailableID();

//             // 2) Asignamos esa ID al elemento que responde actualmente en DEFAULT_DEVICE
//             Serial.println("Cambiando de ID en caso 2");
//             send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA,
//                                               DEFAULT_DEVICE,
//                                               lastAssignedID));
//             delay(2000);
//             // 3) Podemos verificar si deseamos confirmarlo también:
//             if (!confirmarCambioID(lastAssignedID)) {
//                 Serial.println("❌ Falló la confirmación de la nueva ID en elemento nuevo.");
//                 mostrarMensajeTemporal(0, 3000);
//                 finalizarEscaneoElemento();
//                 return;
//             }
//             Serial.printf("✅ Nueva ID asignada y confirmada: 0x%02X\n", lastAssignedID);
//             iniciarEscaneoElemento("Agregando");
//             actualizarBarraProgreso(0);
//             // 4) Descargamos la información completa del elemento (nombre, desc, modos...)
//             delay(500);
//             bool exito = procesar_y_guardar_elemento_nuevo(lastAssignedID);

//             // 5) Resultado final: si todo fue bien => mostrarMensajeTemporal(2, 3000) 
//             //                      si no => mensaje de error
//             if (exito) {
//                 mostrarMensajeTemporal(2, 3000);
//             } else {
//                 mostrarMensajeTemporal(0, 3000);
//             }
//             finalizarEscaneoElemento();
//             return;
//         }
//     }
// }

byte BOTONERA_::getIdFromSPIFFS(byte *serial) {
    if (!SPIFFS.begin(true)) {
        return 0xFF;
    }
    
    fs::File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
        return 0xFF;
    }
    
    root.rewindDirectory();
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Solo considerar archivos válidos
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            // Verificar que el archivo tenga tamaño suficiente para leer el serial
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                file.seek(OFFSET_SERIAL);
                byte existingSerial[5];
                int bytesRead = file.read(existingSerial, 5);
                
                if (bytesRead == 5) {
                    if (memcmp(existingSerial, serial, 5) == 0) {
                        file.seek(OFFSET_ID);
                        byte existingID = 0;
                        bytesRead = file.read(&existingID, 1);
                        
                        if (bytesRead == 1) {
                            file.close();
                            root.close();
                            return existingID;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    root.close();
    return 0xFF; // No se encontró
}

bool BOTONERA_::confirmarCambioID(byte nuevaID) {
    // Petición de ELEM_ID_SECTOR al "nuevaID"
    frameReceived = false;
    Serial.println("Confirmamos cambio de id ####################");
    send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                          nuevaID,
                                          SPANISH_LANG,
                                          ELEM_ID_SECTOR));

    if (!esperar_respuesta(2500)) {
        Serial.println("No llegó respuesta de ELEM_ID_SECTOR");
        return false;
    }

    Serial.println(" 🥲🥲🥲 frameReceived: " + String(frameReceived));
    
    

    // Procesamos la trama recibida
    LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

    // Verificamos que data[0] sea ELEM_ID_SECTOR y data[1] sea la nuevaID
    if (LEF.data.size() >= 2 &&
        LEF.data[0] == ELEM_ID_SECTOR &&
        LEF.data[1] == nuevaID) {
        return true; // Confirmación OK
    }

    return false; // Sector incorrecto o ID incorrecta
}

bool BOTONERA_::esperar_respuesta(unsigned long timeout) {
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (frameReceived) return true; // Respuesta recibida
        delay(100);
    }
    return false; // Timeout
}

bool BOTONERA_::procesar_y_guardar_elemento_nuevo(byte targetID) {
    INFO_PACK_T* infoPack = new INFO_PACK_T;
    memset(infoPack, 0, sizeof(INFO_PACK_T));
    
    // Rellenamos la info base
    infoPack->ID = targetID;
    memcpy(infoPack->serialNum, lastSerial, 5);

    // Pedimos todos los sectores
    bool error = false;
    // Descargar sectores clave manualmente para evitar exclusión de LOCATION
    std::vector<int> sectores = {
        ELEM_NAME_SECTOR,
        ELEM_DESC_SECTOR,
        ELEM_LOCATION_SECTOR,
        ELEM_SERIAL_SECTOR,
        ELEM_ID_SECTOR,
        ELEM_CMODE_SECTOR
    };

    // Modos
    for (int i = 0; i < 16; i++) {
        sectores.push_back(ELEM_MODE_0_NAME_SECTOR + i * 3);
        sectores.push_back(ELEM_MODE_0_DESC_SECTOR + i * 3);
        sectores.push_back(ELEM_MODE_0_FLAG_SECTOR + i * 3);
    }

    // Icono
    for (int i = 0; i < 64; i++) {
        sectores.push_back(ELEM_ICON_ROW_0_SECTOR + i);
    }

    // Procesar todos
    for (int sector : sectores) {
        if (!procesar_sector(sector, infoPack, targetID)) {
            error = true;
            break;
        }
    }


    // Si falló alguno de los sectores, limpiamos y salimos
    if (error) {
        delete infoPack;
        return false;
    }

    // Si llegamos aquí, descargamos todo OK => lo guardamos en SPIFFS
    bool guardado = guardar_elemento(infoPack);
    print_info_pack(infoPack);
    delete infoPack;

    // guardado => true/false
    return guardado;
}

bool BOTONERA_::procesar_sector(int sector, INFO_PACK_T* infoPack, uint8_t targetID) {
    const int max_reintentos = 10;

    for (int intento = 0; intento < max_reintentos; intento++) {
        // Petición del sector al "targetID"
        frameReceived = false;
        send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                              targetID,
                                              SPANISH_LANG,
                                              sector));

        if (esperar_respuesta(2000)) {
            LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

            if (LEF.data.size() > 1 && LEF.data[0] == sector) {
                // Procesar y copiar datos
                procesar_datos_sector(LEF, sector, infoPack);

                // Actualizar alguna barra de progreso, si la tienes implementada
                char etiquetaSector[20];
                snprintf(etiquetaSector, sizeof(etiquetaSector),
                        "Sector %d/%d", sector, ELEM_ICON_ROW_63_SECTOR);
                actualizarBarraProgreso(sector, ELEM_ICON_ROW_63_SECTOR, etiquetaSector);
                return true;
            }
        }
    }
    return false;
}

void BOTONERA_::print_info_pack(const INFO_PACK_T *infoPack) {
    Serial.println("---- INFO DEL ELEMENTO ALMACENADO ----");
    
    Serial.print("Nombre: ");
    Serial.println((char*)infoPack->name);

    Serial.print("Descripción: ");
    Serial.println((char*)infoPack->desc);

    Serial.print("Número de Serie: ");
    Serial.printf("0x%02X%02X%02X%02X%02X\n", infoPack->serialNum[0], infoPack->serialNum[1], infoPack->serialNum[2], infoPack->serialNum[3], infoPack->serialNum[4]);

    Serial.print("ID: 0x");
    Serial.println(infoPack->ID, HEX);

    Serial.print("Modo Actual: ");
    Serial.println(infoPack->currentMode);

    // Imprimir información de los modos
    for (int i = 0; i < 16; ++i) {
        Serial.printf("Modo %d\n", i);
        Serial.print("  Nombre: ");
        Serial.println((char*)infoPack->mode[i].name);

        Serial.print("  Descripción: ");
        Serial.println((char*)infoPack->mode[i].desc);

        // Imprimir configuración en hexadecimal y binario
        Serial.printf("  Configuración: 0x%02X%02X (Binario: ", infoPack->mode[i].config[0], infoPack->mode[i].config[1]);

        // Imprimir el primer byte en binario
        for (int bit = 7; bit >= 0; --bit) {
            Serial.print((infoPack->mode[i].config[0] >> bit) & 1);
        }

        Serial.print(" ");

        // Imprimir el segundo byte en binario
        for (int bit = 7; bit >= 0; --bit) {
            Serial.print((infoPack->mode[i].config[1] >> bit) & 1);
        }

        Serial.println(")");
    }


    // Imprimir información del icono (solo las primeras filas para no saturar el Serial)
    Serial.println("---- ICONO DEL ELEMENTO (16 bits) ----");
    for (int row = 0; row < 5; ++row) {
        Serial.printf("Fila %02d: ", row);
        for (int col = 0; col < 64; ++col) {
            Serial.printf("%04X ", infoPack->icono[row][col]);  // Mostrar en hexadecimal
        }
        Serial.println();
    }
    Serial.println("---- FIN DEL ICONO ----");

    Serial.println("---- FIN DEL INFO PACK ----");
}

bool BOTONERA_::serialExistsInSPIFFS(byte serialNum[5]) {
    if (!SPIFFS.begin(true)) {
        return false;
    }
  
    fs::File root = SPIFFS.open("/");
    if (!root) {
        return false;
    }
    if (!root.isDirectory()) {
        root.close();
        return false;
    }
    
    root.rewindDirectory();
    
    // Verificar primero cuántos archivos hay en total
    int totalFiles = 0;
    while (true) {
        fs::File tempFile = root.openNextFile();
        if (!tempFile) break;
        totalFiles++;
        tempFile.close();
    }
    
    root.rewindDirectory();
    
    INFO_PACK_T *tempPack = new (std::nothrow) INFO_PACK_T;
    if (!tempPack) {
        root.close();
        return false;
    }
    
    bool found = false;
    int filesChecked = 0;
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        filesChecked++;
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo (añadir '/' al inicio si no está presente)
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Solo procesar archivos reales de elementos
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                // Leer el ID primero para verificar
                file.seek(OFFSET_ID);
                byte id;
                int bytesRead = file.read(&id, 1);
                
                if (bytesRead == 1) {
                    // Ahora leer el serial
                    file.seek(OFFSET_SERIAL);
                    bytesRead = file.read(tempPack->serialNum, 5);
                    
                    if (bytesRead == 5) {
                        // Comparar con el serial buscado
                        if (memcmp(tempPack->serialNum, serialNum, 5) == 0) {
                            found = true;
                            file.close();
                            break;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    delete tempPack;
    root.close();
    
    return found;
}

// void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
//     tft.fillScreen(TFT_BLACK);
//     dibujarMarco(TFT_WHITE);
    
//     tft.setTextColor(TFT_WHITE);
//     tft.setTextDatum(MC_DATUM); // Cada línea centrada
//     tft.setTextFont(2);
    
//     const uint16_t maxWidth = 120; // Ancho máximo por línea con margen
//     const uint8_t lineHeight = tft.fontHeight(); // Altura de la fuente
//     uint16_t yPos = 30; // Posición vertical inicial
//     const uint16_t screenHeight = 128;

//     const char* start = mensajeInicial;
//     const char* end = start;
//     const char* lastSpace = nullptr;

//     while (*start) {
//         end = start;
//         lastSpace = nullptr;
//         uint16_t currentWidth = 0;

//         // Encontrar el punto de corte para la línea actual
//         while (*end) {
//             if (*end == ' ') lastSpace = end;

//             // Calcular ancho hasta el carácter actual
//             char temp[21] = {0};
//             strncpy(temp, start, end - start + 1);
//             currentWidth = tft.textWidth(temp);

//             if (currentWidth > maxWidth) break;
//             end++;
//         }

//         // Ajustar el corte al último espacio si es posible
//         if (lastSpace && lastSpace > start && lastSpace < end) {
//             end = lastSpace;
//         } else if (end == start) {
//             end++; // Caso extremo: un carácter muy ancho
//         } else if (*end) {
//             end--; // Retroceder si se superó el ancho
//         }

//         // Extraer y dibujar la línea
//         char line[128] = {0};
//         strncpy(line, start, end - start);
//         tft.drawString(line, 64, yPos);

//         yPos += lineHeight;
//         if (yPos + lineHeight > screenHeight) break; // Verificar espacio vertical

//         start = (*end == ' ') ? end + 1 : end; // Saltar espacios si los hay
//     }
    
//     delay(100);
// }

// void BOTONERA_::actualizarBarraProgreso(float progreso) {
//     int barraAnchoMax = 100;
//     int barraAlto = 10;
//     int barraProgreso = (int)(barraAnchoMax * progreso / 100.0);

//     // Dibujar barra de fondo
//     tft.fillRoundRect(14, 60, barraAnchoMax, barraAlto, 5, TFT_DARKGREY);
    
//     // Dibujar barra de progreso
//     tft.fillRoundRect(14, 60, barraProgreso, barraAlto, 5, TFT_BLUE);

//     // Mostrar porcentaje
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextDatum(MC_DATUM);
//     tft.setTextFont(2); // Usa Font 2
//     tft.drawFloat(progreso, 0, 64, 90);
//     tft.drawString("%", 90, 90);
// }

// void BOTONERA_::actualizarBarraProgreso(float progreso, const char* detalleTexto = nullptr) {
//     int barraX = 14;       // Coordenada X de inicio de la barra
//     int barraY = 60;       // Coordenada Y de inicio de la barra
//     int barraAnchoMax = 100; // Ancho máximo de la barra (para 100%)
//     int barraAlto = 10;    // Alto de la barra

//     // Asegurar que el progreso esté entre 0 y 100
//     if (progreso < 0) progreso = 0;
//     if (progreso > 100) progreso = 100;

//     int barraProgresoActual = (int)(barraAnchoMax * progreso / 100.0);

//     // --- Limpieza del área de texto del progreso y detalle ---
//     // Asumimos que el texto de progreso y detalle está debajo de la barra.
//     // Ajusta estas coordenadas y tamaño si es necesario.
//     // Por ejemplo, si el texto va de y=75 a y=95 y ocupa todo el ancho de la pantalla o una sección.
//     // Aquí limpiaré un área genérica debajo de la barra y a la derecha.
//     // Coordenadas para limpiar el texto del porcentaje y detalle:
//     int textoCleanX = barraX; // Iniciar limpieza desde el mismo X de la barra
//     int textoCleanY = barraY + barraAlto + 2; // Un poco debajo de la barra
//     int textoCleanW = tft.width() - barraX - 5; // Ancho hasta casi el final de la pantalla
//     int textoCleanH = 20; // Alto suficiente para el texto de font 2 (aprox)
    
//     // Limpiar el área del texto del porcentaje (específicamente donde lo dibujas)
//     // tft.fillRect(60, 75, 60, 20, TFT_BLACK); // Ejemplo de limpieza específica del porcentaje si conoces las coords exactas

//     // Limpieza más genérica para texto debajo de la barra
//     tft.fillRect(textoCleanX, textoCleanY, textoCleanW, textoCleanH, TFT_BLACK);


//     // Dibujar barra de fondo
//     tft.fillRoundRect(barraX, barraY, barraAnchoMax, barraAlto, 3, TFT_DARKGREY); // Radio de esquina más pequeño
    
//     // Dibujar barra de progreso
//     if (barraProgresoActual > 0) { // Solo dibujar si hay progreso para evitar un artefacto de 0px
//         tft.fillRoundRect(barraX, barraY, barraProgresoActual, barraAlto, 3, TFT_BLUE);
//     }

//     // --- Mostrar porcentaje y detalle ---
//     tft.setTextColor(TFT_WHITE, TFT_BLACK); // Fondo negro para el texto
//     tft.setTextFont(2); // Usar Font 2

//     // Mostrar porcentaje a la derecha de la barra
//     int porcentajeX = barraX + barraAnchoMax + 5; // A la derecha de la barra
//     int porcentajeY = barraY + barraAlto / 2;     // Centrado verticalmente con la barra
//     tft.setTextDatum(ML_DATUM); // Middle-Left datum para alinear desde la izquierda
    
//     char progresoStr[10];
//     sprintf(progresoStr, "%.0f%%", progreso); // %.0f para no decimales en el porcentaje
//     tft.drawString(progresoStr, porcentajeX, porcentajeY);

//     // Mostrar texto de detalle debajo de la barra, si se proporciona
//     if (detalleTexto != nullptr && strlen(detalleTexto) > 0) {
//         tft.setTextDatum(MC_DATUM); // Middle-Center datum
//         // Posición para el texto de detalle, debajo de la barra
//         int detalleY = barraY + barraAlto + 10; // Ajusta este offset según necesites
//         tft.drawString(detalleTexto, tft.width() / 2, detalleY); // Centrado horizontalmente
//     }
// }

void BOTONERA_::finalizarEscaneoElemento() {
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);
    
    tft.setTextColor(TFT_GREEN);
    tft.setTextDatum(MC_DATUM);
    
    tft.setTextFont(4); // Usa Font 4 para "Escaneo"
    tft.drawString("Escaneo", 64, 40);
    
    tft.setTextFont(2); // Vuelve a Font 2 para "completado"
    tft.drawString("completado", 64, 60);
    
    // Dibujar un icono de verificación
    tft.fillCircle(64, 90, 10, TFT_GREEN);
    tft.drawLine(58, 90, 62, 94, TFT_WHITE);
    tft.drawLine(62, 94, 70, 86, TFT_WHITE);
    
    delay(3000);
    drawCurrentElement();  // Volver a la pantalla principal
}

void BOTONERA_::dibujarMarco(uint16_t color) {
    tft.drawRoundRect(2, 2, 124, 124, 10, color);
    tft.drawRoundRect(4, 4, 120, 120, 8, color);
}


#define MSG_ERROR_NO_RESPUESTA 0
#define MSG_ADVERTENCIA_ID_DESAJUSTE 1
#define MSG_EXITO_GENERICO 2
#define MSG_NUEVO_ELEMENTO_AGREGADO 3
#define MSG_ERROR_DESCONOCIDO 4

void BOTONERA_::mostrarMensajeTemporal(int respuesta, int dTime) {
    // Borrar la pantalla y dibujar el marco
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);

    // Determinar color y mensajes
    uint32_t colorTexto;
    const char* mensajePrincipal;
    const char* mensajeSecundario;

    if (respuesta == 0) { // ERROR GENERAL
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Sin respuesta"; // O un mensaje más genérico si se usa para otros errores
    } else if (respuesta == 1) { // ADVERTENCIA (ej: elemento existente, ID desajustada)
        colorTexto = TFT_YELLOW;
        mensajePrincipal = "ADVERTENCIA";
        mensajeSecundario = "Verifique el estado del elemento"; // Mensaje genérico de advertencia
    } else if (respuesta == 2) { // ÉXITO / NUEVO (ej: elemento agregado, ID confirmada)
        colorTexto = TFT_GREEN;
        mensajePrincipal = "ÉXITO";
        mensajeSecundario = "Operación completada correctamente"; // Mensaje genérico de éxito
    } else if (respuesta == 3) { // ERROR ESPECÍFICO (podrías definir más)
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Fallo en la operación"; // Mensaje más específico
    } else { // ERROR DESCONOCIDO
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Error desconocido";
    }

    // Mostrar mensaje principal centrado
    tft.setTextFont(4);
    if (respuesta == 1) tft.setTextFont(2);  // Fuente aún más pequeña
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(colorTexto);
    tft.drawString(mensajePrincipal, 64, 30);

    // Configurar fuente pequeña para texto secundario
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE);

    // Dividir el texto secundario en líneas automáticamente
    const int maxCharsPerLine = 18;
    const int lineHeight = 20;
    char buffer[maxCharsPerLine + 1];
    int textLength = strlen(mensajeSecundario);
    int yPos = 60;

    for (int i = 0; i < textLength; i += maxCharsPerLine) {
        strncpy(buffer, mensajeSecundario + i, maxCharsPerLine);
        buffer[maxCharsPerLine] = '\0'; // Finalizar correctamente
        tft.drawString(buffer, 65, yPos);
        yPos += lineHeight;
    }

    // Dibujar icono correspondiente
    int iconCenterX = 64;
    int iconCenterY = 110;
    int iconSize = 10;

    if (respuesta == 0 || respuesta == 3 || (respuesta != 1 && respuesta != 2)) {  // ERROR (0, 3, default)
        tft.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_RED);
        tft.drawLine(iconCenterX - iconSize/2 +2 , iconCenterY - iconSize/2 +2, iconCenterX + iconSize/2 -2, iconCenterY + iconSize/2 -2, TFT_WHITE);
        tft.drawLine(iconCenterX + iconSize/2 -2, iconCenterY - iconSize/2 +2, iconCenterX - iconSize/2 +2, iconCenterY + iconSize/2 -2, TFT_WHITE);
    }
    else if (respuesta == 2) {  // ÉXITO / NUEVO
        tft.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_GREEN);
        tft.drawLine(iconCenterX - iconSize/2 +2 , iconCenterY, iconCenterX - iconSize/2 + 4, iconCenterY + 2, TFT_WHITE); // Ajustar tick
        tft.drawLine(iconCenterX - iconSize/2 + 4, iconCenterY + 2, iconCenterX + iconSize/2 -1, iconCenterY - 3, TFT_WHITE); // Ajustar tick
    }
    else if (respuesta == 1) {  // ADVERTENCIA
        int triangleSize = iconSize * 2;
        float triangleHeight = triangleSize * 0.866f;
        tft.fillTriangle(
            iconCenterX, iconCenterY - (triangleHeight / 2),
            iconCenterX - (triangleSize / 2), iconCenterY + (triangleHeight / 2),
            iconCenterX + (triangleSize / 2), iconCenterY + (triangleHeight / 2),
            TFT_YELLOW
        );
        int exclamationHeight = (int)(triangleHeight * 0.6); // Ajustado para más proporción
        int barY = iconCenterY - (exclamationHeight / 2) +1; // +1 para bajarlo un poco
        tft.fillRect(iconCenterX - 1, barY, 2, exclamationHeight - 3, TFT_BLACK); // barra
        tft.fillRect(iconCenterX - 1, barY + exclamationHeight -3 + 1, 2, 2, TFT_BLACK); // punto
    }

    // Esperar y regresar al menú
    delay(dTime);
    drawCurrentElement();
}

byte BOTONERA_::getNextAvailableID() {
    byte nextID = 0x01;
    
    if (!SPIFFS.begin(true)) {
        return 0xDD;
    }
    
    fs::File root = SPIFFS.open("/");
    if (!root) {
        return 0xDD;
    }
    if (!root.isDirectory()) {
        root.close();
        return 0xDD;
    }
    
    // Contar total de archivos
    root.rewindDirectory();
    int totalFiles = 0;
    while (true) {
        fs::File tempFile = root.openNextFile();
        if (!tempFile) break;
        
        totalFiles++;
        tempFile.close();
    }
    
    // Buscar archivos de elementos
    root.rewindDirectory();
    int elementCount = 0;
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo (añadir '/' al inicio si no está presente)
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Verificar si es un archivo de elemento
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            elementCount++;
            
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                // Leer la ID
                file.seek(OFFSET_ID);
                byte existingID = 0;
                int bytesRead = file.read(&existingID, 1);
                
                if (bytesRead == 1) {
                    // Verificar el número de serie para confirmar que es un elemento válido
                    file.seek(OFFSET_SERIAL);
                    byte serial[5];
                    bytesRead = file.read(serial, 5);
                    
                    if (bytesRead == 5) {
                        // Actualizar nextID si es necesario
                        if (existingID >= nextID) {
                            nextID = existingID + 1;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    root.close();
    
    return nextID;
}

bool inCognitiveMenu = false;

void BOTONERA_::activateCognitiveMode() {
    inCognitiveMenu = true;
    drawCognitiveMenu();
    colorHandler.mapCognitiveLEDs(); // función que veremos abajo
}
void BOTONERA_::deactivateCognitiveMode() {
    inCognitiveMenu = false;
    drawCurrentElement(); // Volver a la pantalla principal
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool scanDD = false;
// === Funciones Auxiliares NUEVAS o MODIFICADAS ===

/**
 * @brief Pings a targetID for its serial number.
 * @param targetID The ID of the element to scan.
 * @param serial Output array (5 bytes) to store the received serial number.
 * @param timeoutPerAttempt Timeout in ms for each attempt.
 * @param retries Number of retries.
 * @return true if the element responded with a valid serial number, false otherwise.
 */

bool BOTONERA_::escanearID(byte targetID, byte serial[5], unsigned long timeoutPerAttempt, int retries) {
    Serial.printf("escanearID: Solicitando serial a ID 0x%02X...\n", targetID);
    for (int i = 0; i < retries; ++i) {
        frameReceived = false;
            send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                                  targetID,
                                                  SPANISH_LANG,
                                                  ELEM_SERIAL_SECTOR));
        

        if (esperar_respuesta(timeoutPerAttempt)) { // esperar_respuesta usa frameReceived
            LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

            // Validar que la respuesta sea del sector correcto y tenga suficientes datos
            // Data[0] es el sector, Data[1] a Data[5] es el serial.
            if (LEF.function == F_RETURN_ELEM_SECTOR && LEF.data.size() >= (1 + 5) && LEF.data[0] == ELEM_SERIAL_SECTOR) {
                memcpy(serial, &LEF.data[1], 5);
                Serial.printf("escanearID: ID 0x%02X respondió con SN: %02X%02X%02X%02X%02X\n",
                              targetID, serial[0], serial[1], serial[2], serial[3], serial[4]);
                return true;
            } else {
                Serial.printf("escanearID: Respuesta inválida de ID 0x%02X (Fun:0x%02X, Sector:0x%02X, Size:%d). Intento %d/%d\n",
                              targetID, LEF.function, (LEF.data.size()>0 ? LEF.data[0] : 0xFF), LEF.data.size(), i + 1, retries);
            }
        } else {
            Serial.printf("escanearID: Timeout esperando respuesta de ID 0x%02X. Intento %d/%d\n", targetID, i + 1, retries);
        }
        if (i < retries - 1) delay(50); // Pequeña pausa antes del siguiente reintento
    }
    return false; // No se obtuvo respuesta válida tras todos los reintentos
}

/**
 * @brief Finds an element file in SPIFFS by its serial number and updates its ID field.
 * @param serial The serial number (5 bytes) of the element to update.
 * @param nuevaID The new ID to write into the element's file.
 */
void BOTONERA_::actualizarIDenSPIFFS(const byte serial[5], byte nuevaID) {
    Serial.printf("actualizarIDenSPIFFS: Buscando SN %02X%02X%02X%02X%02X para actualizar ID a 0x%02X\n",
                  serial[0], serial[1], serial[2], serial[3], serial[4], nuevaID);

    if (!SPIFFS.begin(true)) {
        Serial.println("actualizarIDenSPIFFS: Error al montar SPIFFS.");
        return;
    }

    // Iterar sobre los archivos conocidos por loadElementsFromSPIFFS
    // Esto es más eficiente que abrir y cerrar cada archivo del directorio raíz.
    for (const String& filePath : elementFiles) {
        fs::File file = SPIFFS.open(filePath, "r+"); // Abrir en modo lectura/escritura
        if (!file) {
            Serial.printf("actualizarIDenSPIFFS: No se pudo abrir el archivo %s\n", filePath.c_str());
            continue;
        }

        if (file.size() < MIN_VALID_ELEMENT_SIZE) {
            file.close();
            continue;
        }

        byte existingSerial[5];
        file.seek(OFFSET_SERIAL);
        if (file.read(existingSerial, 5) == 5) {
            if (memcmp(existingSerial, serial, 5) == 0) {
                // Serial coincide, actualizamos la ID
                byte oldID;
                file.seek(OFFSET_ID);
                file.read(&oldID,1); // Leer la ID antigua por si acaso o para loguear

                file.seek(OFFSET_ID); // Volver a posicionar para escribir
                if (file.write(&nuevaID, 1) == 1) {
                    Serial.printf("actualizarIDenSPIFFS: Éxito. ID para SN %02X... actualizada de 0x%02X a 0x%02X en archivo %s\n",
                                  serial[0], oldID, nuevaID, filePath.c_str());
                } else {
                    Serial.printf("actualizarIDenSPIFFS: Error al escribir nueva ID en archivo %s\n", filePath.c_str());
                }
                file.close();
                // Una vez actualizado, se podría recargar elementFiles o la info del elemento específico si está en RAM.
                // loadElementsFromSPIFFS(); // Opcional: recargar todo si es necesario inmediatamente.
                return; // Encontrado y procesado (o intento de proceso)
            }
        }
        file.close();
    }
    Serial.printf("actualizarIDenSPIFFS: No se encontró archivo en SPIFFS con SN %02X...\n", serial[0]);
}

/**
 * @brief Finds the first ID (1-32) not marked as true in the `ocupadas` array.
 * @param ocupadas Boolean array of 32 elements. ocupadas[i] is true if ID (i+1) is taken.
 * @return The first free ID (1-32), or 0xFF if no ID is free.
 */
byte BOTONERA_::buscarPrimerIDLibre(const bool ocupadas[32]) {
    for (int i = 0; i < 32; ++i) {
        if (!ocupadas[i]) {
            Serial.printf("buscarPrimerIDLibre: ID libre encontrada: 0x%02X\n", (byte)(i + 1));
            return (byte)(i + 1); // IDs son 1-based
        }
    }
    Serial.println("buscarPrimerIDLibre: No hay IDs libres entre 1 y 32.");
    return 0xFF; // Ninguna ID libre encontrada
}


/**
 * @brief Downloads all sectors from a new element and saves it to SPIFFS.
 *        This is a modified version that takes the serial number as a parameter.
 * @param targetID The ID of the new element.
 * @param serialNumDelElemento The serial number (5 bytes) of the new element.
 * @return true if the element was successfully processed and saved, false otherwise.
 */
bool BOTONERA_::procesar_y_guardar_elemento_nuevo(byte targetID, const byte serialNumDelElemento[5]) {
    INFO_PACK_T* infoPack = new INFO_PACK_T;
    if (!infoPack) {
    Serial.println("procesar_y_guardar_elemento_nuevo: Error: Fallo al alocar memoria para INFO_PACK_T");
    return false;
    }
    memset(infoPack, 0, sizeof(INFO_PACK_T));

    // Rellenamos la info base
    infoPack->ID = targetID;
    memcpy(infoPack->serialNum, serialNumDelElemento, 5); // Usar el serial pasado como parámetro

    Serial.printf("procesar_y_guardar_elemento_nuevo: Preparando para descargar info de ID 0x%02X SN:%02X%02X%02X%02X%02X\n",
                targetID, serialNumDelElemento[0], serialNumDelElemento[1], serialNumDelElemento[2], serialNumDelElemento[3], serialNumDelElemento[4]);

    // Definición de los sectores a descargar
    std::vector<int> sectores_a_descargar = {
        ELEM_NAME_SECTOR, ELEM_DESC_SECTOR, ELEM_LOCATION_SECTOR,
        ELEM_CMODE_SECTOR
        // ELEM_SERIAL_SECTOR y ELEM_ID_SECTOR podrían pedirse de nuevo para doble confirmación,
        // pero ya tenemos el serial y hemos asignado (o estamos usando) targetID.
        // Si se añaden, hay que asegurarse que procesar_datos_sector no sobrescriba
        // infoPack->ID o infoPack->serialNum si ya están seteados correctamente.
        // Por ahora, los omitimos para eficiencia, asumiendo que la info que tenemos es correcta.
    };

    // Añadir sectores de los 16 modos
    for (int i = 0; i < 16; i++) {
        sectores_a_descargar.push_back(ELEM_MODE_0_NAME_SECTOR + i * 3);  // Nombre del modo i
        sectores_a_descargar.push_back(ELEM_MODE_0_DESC_SECTOR + i * 3);  // Descripción del modo i
        sectores_a_descargar.push_back(ELEM_MODE_0_FLAG_SECTOR + i * 3); // Config/Flags del modo i
    }

    // Añadir sectores del icono (64 filas)
    for (int i = 0; i < ICON_ROWS; i++) { // ICON_ROWS debería ser 64
        sectores_a_descargar.push_back(ELEM_ICON_ROW_0_SECTOR + i);
    }

    // Interfaz de Usuario: Iniciar progreso de descarga
    // iniciarEscaneoElemento("Descargando Info..."); // Mensaje general de descarga
    // actualizarBarraProgreso(0);

    bool error_descarga = false;
    for (size_t i = 0; i < sectores_a_descargar.size(); ++i) {
        int sector = sectores_a_descargar[i];
        Serial.printf("Descargando sector 0x%02X para ID 0x%02X (%zu/%zu)\n", sector, targetID, i + 1, sectores_a_descargar.size());
        
        // Actualizar UI de progreso
        // float progreso = ((float)(i + 1) / sectores_a_descargar.size()) * 100.0;
        // actualizarBarraProgreso(progreso);

        if (!procesar_sector(sector, infoPack, targetID)) { // procesar_sector es una función existente
            Serial.printf("procesar_y_guardar_elemento_nuevo: Error al procesar sector 0x%02X para ID 0x%02X.\n", sector, targetID);
            error_descarga = true;
            break;
        }
    }

    if (error_descarga) {
        delete infoPack;
        Serial.printf("procesar_y_guardar_elemento_nuevo: Falló la descarga de uno o más sectores para ID 0x%02X.\n", targetID);
        // mostrarMensajeTemporal("Error Descarga", 2000);
        return false;
    }

    Serial.printf("procesar_y_guardar_elemento_nuevo: Todos los sectores descargados para ID 0x%02X. Guardando en SPIFFS...\n", targetID);
    // print_info_pack(infoPack); // Descomentar para depuración si es necesario

    bool guardado_exitoso = guardar_elemento(infoPack); // guardar_elemento es una función existente
    delete infoPack; // Liberar memoria del infoPack

    if (guardado_exitoso) {
        Serial.printf("procesar_y_guardar_elemento_nuevo: Elemento ID 0x%02X guardado exitosamente.\n", targetID);
        // `guardar_elemento` ya debería llamar a `loadElementsFromSPIFFS()` internamente si es necesario.
        // mostrarMensajeTemporal("Elemento Guardado", 2000);
    } else {
        Serial.printf("procesar_y_guardar_elemento_nuevo: Error al guardar el elemento ID 0x%02X en SPIFFS.\n", targetID);
        // mostrarMensajeTemporal("Error Guardando", 2000);
    }

    return guardado_exitoso;
}

/**
 * @brief Checks if SPIFFS contains a file for an element that is registered with the given ID.
 * @param idToFind The ID to look for in SPIFFS element files.
 * @return true if an element file exists and is registered with idToFind, false otherwise.
 */
bool BOTONERA_::elementoAsignadoA_ID_enSPIFFS(byte idToFind) {
    if (!SPIFFS.begin(true)) {
        Serial.println("elementoAsignadoA_ID_enSPIFFS: Error al montar SPIFFS.");
        return false;
    }

    // Iterar sobre los archivos conocidos por loadElementsFromSPIFFS
    for (const String& filePath : elementFiles) {
        fs::File file = SPIFFS.open(filePath, "r");
        if (!file) {
            // Serial.printf("elementoAsignadoA_ID_enSPIFFS: No se pudo abrir %s\n", filePath.c_str());
            continue;
        }

        if (file.size() < MIN_VALID_ELEMENT_SIZE) { // Asegúrate que MIN_VALID_ELEMENT_SIZE y OFFSET_ID estén definidos
            file.close();
            continue;
        }

        byte idEnArchivo;
        file.seek(OFFSET_ID); //OFFSET_ID debe estar definido
        if (file.read(&idEnArchivo, 1) == 1) {
            if (idEnArchivo == idToFind) {
                file.close();
                // Serial.printf("elementoAsignadoA_ID_enSPIFFS: ID 0x%02X encontrada en %s\n", idToFind, filePath.c_str());
                return true;
            }
        }
        file.close();
    }
    // Serial.printf("elementoAsignadoA_ID_enSPIFFS: ID 0x%02X no encontrada en ningún archivo de SPIFFS.\n", idToFind);
    return false;
}

// void BOTONERA_::escanearSala() {
// // Etiqueta para reiniciar el proceso de escaneo completo si se reasigna un 0xDD
// inicio_escanear_sala_completo:
//     Serial.println("=== 🚀 INICIO ESCANEO DE SALA (REVISADO) 🚀 ===");
//     scanDD = false; // IMPORTANTE: Para que escanearID (1-32) pida ELEM_SERIAL_SECTOR
//     iniciarEscaneoElemento("Escaneando Sala (1-32)...");

//     loadElementsFromSPIFFS(); // Carga el estado actual de SPIFFS

//     bool listaIDsOcupadasScanActual[32]; // Estado *detectado en vivo* en la pasada actual 1-32
//     bool changeFlag;                     // Detecta cambios *dentro* del bucle de escaneo 1-32 que fuerzan reinicio de ese bucle
//     byte currentID;

//     do {
//         changeFlag = false;
//         currentID = 1;
//         // Limpia la lista de IDs ocupadas para ESTA pasada del barrido 1-32.
//         // Esta lista se construye basándose en lo que *realmente responde* en el bus.
//         memset(listaIDsOcupadasScanActual, false, sizeof(listaIDsOcupadasScanActual));

//         Serial.println("--- Iniciando pasada de escaneo IDs 1-32 ---");
//         actualizarBarraProgreso(0);

//         while (currentID <= 32) {
//             Serial.printf("🔍 Escaneando ID: 0x%02X (%d/32)\n", currentID, currentID);
//             float progress = ((float)(currentID - 1) / 32.0) * 100.0;
//             actualizarBarraProgreso(progress);

//             byte serialRecibido[5];
//             // scanDD es false aquí, por lo que escanearID pedirá ELEM_SERIAL_SECTOR
//             bool responde = escanearID(currentID, serialRecibido, 600, 2);

//             if (!responde) {
//                 Serial.printf("ID 0x%02X: No responde.\n", currentID);
//                 listaIDsOcupadasScanActual[currentID - 1] = false; // Confirmado no ocupado en esta pasada
//                 // bool seEsperabaElementoEnEstaIDPorSPIFFS = elementoAsignadoA_ID_enSPIFFS(currentID);
//                 // if (seEsperabaElementoEnEstaIDPorSPIFFS) {
//                 //     // Considerar si una desaparición debe activar changeFlag para el bucle 1-32.
//                 //     // Tu lógica original no lo hacía, lo cual es razonable si changeFlag es para adiciones/actualizaciones
//                 //     // que requieren reprocesar IDs ya vistas.
//                 // }
//             } else { // Sí responde
//                 Serial.printf("ID 0x%02X: Responde con SN: %02X%02X%02X%02X%02X\n",
//                               currentID, serialRecibido[0], serialRecibido[1], serialRecibido[2], serialRecibido[3], serialRecibido[4]);
                
//                 listaIDsOcupadasScanActual[currentID - 1] = true; // Confirmado ocupado en esta pasada

//                 bool snExisteEnSPIFFS_flag = serialExistsInSPIFFS(serialRecibido);

//                 if (!snExisteEnSPIFFS_flag) {
//                     Serial.printf("SN %02X... no encontrado en SPIFFS. Tratando como nuevo en ID 0x%02X.\n", serialRecibido[0], currentID);
//                     bool elementoAgregadoConExito = false;
//                     for (int intento = 0; intento < 3; ++intento) {
//                         if (procesar_y_guardar_elemento_nuevo(currentID, serialRecibido)) {
//                             elementoAgregadoConExito = true;
//                             break;
//                         }
//                         Serial.printf("Fallo intento %d/3 procesar_y_guardar para ID 0x%02X. Reintentando...\n", intento + 1, currentID);
//                         delay(200);
//                     }

//                     if (elementoAgregadoConExito) {
//                         Serial.printf("Nuevo elemento en ID 0x%02X guardado.\n", currentID);
//                         changeFlag = true; // Adición: reiniciar barrido 1-32
//                         Serial.println("Elemento nuevo agregado. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;
//                     } else {
//                         Serial.printf("ERROR: Fallaron todos los intentos para guardar nuevo elemento en ID 0x%02X.\n", currentID);
//                         listaIDsOcupadasScanActual[currentID - 1] = false; // No se pudo guardar, no está realmente ocupado de forma consistente
//                     }
//                 } else { // SN EXISTE en SPIFFS
//                     byte idSPIFFS = getIdFromSPIFFS(serialRecibido);
//                     Serial.printf("SN %02X... encontrado en SPIFFS. ID en SPIFFS: 0x%02X. ID actual respuesta: 0x%02X.\n", serialRecibido[0], idSPIFFS, currentID);

//                     if (idSPIFFS == 0xFF) {
//                         Serial.printf("ERROR CRÍTICO: SN %02X... existe en SPIFFS pero ID guardada es inválida (0xFF).\n", serialRecibido[0]);
//                         // Podríamos tratarlo como si necesitara una actualización de ID en SPIFFS a currentID
//                         // O asumir que el registro está corrupto y requiere descarga.
//                         // Por simplicidad, si está en currentID y el SN coincide, pero idSPIFFS es 0xFF,
//                         // actualizamos SPIFFS para que refleje la realidad.
//                         Serial.printf("Actualizando SPIFFS para SN %02X... con ID actual 0x%02X.\n", serialRecibido[0], currentID);
//                         actualizarIDenSPIFFS(serialRecibido, currentID);
//                         changeFlag = true;
//                         Serial.println("ID en SPIFFS (era 0xFF) actualizada. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;

//                     } else if (idSPIFFS == currentID) {
//                         Serial.printf("ID 0x%02X: Correcta y confirmada con SPIFFS.\n", currentID);
//                         // No se necesita changeFlag.
//                     } else { // ID no coincide: El elemento está en currentID, pero SPIFFS dice otra cosa (idSPIFFS != 0xFF).
//                         Serial.printf("ID 0x%02X: Desajuste. SPIFFS dice 0x%02X para SN %02X.... Actualizando SPIFFS a 0x%02X.\n", currentID, idSPIFFS, serialRecibido[0], currentID);
//                         actualizarIDenSPIFFS(serialRecibido, currentID);
//                         changeFlag = true; // Actualización de ID en SPIFFS: reiniciar barrido 1-32
//                         Serial.println("ID en SPIFFS actualizada. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;
//                     }
//                 }
//             }
//             currentID++;
//         } // Fin while (currentID <= 32)

//     reiniciar_barrido_1_32:;
//         if (currentID <= 32 && changeFlag) {
//             Serial.printf("Interrupción en ID 0x%02X para reiniciar escaneo 1-32 debido a changeFlag.\n", currentID);
//         }
//         Serial.printf("--- Pasada de escaneo IDs 1-32 completada. ChangeFlag: %s ---\n", changeFlag ? "TRUE" : "FALSE");

//     } while (changeFlag); // Repetir barrido 1-32 si hubo adiciones/actualizaciones que requirieron reinicio inmediato.

//     // En este punto, el barrido 1-32 está estable, y listaIDsOcupadasScanActual refleja el estado en vivo.
//     Serial.println("=== Escaneo IDs 1-32 ESTABLE ===");
//     actualizarBarraProgreso(100);
//     delay(500);

//     // 🔶 PASO 2: Gestión de múltiples elementos con ID = 0xDD (NUEVA LÓGICA)
//     Serial.println("--- Iniciando gestión de dispositivos en ID 0xDD (DEFAULT_DEVICE) ---");
//     iniciarEscaneoElemento("Buscando en 0xDD...");
//     bool seProcesoUnDDYRequiereReinicio = false;
    
//     frameReceived = false; // Limpiar estado de frame previo

//     Serial.printf("Enviando ELEM_ATTACH_REQ a ID 0x%02X...\n", DEFAULT_DEVICE);
//     // No usamos escanearID aquí porque queremos enviar una única petición y escuchar múltiples respuestas.
//     // `scanDD` no afecta directamente a este `send_frame`.
//     send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
//                                           DEFAULT_DEVICE,    // Target 0xDD
//                                           SPANISH_LANG,
//                                           ELEM_LAST_SPLIT_ATTACH_REQ)); // ELEM_ATTACH_REQ = 0

//     Serial.println("Esperando respuestas de dispositivos 0xDD durante 20 segundos...");
//     unsigned long tiempoInicioEsperaDD = millis();
//     std::vector<std::array<byte, 5>> serialesDDProcesadosEnEstaVentana; // Para no procesar el mismo serial múltiples veces en esta ventana

//     int ddResponsesProcessedCount = 0;
//     while (millis() - tiempoInicioEsperaDD < 20000) {
//         if (esperar_respuesta(50)) { // Polling con timeout corto
//             LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
//             frameReceived = false; // Frame procesado o a punto de serlo

//             // Validar que la respuesta es de un 0xDD y es un serial
//             if (LEF.origin == DEFAULT_DEVICE &&
//                 LEF.function == F_RETURN_ELEM_SECTOR &&
//                 LEF.data.size() >= (1 + 5) && // 1 byte para tipo de sector, 5 para serial
//                 LEF.data[0] == ELEM_ATTACH_REQ) {

//                 byte serialRecibidoDD[5];
//                 memcpy(serialRecibidoDD, &LEF.data[1], 5);
//                 Serial.printf("Respuesta de 0xDD con SN: %02X%02X%02X%02X%02X\n",
//                               serialRecibidoDD[0], serialRecibidoDD[1], serialRecibidoDD[2], serialRecibidoDD[3], serialRecibidoDD[4]);
                
//                 ddResponsesProcessedCount++;
//                 // Actualizar UI de progreso si es relevante aquí
//                 //actualizarBarraProgreso((float)ddResponsesProcessedCount * X); // Ajustar X

//                 bool yaProcesadoEnEstaVentana = false;
//                 for (const auto& s_arr : serialesDDProcesadosEnEstaVentana) {
//                     if (memcmp(s_arr.data(), serialRecibidoDD, 5) == 0) {
//                         yaProcesadoEnEstaVentana = true;
//                         break;
//                     }
//                 }
//                 if (yaProcesadoEnEstaVentana) {
//                     Serial.printf("SN %02X... de 0xDD ya fue re-ID'd en esta ventana. Ignorando respuesta duplicada.\n", serialRecibidoDD[0]);
//                     continue;
//                 }

//                 // Intentar reasignar ID
//                 bool reasignacionExitosaEsteDD = false;
//                 byte idAsignadaOReafirmada = 0xFF;

//                 if (serialExistsInSPIFFS(serialRecibidoDD)) {
//                     byte idEnSpiffs = getIdFromSPIFFS(serialRecibidoDD);
//                     if (idEnSpiffs >= 1 && idEnSpiffs <= 32) { // SN conocido y con ID válida (1-32)
//                         Serial.printf("SN %02X... (en 0xDD) ya existe en SPIFFS con ID 0x%02X. Reafirmando esa ID.\n", serialRecibidoDD[0], idEnSpiffs);
//                         send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, idEnSpiffs)); // Origen es 0xDD
//                         delay(250); // Tiempo para cambio
//                         if (confirmarCambioID(idEnSpiffs)) { // Confirmar en la ID esperada
//                              Serial.printf("Confirmado: Elemento 0xDD (SN %02X...) restaurado a ID 0x%02X.\n", serialRecibidoDD[0], idEnSpiffs);
//                              listaIDsOcupadasScanActual[idEnSpiffs - 1] = true; // Marcar como ocupada para esta ventana
//                              reasignacionExitosaEsteDD = true;
//                              idAsignadaOReafirmada = idEnSpiffs;
//                         } else {
//                             Serial.printf("ADVERTENCIA: No se pudo confirmar restauración a ID 0x%02X para 0xDD (SN %02X...). Puede seguir en 0xDD.\n", idEnSpiffs, serialRecibidoDD[0]);
//                         }
//                     } else { // SN conocido pero con ID 0xDD, 0xFF o inválida en SPIFFS. Tratar como si necesitara nueva ID.
//                         Serial.printf("SN %02X... (en 0xDD) existe en SPIFFS con ID 0x%02X no válida (o 0xDD). Buscando ID libre.\n", serialRecibidoDD[0], idEnSpiffs);
//                         // Continuar para buscar idLibre
//                     }
//                 }
                
//                 // Si no se reafirmó ID (porque no estaba en SPIFFS con ID 1-32, o la reafirmación falló), buscar ID libre
//                 if (!reasignacionExitosaEsteDD) {
//                     byte idLibre = buscarPrimerIDLibre(listaIDsOcupadasScanActual); // Usa la lista actualizada por el barrido 1-32 estable
//                     if (idLibre != 0xFF && idLibre != 0x00 && idLibre != DEFAULT_DEVICE) {
//                         Serial.printf("Asignando ID 0x%02X al elemento 0xDD con SN %02X...\n", idLibre, serialRecibidoDD[0]);
//                         send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, idLibre)); // Origen es 0xDD
//                         delay(250);
//                         if (confirmarCambioID(idLibre)) { // Confirmar en la nueva ID
//                             Serial.printf("Confirmado: Elemento 0xDD (SN %02X...) ahora en ID 0x%02X.\n", serialRecibidoDD[0], idLibre);
//                             listaIDsOcupadasScanActual[idLibre - 1] = true; // Marcar como ocupada para esta ventana
//                             reasignacionExitosaEsteDD = true;
//                             idAsignadaOReafirmada = idLibre;
//                         } else {
//                             Serial.printf("ADVERTENCIA: No se pudo confirmar cambio a ID 0x%02X para 0xDD (SN %02X...). Pudo no cambiar de ID.\n", idLibre, serialRecibidoDD[0]);
//                         }
//                     } else {
//                         Serial.printf("ERROR: No hay ID libre válida (1-32) para 0xDD con SN %02X...\n", serialRecibidoDD[0]);
//                     }
//                 }

//                 if (reasignacionExitosaEsteDD) {
//                     seProcesoUnDDYRequiereReinicio = true;
//                     std::array<byte, 5> s_arr_mem; memcpy(s_arr_mem.data(), serialRecibidoDD, 5);
//                     serialesDDProcesadosEnEstaVentana.push_back(s_arr_mem);
//                     // No se guardan datos aquí. El reinicio de escanearSala lo hará si es nuevo.
//                 }

//             } else if (LEF.origin == DEFAULT_DEVICE) {
//                  Serial.printf("Respuesta inesperada de 0xDD: Fun:0x%02X, Data[0]:0x%02X. Ignorando.\n",
//                                 LEF.function, (LEF.data.size()>0 ? LEF.data[0] : 0xFF) );
//             }
//             // Ignorar otros frames que no sean de 0xDD o no sean la respuesta esperada.
//         }
//         delay(10); // Pequeña pausa para no saturar CPU, permite otros procesos.
//     } // Fin del while de 8 segundos

//     Serial.println("--- Fin de la ventana de 8 segundos para dispositivos 0xDD ---");

//     if (seProcesoUnDDYRequiereReinicio) {
//         Serial.println("Se procesaron uno o más elementos 0xDD y se les asignó/reafirmó ID. Reiniciando escaneo de sala completo...");
//         // El siguiente escaneo completo (1-32) encontrará estos dispositivos en sus nuevas (o reafirmadas) IDs.
//         // Si un dispositivo reasignado a idLibre es nuevo para SPIFFS (o su registro en SPIFFS tenía ID 0xDD/0xFF),
//         // la lógica de `!snExisteEnSPIFFS_flag` o `idSPIFFS == 0xFF` en el barrido 1-32
//         // se encargará de llamar a `procesar_y_guardar_elemento_nuevo`.
//         // Si se reafirmó una ID existente (1-32), el barrido 1-32 simplemente lo confirmará.
//         goto inicio_escanear_sala_completo;
//     } else {
//         Serial.println("No se procesaron elementos 0xDD que requieran reinicio, o no hubo respuestas de 0xDD.");
//     }

//     // Si llegamos aquí, el escaneo 1-32 está estable Y NINGÚN 0xDD causó reinicio.
//     loadElementsFromSPIFFS(); // Carga final para reflejar cualquier cambio (aunque improbable si no hubo reinicio por 0xDD).
//     iniciarEscaneoElemento("Escaneo Finalizado");
//     actualizarBarraProgreso(100);
//     Serial.println("=== ✅ FIN ESCANEO DE SALA COMPLETO (SIN REINICIO POR 0xDD) ✅ ===");
// }

// Asumimos que ELEM_FIRST_SPLIT_ATTACH_REQ y ELEM_LAST_SPLIT_ATTACH_REQ
// son constantes definidas, por ejemplo, en tu enum SECTOR_LIST.
// const byte ELEM_FIRST_SPLIT_ATTACH_REQ = X; // Reemplaza X con el valor real
// const byte ELEM_LAST_SPLIT_ATTACH_REQ = Y;  // Reemplaza Y con el valor real
//=======================================================================================================================
// void BOTONERA_::escanearSala() {
// // Etiqueta para reiniciar el proceso de escaneo completo si se reasigna un 0xDD
// inicio_escanear_sala_completo:
//     Serial.println("=== 🚀 INICIO ESCANEO DE SALA (CON SPLIT ATTACH) 🚀 ===");
//     scanDD = false; // Para que escanearID (1-32) pida ELEM_SERIAL_SECTOR
//     iniciarEscaneoElemento("Escaneando Sala (1-32)...");

//     loadElementsFromSPIFFS();

//     bool listaIDsOcupadasScanActual[32];
//     bool changeFlag;
//     byte currentID;

//     do {
//         changeFlag = false;
//         currentID = 1;
//         memset(listaIDsOcupadasScanActual, false, sizeof(listaIDsOcupadasScanActual));

//         Serial.println("--- Iniciando pasada de escaneo IDs 1-32 ---");
//         actualizarBarraProgreso(0);

//         while (currentID <= 32) {
//             Serial.printf("🔍 Escaneando ID: 0x%02X (%d/32)\n", currentID, currentID);
//             float progress = ((float)(currentID - 1) / 32.0) * 100.0;
//             actualizarBarraProgreso(progress);

//             byte serialRecibido[5];
//             bool responde = escanearID(currentID, serialRecibido, 600, 2);

//             if (!responde) {
//                 Serial.printf("ID 0x%02X: No responde.\n", currentID);
//                 listaIDsOcupadasScanActual[currentID - 1] = false;
//             } else {
//                 Serial.printf("ID 0x%02X: Responde con SN: %02X%02X%02X%02X%02X\n",
//                               currentID, serialRecibido[0], serialRecibido[1], serialRecibido[2], serialRecibido[3], serialRecibido[4]);
//                 listaIDsOcupadasScanActual[currentID - 1] = true;

//                 bool snExisteEnSPIFFS_flag = serialExistsInSPIFFS(serialRecibido);

//                 if (!snExisteEnSPIFFS_flag) {
//                     Serial.printf("SN %02X... no encontrado en SPIFFS. Tratando como nuevo en ID 0x%02X.\n", serialRecibido[0], currentID);
//                     bool elementoAgregadoConExito = false;
//                     for (int intento = 0; intento < 3; ++intento) {
//                         if (procesar_y_guardar_elemento_nuevo(currentID, serialRecibido)) {
//                             elementoAgregadoConExito = true;
//                             break;
//                         }
//                         Serial.printf("Fallo intento %d/3 procesar_y_guardar para ID 0x%02X. Reintentando...\n", intento + 1, currentID);
//                         delay(200);
//                     }
//                     if (elementoAgregadoConExito) {
//                         Serial.printf("Nuevo elemento en ID 0x%02X guardado.\n", currentID);
//                         changeFlag = true;
//                         Serial.println("Elemento nuevo agregado. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;
//                     } else {
//                         Serial.printf("ERROR: Fallaron todos los intentos para guardar nuevo elemento en ID 0x%02X.\n", currentID);
//                         listaIDsOcupadasScanActual[currentID - 1] = false;
//                     }
//                 } else {
//                     byte idSPIFFS = getIdFromSPIFFS(serialRecibido);
//                     Serial.printf("SN %02X... encontrado en SPIFFS. ID en SPIFFS: 0x%02X. ID actual respuesta: 0x%02X.\n", serialRecibido[0], idSPIFFS, currentID);
//                     if (idSPIFFS == 0xFF) {
//                         Serial.printf("ERROR CRÍTICO: SN %02X... existe en SPIFFS pero ID guardada es inválida (0xFF).\n", serialRecibido[0]);
//                         Serial.printf("Actualizando SPIFFS para SN %02X... con ID actual 0x%02X.\n", serialRecibido[0], currentID);
//                         actualizarIDenSPIFFS(serialRecibido, currentID);
//                         changeFlag = true;
//                         Serial.println("ID en SPIFFS (era 0xFF) actualizada. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;
//                     } else if (idSPIFFS == currentID) {
//                         Serial.printf("ID 0x%02X: Correcta y confirmada con SPIFFS.\n", currentID);
//                     } else {
//                         Serial.printf("ID 0x%02X: Desajuste. SPIFFS dice 0x%02X para SN %02X.... Actualizando SPIFFS a 0x%02X.\n", currentID, idSPIFFS, serialRecibido[0], currentID);
//                         actualizarIDenSPIFFS(serialRecibido, currentID);
//                         changeFlag = true;
//                         Serial.println("ID en SPIFFS actualizada. Forzando reinicio de escaneo 1-32.");
//                         goto reiniciar_barrido_1_32;
//                     }
//                 }
//             }
//             currentID++;
//         }

//     reiniciar_barrido_1_32:;
//         if (currentID <= 32 && changeFlag) {
//             Serial.printf("Interrupción en ID 0x%02X para reiniciar escaneo 1-32 debido a changeFlag.\n", currentID);
//         }
//         Serial.printf("--- Pasada de escaneo IDs 1-32 completada. ChangeFlag: %s ---\n", changeFlag ? "TRUE" : "FALSE");

//     } while (changeFlag);

//     Serial.println("=== Escaneo IDs 1-32 ESTABLE ===");
//     actualizarBarraProgreso(100);
//     delay(500);

//     // 🔶 PASO DE GESTIÓN DE 0xDD EN DOS FASES 🔶
//     Serial.println("--- Iniciando gestión de dispositivos en ID 0xDD (DEFAULT_DEVICE) en dos fases ---");
//     bool seProcesoUnDDYRequiereReinicio = false;
//     std::vector<std::array<byte, 5>> serialesDDProcesadosGlobalmente; // Para no procesar el mismo serial entre las dos fases

//     byte attachRequestTypes[] = {ELEM_FIRST_SPLIT_ATTACH_REQ, ELEM_LAST_SPLIT_ATTACH_REQ};
//     const char* attachRequestNames[] = {"FIRST_SPLIT_ATTACH_REQ", "LAST_SPLIT_ATTACH_REQ"};

//     for (int fase = 0; fase < 2; ++fase) {
//         Serial.printf("--- Fase %d de gestión 0xDD: Enviando %s ---\n", fase + 1, attachRequestNames[fase]);
//         iniciarEscaneoElemento(fase == 0 ? "Buscando 0xDD (1/2)..." : "Buscando 0xDD (2/2)...");
//         frameReceived = false;
//         delay(100);
//         send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
//                                               DEFAULT_DEVICE,
//                                               SPANISH_LANG,
//                                               attachRequestTypes[fase]));
//         if (attachRequestTypes[fase] == ELEM_LAST_SPLIT_ATTACH_REQ) {
//             Serial.println("Enviando segunda fase");
//             delay(1000);
//             send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
//                                               DEFAULT_DEVICE,
//                                               SPANISH_LANG,
//                                               attachRequestTypes[fase]));
//         }

//         Serial.printf("Esperando respuestas de dispositivos 0xDD durante 20 segundos (Fase %d)...\n", fase + 1);
//         unsigned long tiempoInicioEsperaDD = millis();
//         int ddResponsesProcessedThisPhase = 0;

//         while (millis() - tiempoInicioEsperaDD < 20000) {
//             if (esperar_respuesta(50)) {
//                 LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
//                 frameReceived = false;

//                 // CORRECCIÓN IMPORTANTE: El dispositivo responde *a* ELEM_XXX_ATTACH_REQ
//                 // devolviendo su ELEM_SERIAL_SECTOR.
//                 if (LEF.origin == DEFAULT_DEVICE &&
//                     LEF.function == F_RETURN_ELEM_SECTOR &&
//                     LEF.data.size() >= (1 + 5) &&
//                     LEF.data[0] == ELEM_FIRST_SPLIT_ATTACH_REQ || LEF.data[0] == ELEM_LAST_SPLIT_ATTACH_REQ) { // El dispositivo devuelve su serial

//                     byte serialRecibidoDD[5];
//                     memcpy(serialRecibidoDD, &LEF.data[1], 5);
//                     Serial.printf("Fase %d: Respuesta de 0xDD con SN: %02X%02X%02X%02X%02X\n",
//                                   fase + 1, serialRecibidoDD[0], serialRecibidoDD[1], serialRecibidoDD[2], serialRecibidoDD[3], serialRecibidoDD[4]);
                    
//                     ddResponsesProcessedThisPhase++;
//                     // actualizarBarraProgreso(...); // Podrías tener una barra de progreso por fase

//                     bool yaProcesadoGlobalmente = false;
//                     for (const auto& s_arr : serialesDDProcesadosGlobalmente) {
//                         if (memcmp(s_arr.data(), serialRecibidoDD, 5) == 0) {
//                             yaProcesadoGlobalmente = true;
//                             break;
//                         }
//                     }
//                     if (yaProcesadoGlobalmente) {
//                         Serial.printf("SN %02X... de 0xDD ya fue procesado en una fase anterior o en esta. Ignorando.\n", serialRecibidoDD[0]);
//                         continue;
//                     }

//                     bool reasignacionExitosaEsteDD = false;
//                     //byte idAsignadaOReafirmada = 0xFF; // No se usa fuera de logs, pero podría ser útil

//                     if (serialExistsInSPIFFS(serialRecibidoDD)) {
//                         byte idEnSpiffs = getIdFromSPIFFS(serialRecibidoDD);
//                         if (idEnSpiffs >= 1 && idEnSpiffs <= 32) {
//                             Serial.printf("Fase %d: SN %02X... (en 0xDD) ya existe en SPIFFS con ID 0x%02X. Reafirmando.\n", fase + 1, serialRecibidoDD[0], idEnSpiffs);
//                             send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, idEnSpiffs));
//                             delay(500);
//                             if (confirmarCambioID(idEnSpiffs)) {
//                                  Serial.printf("Fase %d: Confirmado: Elemento 0xDD (SN %02X...) restaurado a ID 0x%02X.\n", fase + 1, serialRecibidoDD[0], idEnSpiffs);
//                                  listaIDsOcupadasScanActual[idEnSpiffs - 1] = true;
//                                  reasignacionExitosaEsteDD = true;
//                                  //idAsignadaOReafirmada = idEnSpiffs;
//                             } else {
//                                 Serial.printf("Fase %d: ADVERTENCIA: No se pudo confirmar restauración a ID 0x%02X para 0xDD (SN %02X...).\n", fase + 1, idEnSpiffs, serialRecibidoDD[0]);
//                             }
//                         } else {
//                             Serial.printf("Fase %d: SN %02X... (en 0xDD) existe en SPIFFS con ID 0x%02X no válida. Buscando ID libre.\n", fase + 1, serialRecibidoDD[0], idEnSpiffs);
//                         }
//                     }
                    
//                     if (!reasignacionExitosaEsteDD) {
//                         byte idLibre = buscarPrimerIDLibre(listaIDsOcupadasScanActual);
//                         if (idLibre != 0xFF && idLibre != 0x00 && idLibre != DEFAULT_DEVICE) {
//                             Serial.printf("Fase %d: Asignando ID 0x%02X al elemento 0xDD con SN %02X...\n", fase + 1, idLibre, serialRecibidoDD[0]);
//                             send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, idLibre));
//                             delay(250);
//                             if (confirmarCambioID(idLibre)) {
//                                 Serial.printf("Fase %d: Confirmado: Elemento 0xDD (SN %02X...) ahora en ID 0x%02X.\n", fase + 1, serialRecibidoDD[0], idLibre);
//                                 listaIDsOcupadasScanActual[idLibre - 1] = true;
//                                 reasignacionExitosaEsteDD = true;
//                                 //idAsignadaOReafirmada = idLibre;
//                             } else {
//                                 Serial.printf("Fase %d: ADVERTENCIA: No se pudo confirmar cambio a ID 0x%02X para 0xDD (SN %02X...).\n", fase + 1, idLibre, serialRecibidoDD[0]);
//                             }
//                         } else {
//                             Serial.printf("Fase %d: ERROR: No hay ID libre válida (1-32) para 0xDD con SN %02X...\n", fase + 1, serialRecibidoDD[0]);
//                         }
//                     }

//                     if (reasignacionExitosaEsteDD) {
//                         seProcesoUnDDYRequiereReinicio = true;
//                         std::array<byte, 5> s_arr_mem; memcpy(s_arr_mem.data(), serialRecibidoDD, 5);
//                         serialesDDProcesadosGlobalmente.push_back(s_arr_mem); // Añadir a la lista global
//                     }

//                 }
//             }
//             delay(10);
//         } // Fin del while de 20 segundos para la fase actual
//         Serial.printf("--- Fin de la ventana de 20 segundos para dispositivos 0xDD (Fase %d) ---\n", fase + 1);
//         if (fase == 0 && !seProcesoUnDDYRequiereReinicio && ddResponsesProcessedThisPhase == 0) {
//              Serial.println("Fase 1 no procesó ningún DD ni obtuvo respuestas. Optimizando: saltando Fase 2.");
//              //break; // Opcional: Si la fase 1 no da señales, la fase 2 podría no ser necesaria.
//                      // Pero para asegurar que todos los dispositivos (que podrían responder a uno u otro) sean detectados,
//                      // es más seguro ejecutar ambas fases siempre. Considera esta optimización si es pertinente.
//         }
//     } // Fin del bucle de dos fases

//     if (seProcesoUnDDYRequiereReinicio) {
//         Serial.println("Se procesaron uno o más elementos 0xDD en las fases de attach. Reiniciando escaneo de sala completo...");
//         goto inicio_escanear_sala_completo;
//     } else {
//         Serial.println("No se procesaron elementos 0xDD que requieran reinicio en las fases de attach, o no hubo respuestas.");
//     }

//     loadElementsFromSPIFFS();
//     iniciarEscaneoElemento("Escaneo Finalizado");
//     actualizarBarraProgreso(100);
//     Serial.println("=== ✅ FIN ESCANEO DE SALA COMPLETO (SIN REINICIO POR 0xDD) ✅ ===");
// }

void BOTONERA_::escanearSala() {
inicio_escanear_sala_completo:
    Serial.println("=== 🚀 INICIO ESCANEO DE SALA (CON SPLIT ATTACH) 🚀 ===");
    scanDD = false;  // Para que escanearID (1-32) pida ELEM_SERIAL_SECTOR
    //iniciarEscaneoElemento("Escaneando Sala 1/32");

    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);

    loadElementsFromSPIFFS();

    bool listaIDsOcupadasScanActual[32];
    bool changeFlag;
    byte currentID;

    do {
        changeFlag = false;
        currentID = 1;
        memset(listaIDsOcupadasScanActual, false, sizeof(listaIDsOcupadasScanActual));

        Serial.println("--- Iniciando pasada de escaneo IDs 1-32 ---");

        while (currentID <= 32) {
            Serial.printf("🔍 Escaneando ID: 0x%02X (%d/32)\n", currentID, currentID);

            // 1) Actualizar barra de progreso e indicar ID actual
            char etiquetaID[16];
            snprintf(etiquetaID, sizeof(etiquetaID), "ID %d/32", currentID);
            actualizarBarraProgreso(currentID, 32, etiquetaID);

            // 2) Escanear
            byte serialRecibido[5];
            bool responde = escanearID(currentID, serialRecibido, 600, 2);

            if (!responde) {
                Serial.printf("ID 0x%02X: No responde.\n", currentID);
                listaIDsOcupadasScanActual[currentID - 1] = false;
            } else {
                Serial.printf(
                    "ID 0x%02X: Responde con SN: %02X%02X%02X%02X%02X\n",
                    currentID,
                    serialRecibido[0], serialRecibido[1],
                    serialRecibido[2], serialRecibido[3],
                    serialRecibido[4]
                );
                listaIDsOcupadasScanActual[currentID - 1] = true;

                bool snExisteEnSPIFFS_flag = serialExistsInSPIFFS(serialRecibido);
                if (!snExisteEnSPIFFS_flag) {
                    Serial.printf(
                        "SN %02X... no encontrado en SPIFFS. Tratando como nuevo en ID 0x%02X.\n",
                        serialRecibido[0], currentID
                    );
                    bool elementoAgregadoConExito = false;
                    for (int intento = 0; intento < 3; ++intento) {
                        if (procesar_y_guardar_elemento_nuevo(currentID, serialRecibido)) {
                            elementoAgregadoConExito = true;
                            break;
                        }
                        Serial.printf(
                            "Fallo intento %d/3 procesar_y_guardar para ID 0x%02X. Reintentando...\n",
                            intento + 1, currentID
                        );
                        delay(200);
                    }
                    if (elementoAgregadoConExito) {
                        Serial.printf("Nuevo elemento en ID 0x%02X guardado.\n", currentID);
                        changeFlag = true;
                        Serial.println("Elemento nuevo agregado. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;
                    } else {
                        Serial.printf(
                            "ERROR: Fallaron todos los intentos para guardar nuevo elemento en ID 0x%02X.\n",
                            currentID
                        );
                        listaIDsOcupadasScanActual[currentID - 1] = false;
                    }

                } else {
                    byte idSPIFFS = getIdFromSPIFFS(serialRecibido);
                    Serial.printf(
                        "SN %02X... encontrado en SPIFFS. ID en SPIFFS: 0x%02X. ID actual respuesta: 0x%02X.\n",
                        serialRecibido[0], idSPIFFS, currentID
                    );
                    if (idSPIFFS == 0xFF) {
                        Serial.printf(
                            "ERROR CRÍTICO: SN %02X... existe en SPIFFS pero ID guardada es inválida (0xFF).\n",
                            serialRecibido[0]
                        );
                        Serial.printf(
                            "Actualizando SPIFFS para SN %02X... con ID actual 0x%02X.\n",
                            serialRecibido[0], currentID
                        );
                        actualizarIDenSPIFFS(serialRecibido, currentID);
                        changeFlag = true;
                        Serial.println("ID en SPIFFS (era 0xFF) actualizada. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;

                    } else if (idSPIFFS == currentID) {
                        Serial.printf("ID 0x%02X: Correcta y confirmada con SPIFFS.\n", currentID);

                    } else {
                        Serial.printf(
                            "ID 0x%02X: Desajuste. SPIFFS dice 0x%02X para SN %02X.... Actualizando SPIFFS a 0x%02X.\n",
                            currentID, idSPIFFS, serialRecibido[0], currentID
                        );
                        actualizarIDenSPIFFS(serialRecibido, currentID);
                        changeFlag = true;
                        Serial.println("ID en SPIFFS actualizada. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;
                    }
                }
            }

            currentID++;
        }

    reiniciar_barrido_1_32:;
        if (currentID <= 32 && changeFlag) {
            Serial.printf(
                "Interrupción en ID 0x%02X para reiniciar escaneo 1-32 debido a changeFlag.\n",
                currentID
            );
        }
        Serial.printf(
            "--- Pasada de escaneo IDs 1-32 completada. ChangeFlag: %s ---\n",
            changeFlag ? "TRUE" : "FALSE"
        );

    } while (changeFlag);

    Serial.println("=== Escaneo IDs 1-32 ESTABLE ===");
    actualizarBarraProgreso(32, 32, "ID 32/32");
    delay(500);

    // 🔶 PASO DE GESTIÓN DE 0xDD EN DOS FASES 🔶
    Serial.println("--- Iniciando gestión de dispositivos en ID 0xDD (DEFAULT_DEVICE) en dos fases ---");
    bool seProcesoUnDDYRequiereReinicio = false;
    std::vector<std::array<byte, 5>> serialesDDProcesadosGlobalmente;

    byte attachRequestTypes[] = { ELEM_FIRST_SPLIT_ATTACH_REQ, ELEM_LAST_SPLIT_ATTACH_REQ };
    const char* attachRequestNames[] = { "FIRST_SPLIT_ATTACH_REQ", "LAST_SPLIT_ATTACH_REQ" };

    for (int fase = 0; fase < 2; ++fase) {
        Serial.printf(
            "--- Fase %d de gestión 0xDD: Enviando %s ---\n",
            fase + 1, attachRequestNames[fase]
        );
        iniciarEscaneoElemento(
            fase == 0 ? "Buscando 0xDD (1/2)..." : "Buscando 0xDD (2/2)..."
        );
        frameReceived = false;
        delay(100);

        // Envío de petición RF
        send_frame(frameMaker_REQ_ELEM_SECTOR(
            DEFAULT_BOTONERA, DEFAULT_DEVICE, SPANISH_LANG, attachRequestTypes[fase]
        ));
        if (attachRequestTypes[fase] == ELEM_LAST_SPLIT_ATTACH_REQ) {
            delay(1000);
            send_frame(frameMaker_REQ_ELEM_SECTOR(
                DEFAULT_BOTONERA, DEFAULT_DEVICE, SPANISH_LANG, attachRequestTypes[fase]
            ));
        }

        Serial.printf(
            "Esperando respuestas de dispositivos 0xDD durante 30 segundos (Fase %d)...\n",
            fase + 1
        );
        unsigned long tiempoInicioEsperaDD = millis();
        int ddResponsesProcessedThisPhase = 0;

        while (millis() - tiempoInicioEsperaDD < 30000) {
            if (esperar_respuesta(50)) {
                LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
                frameReceived = false;

                if (LEF.origin == DEFAULT_DEVICE &&
                    LEF.function == F_RETURN_ELEM_SECTOR &&
                    LEF.data.size() >= 6 &&
                    (LEF.data[0] == ELEM_FIRST_SPLIT_ATTACH_REQ ||
                     LEF.data[0] == ELEM_LAST_SPLIT_ATTACH_REQ)
                ) {
                    byte serialRecibidoDD[5];
                    memcpy(serialRecibidoDD, &LEF.data[1], 5);
                    Serial.printf(
                        "Fase %d: Respuesta de 0xDD con SN: %02X%02X%02X%02X%02X\n",
                        fase + 1,
                        serialRecibidoDD[0], serialRecibidoDD[1],
                        serialRecibidoDD[2], serialRecibidoDD[3],
                        serialRecibidoDD[4]
                    );
                    ddResponsesProcessedThisPhase++;

                    // Verificar duplicados globales
                    bool yaProcesadoGlobalmente = false;
                    for (const auto& s_arr : serialesDDProcesadosGlobalmente) {
                        if (memcmp(s_arr.data(), serialRecibidoDD, 5) == 0) {
                            yaProcesadoGlobalmente = true;
                            break;
                        }
                    }
                    if (yaProcesadoGlobalmente) {
                        Serial.printf(
                            "SN %02X... de 0xDD ya fue procesado. Ignorando.\n",
                            serialRecibidoDD[0]
                        );
                        continue;
                    }

                    bool reasignacionExitosaEsteDD = false;

                    // Si ya existe en SPIFFS, reafirmar ID
                    if (serialExistsInSPIFFS(serialRecibidoDD)) {
                        byte idEnSpiffs = getIdFromSPIFFS(serialRecibidoDD);
                        if (idEnSpiffs >= 1 && idEnSpiffs <= 32) {
                            Serial.printf(
                                "Fase %d: SN %02X... ya existe con ID 0x%02X. Reafirmando.\n",
                                fase + 1, serialRecibidoDD[0], idEnSpiffs
                            );
                            send_frame(frameMaker_SET_ELEM_ID(
                                DEFAULT_BOTONERA, DEFAULT_DEVICE, idEnSpiffs
                            ));
                            delay(500);
                            if (confirmarCambioID(idEnSpiffs)) {
                                Serial.printf(
                                    "Fase %d: Confirmado: 0xDD restaurado a ID 0x%02X.\n",
                                    fase + 1, idEnSpiffs
                                );
                                listaIDsOcupadasScanActual[idEnSpiffs - 1] = true;
                                reasignacionExitosaEsteDD = true;
                            } else {
                                Serial.printf(
                                    "Fase %d: ADVERTENCIA: No se pudo confirmar restauración.\n",
                                    fase + 1
                                );
                            }
                        } else {
                            Serial.printf(
                                "Fase %d: ID en SPIFFS no válida (0x%02X). Buscando libre.\n",
                                fase + 1, idEnSpiffs
                            );
                        }
                    }

                    // Si no se reafirmó, asignar primer ID libre
                    if (!reasignacionExitosaEsteDD) {
                        byte idLibre = buscarPrimerIDLibre(listaIDsOcupadasScanActual);
                        if (idLibre != 0xFF && idLibre != 0x00 && idLibre != DEFAULT_DEVICE) {
                            Serial.printf(
                                "Fase %d: Asignando ID 0x%02X al 0xDD.\n",
                                fase + 1, idLibre
                            );
                            send_frame(frameMaker_SET_ELEM_ID(
                                DEFAULT_BOTONERA, DEFAULT_DEVICE, idLibre
                            ));
                            delay(250);
                            if (confirmarCambioID(idLibre)) {
                                Serial.printf(
                                    "Fase %d: Confirmado: 0xDD ahora en ID 0x%02X.\n",
                                    fase + 1, idLibre
                                );
                                listaIDsOcupadasScanActual[idLibre - 1] = true;
                                reasignacionExitosaEsteDD = true;
                            } else {
                                Serial.printf(
                                    "Fase %d: ADVERTENCIA: No se pudo confirmar cambio.\n",
                                    fase + 1
                                );
                            }
                        } else {
                            Serial.printf(
                                "Fase %d: ERROR: No hay ID libre válida.\n",
                                fase + 1
                            );
                        }
                    }

                    if (reasignacionExitosaEsteDD) {
                        seProcesoUnDDYRequiereReinicio = true;
                        std::array<byte, 5> s_arr_mem;
                        memcpy(s_arr_mem.data(), serialRecibidoDD, 5);
                        serialesDDProcesadosGlobalmente.push_back(s_arr_mem);
                    }
                }
            }
            delay(10);
        }  // while 20 s por fase

        Serial.printf(
            "--- Fin de la ventana de 30 segundos para dispositivos 0xDD (Fase %d) ---\n",
            fase + 1
        );
        if (fase == 0 && !seProcesoUnDDYRequiereReinicio && ddResponsesProcessedThisPhase == 0) {
            Serial.println("Fase 1 no procesó ningún DD. Saltando Fase 2 opcionalmente.");
        }
    }  // for fases

    if (seProcesoUnDDYRequiereReinicio) {
        Serial.println("Se procesaron 0xDD que requieren reinicio. Reiniciando escaneo completo...");
        goto inicio_escanear_sala_completo;
    } else {
        Serial.println("No hubo 0xDD que requieran reinicio.");
    }

    loadElementsFromSPIFFS();
    iniciarEscaneoElemento("Escaneo Finalizado");
    actualizarBarraProgreso(32, 32, nullptr);
    Serial.println("=== ✅ FIN ESCANEO DE SALA COMPLETO (SIN REINICIO POR 0xDD) ✅ ===");
}


// Función para mostrar texto multilínea ajustado al ancho máximo sin romper palabras
static void mostrarTextoAjustado(TFT_eSPI& tft,
                                 const char* texto,
                                 uint16_t xCentro,
                                 uint16_t yInicio,
                                 uint16_t maxWidth)
{
    // Configuración de texto
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    
    const uint16_t lineHeight   = tft.fontHeight();
    const uint16_t screenHeight = 128;  // altura de pantalla fija
    const char*   ptr           = texto;
    uint16_t      y             = yInicio;

    while (*ptr && y + lineHeight <= screenHeight) {
        const char*  scan       = ptr;
        size_t       lastSpace  = 0;
        size_t       charsCount = 0;
        uint16_t     w           = 0;

        // Avanzamos hasta que el ancho supere el máximo o lleguemos al final
        while (*scan) {
            ++charsCount;
            if (charsCount >= 128) break; // límite de búfer de medición
            char buf[128];
            memcpy(buf, ptr, charsCount);
            buf[charsCount] = '\0';
            w = tft.textWidth(buf);
            if (w > maxWidth) {
                --charsCount;
                break;
            }
            if (*scan == ' ') {
                lastSpace = charsCount;
            }
            ++scan;
        }

        // Determinar cuántos caracteres dibujar esta línea
        size_t lineLen;
        if (*scan == '\0') {
            // todo el texto cabe
            lineLen = charsCount;
        }
        else if (lastSpace > 0) {
            // rompemos en el último espacio
            lineLen = lastSpace;
        }
        else {
            // no hay espacio: rompemos donde esté
            lineLen = charsCount;
        }

        // Extraer y dibujar la línea
        char lineBuf[128];
        memcpy(lineBuf, ptr, lineLen);
        lineBuf[lineLen] = '\0';
        tft.drawString(lineBuf, xCentro, y);

        // Avanzar al siguiente bloque de texto
        ptr += lineLen;
        while (*ptr == ' ') ++ptr;  // saltar espacios iniciales
        y   += lineHeight;
    }
}


void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
    // 1) Limpiar pantalla y marco
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);

    // 2) Mostrar mensaje inicial (centrado, multiline)
    mostrarTextoAjustado(tft, mensajeInicial, 64, 30, 120);

    // breve pausa para dar tiempo a que el usuario lo lea
    delay(100);
}

void BOTONERA_::actualizarBarraProgreso(int pasoActual,
                                        int pasosTotales,
                                        const char* etiqueta)
{
    const int  xBarra    = 14;
    const int  yBarra    = 60;
    const int  anchoMax  = 100;
    const int  altoBarra = 10;
    const int  margen    = 4;
    const int  screenW   = 128;
    const int  fontH     = tft.fontHeight();

    // 1) Borrar solo la región de la barra + texto de porcentaje
    tft.fillRect(xBarra - margen,
                 yBarra - margen,
                 anchoMax + 2*margen,
                 altoBarra + fontH*2 + 2*margen,
                 TFT_BLACK);

    // 2) Borrar zona de la etiqueta (“ID X/32” o “Sector Y/Z”)
    if (etiqueta) {
        int etiquetaY = yBarra - fontH - 6;
        // limpiamos todo el ancho para no dejar restos
        tft.fillRect(0,
                     etiquetaY,
                     screenW,
                     fontH + 4,
                     TFT_BLACK);
    }

    // 3) Dibujar fondo de barra y avance
    tft.fillRoundRect(xBarra, yBarra, anchoMax, altoBarra, 5, TFT_DARKGREY);
    int pixelesProg = (int)(anchoMax * (pasoActual - 1) / float(pasosTotales));
    tft.fillRoundRect(xBarra, yBarra, pixelesProg, altoBarra, 5, TFT_BLUE);

    // 4) Mostrar porcentaje
    char bufPct[8];
    int pct = int((pasoActual - 1) * 100.0f / pasosTotales);
    snprintf(bufPct, sizeof(bufPct), "%d%%", pct);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(bufPct,
                   xBarra + anchoMax/2,
                   yBarra + altoBarra + fontH/2);

    // 5) Mostrar etiqueta justo encima de la barra
    if (etiqueta) {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(etiqueta,
                       screenW/2,                // centrado horizontal
                       yBarra - fontH - 4);      // justo encima
    }
}


