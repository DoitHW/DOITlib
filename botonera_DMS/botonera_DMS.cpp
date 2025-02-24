#include <botonera_DMS/botonera_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <encoder_handler/encoder_handler.h>
#include <display_handler/display_handler.h>





BOTONERA_::BOTONERA_() : ELEMENT_() {
            set_type(TYPE_BOTONERA);
        }

void BOTONERA_::botonera_begin(){

    
            
}

void BOTONERA_::printFrameInfo(LAST_ENTRY_FRAME_T LEF) {
    Serial.println("\n==== 📨 Trama Recibida 📨 ====");

    // Determinar origen
    String origenStr;
    if (LEF.origin == 0xDB) origenStr = "BOTONERA";
    else if (LEF.origin == 0xDC) origenStr = "CONSOLA";
    else if (LEF.origin == 0xFF) origenStr = "BROADCAST";
    else origenStr = "DESCONOCIDO";

    Serial.printf("📌 Origen: %s (0x%02X)\n", origenStr.c_str(), LEF.origin);

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
    Serial.println("Inicio de RX_main_handler");
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                                Serial.println("Stack restante: " + String(stackSize));
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
                                                                    Serial.println("Stack restante al final: " + String(stackSize));
                                                                #endif
}


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
    frameReceived = false;

    for (int intento = 0; intento < max_reintentos; intento++) {
        // Mostrar algo en la interfaz de usuario, p.ej. "Escaneando..."
        iniciarEscaneoElemento("Buscando elementos...");

        // Petición de ELEM_SERIAL_SECTOR al DEFAULT_DEVICE
        send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                              DEFAULT_DEVICE,
                                              SPANISH_LANG,
                                              ELEM_SERIAL_SECTOR));

        frameReceived = false;
        unsigned long startTime = millis();

        // Espera hasta 2.5s a que frameReceived se ponga en true
        while (!frameReceived && (millis() - startTime < 2500)) {
            delay(10);
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
    delay(10);

    Serial.println("🆙🆙🆙🆙 ID reasignada");
}

void BOTONERA_::validar_elemento() {
    // Llama a validar_serial(), que retorna:
    //   0 -> No se recibió respuesta (error)
    //   1 -> Elemento existente en SPIFFS
    //   2 -> Elemento no existente (nuevo)
    byte resultado = validar_serial();
    switch (resultado) {
        case 0: {
            // Error al leer el serial
            Serial.println("❌ No se obtuvo respuesta");
            mostrarMensajeTemporal(0, 3000);
            return;
        }

        case 1: {
            // El elemento ya existe. 
            // 1) Recuperar ID desde SPIFFS
            byte existingID = getIdFromSPIFFS(lastSerial);
            if (existingID == 0xFF) {
                // Algo falló al buscar la ID
                Serial.println("❌ No se encontró la ID en SPIFFS, aunque el serial existe.");
                mostrarMensajeTemporal(0, 3000);
                return;
            }

            // 2) Reasignar la ID al elemento que responde en la ID por defecto
            lastAssignedID = existingID; 
            send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA,
                                              DEFAULT_DEVICE,
                                              lastAssignedID));
            delay(500);
            // 3) Verificar confirmación de ID (petición de ELEM_ID_SECTOR a la new ID)
            if (confirmarCambioID(lastAssignedID)) {
                // Éxito: ID confirmada
                Serial.printf("✅ ID reasignada y confirmada: 0x%02X\n", lastAssignedID);
                // Mostramos mensaje "elemento existente" => 1
                mostrarMensajeTemporal(1, 3000);
            } else {
                // Falló la confirmación 
                Serial.println("❌ Falló la confirmación de la ID reasignada.");
                mostrarMensajeTemporal(0, 3000);
            }

            return;
        }

        case 2: {
            // Elemento nuevo. 
            // 1) Obtenemos la próxima ID disponible
            lastAssignedID = getNextAvailableID();

            // 2) Asignamos esa ID al elemento que responde actualmente en DEFAULT_DEVICE
            Serial.println("Cambiando de ID en caso 2");
            send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA,
                                              DEFAULT_DEVICE,
                                              lastAssignedID));
            delay(500);
            // 3) Podemos verificar si deseamos confirmarlo también:
            if (!confirmarCambioID(lastAssignedID)) {
                Serial.println("❌ Falló la confirmación de la nueva ID en elemento nuevo.");
                mostrarMensajeTemporal(0, 3000);
                finalizarEscaneoElemento();
                return;
            }
            Serial.printf("✅ Nueva ID asignada y confirmada: 0x%02X\n", lastAssignedID);
            iniciarEscaneoElemento("Agregando ...");
            actualizarBarraProgreso(0);
            // 4) Descargamos la información completa del elemento (nombre, desc, modos...)
            delay(500);
            bool exito = procesar_y_guardar_elemento_nuevo(lastAssignedID);

            // 5) Resultado final: si todo fue bien => mostrarMensajeTemporal(2, 3000) 
            //                      si no => mensaje de error
            if (exito) {
                mostrarMensajeTemporal(2, 3000);
            } else {
                mostrarMensajeTemporal(0, 3000);
            }
            finalizarEscaneoElemento();
            return;
        }
    }
}

byte BOTONERA_::getIdFromSPIFFS(byte *serial) {
  
    fs::File root = SPIFFS.open("/");
    fs::File file = root.openNextFile();

    while (file) {
        // Leemos el serial en la posición OFFSET_SERIAL
        file.seek(OFFSET_SERIAL);
        byte existingSerial[5];
        file.read(existingSerial, 5);

        // Comparamos con el serial pasado como parámetro
        if (memcmp(existingSerial, serial, 5) == 0) {
            // Leemos la ID
            file.seek(OFFSET_ID);
            byte existingID;
            file.read(&existingID, 1);

            file.close();
            root.close();
            return existingID; // Devuelve la ID encontrada
        }

        file = root.openNextFile();
    }

    root.close();
    return 0xFF; // Indica que no se encontró
}

bool BOTONERA_::confirmarCambioID(byte nuevaID) {
    // Petición de ELEM_ID_SECTOR al "nuevaID"
    send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                          nuevaID,
                                          SPANISH_LANG,
                                          ELEM_ID_SECTOR));

    if (!esperar_respuesta(2000)) {
        Serial.println("No llegó respuesta de ELEM_ID_SECTOR");
        return false;
    }

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
    frameReceived = false;

    while (millis() - startTime < timeout) {
        if (frameReceived) {
            return true; // Respuesta recibida
        }
        delay(10);
        // Aquí podrías hacer yield() en ESP8266/ESP32
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
    for (int sector = ELEM_NAME_SECTOR; sector <= ELEM_ICON_ROW_63_SECTOR; sector++) {
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
    const int max_reintentos = 3;

    for (int intento = 0; intento < max_reintentos; intento++) {
        // Petición del sector al "targetID"
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
                actualizarBarraProgreso((sector * 100.0) / ELEM_ICON_ROW_63_SECTOR);
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
    // Validación del sistema de archivos antes de abrir
    if (!SPIFFS.begin(true)) {
        Serial.println("Error: SPIFFS no pudo montarse correctamente.");
        return false;
    }
  
    fs::File root = SPIFFS.open("/");
    
    if (!root || !root.isDirectory()) {
        Serial.println("Error: No se pudo abrir el directorio raíz de SPIFFS.");
        return false;
    }

    fs::File file = root.openNextFile();

    // Evitar uso excesivo de pila: Crear tempPack en el heap
    INFO_PACK_T *tempPack = new (std::nothrow) INFO_PACK_T;
    if (!tempPack) {
        Serial.println("Error: No se pudo asignar memoria para tempPack.");
        return false;
    }

    while (file) {
        // Validar que el archivo tenga suficiente tamaño antes de leer
        if (file.size() >= OFFSET_SERIAL + 5) {
            file.seek(OFFSET_SERIAL);
            file.read(tempPack->serialNum, 5);
            Serial.printf("Serial leído: %02X%02X%02X%02X%02X\n", tempPack->serialNum[0], tempPack->serialNum[1], tempPack->serialNum[2], tempPack->serialNum[3], tempPack->serialNum[4]);

            if (tempPack->serialNum[0] == serialNum[0] && tempPack->serialNum[1] == serialNum[1] && tempPack->serialNum[2] == serialNum[2] && tempPack->serialNum[3] == serialNum[3] && tempPack->serialNum[4] == serialNum[4]) {
                Serial.println("Número de serie encontrado.");
                delete tempPack;
                file.close();
                return true;  
            }
        } else {
            Serial.println("Error: El archivo es demasiado pequeño para contener un número de serie.");
        }

        file.close();  // Cerrar archivo antes de abrir el siguiente
        file = root.openNextFile();
    }

    Serial.println("Número de serie NO encontrado.");
    delete tempPack;
    return false;
}

// void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
//     tft.fillScreen(TFT_BLACK);
//     dibujarMarco(TFT_WHITE);
    
//     tft.setTextColor(TFT_WHITE);
//     tft.setTextDatum(MC_DATUM);
//     tft.setTextFont(2); // Usa Font 2
//     tft.drawString(mensajeInicial, 64, 30);
//     delay(100);
    
// }

void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM); // Cada línea centrada
    tft.setTextFont(2);
    
    const uint16_t maxWidth = 120; // Ancho máximo por línea con margen
    const uint8_t lineHeight = tft.fontHeight(); // Altura de la fuente
    uint16_t yPos = 30; // Posición vertical inicial
    const uint16_t screenHeight = 128;

    const char* start = mensajeInicial;
    const char* end = start;
    const char* lastSpace = nullptr;

    while (*start) {
        end = start;
        lastSpace = nullptr;
        uint16_t currentWidth = 0;

        // Encontrar el punto de corte para la línea actual
        while (*end) {
            if (*end == ' ') lastSpace = end;

            // Calcular ancho hasta el carácter actual
            char temp[21] = {0};
            strncpy(temp, start, end - start + 1);
            currentWidth = tft.textWidth(temp);

            if (currentWidth > maxWidth) break;
            end++;
        }

        // Ajustar el corte al último espacio si es posible
        if (lastSpace && lastSpace > start && lastSpace < end) {
            end = lastSpace;
        } else if (end == start) {
            end++; // Caso extremo: un carácter muy ancho
        } else if (*end) {
            end--; // Retroceder si se superó el ancho
        }

        // Extraer y dibujar la línea
        char line[128] = {0};
        strncpy(line, start, end - start);
        tft.drawString(line, 64, yPos);

        yPos += lineHeight;
        if (yPos + lineHeight > screenHeight) break; // Verificar espacio vertical

        start = (*end == ' ') ? end + 1 : end; // Saltar espacios si los hay
    }
    
    delay(100);
}


void BOTONERA_::actualizarBarraProgreso(float progreso) {
    int barraAnchoMax = 100;
    int barraAlto = 10;
    int barraProgreso = (int)(barraAnchoMax * progreso / 100.0);

    // Dibujar barra de fondo
    tft.fillRoundRect(14, 60, barraAnchoMax, barraAlto, 5, TFT_DARKGREY);
    
    // Dibujar barra de progreso
    tft.fillRoundRect(14, 60, barraProgreso, barraAlto, 5, TFT_BLUE);

    // Mostrar porcentaje
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2); // Usa Font 2
    tft.drawFloat(progreso, 0, 64, 90);
    tft.drawString("%", 90, 90);
}

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

void BOTONERA_::mostrarMensajeTemporal(int respuesta, int dTime) {
    // Borrar la pantalla y dibujar el marco
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);

    // Determinar color y mensajes
    uint32_t colorTexto;
    const char* mensajePrincipal;
    const char* mensajeSecundario;

    if (respuesta == 0) {
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Sin respuesta";
    } else if (respuesta == 2) {
        colorTexto = TFT_GREEN;
        mensajePrincipal = "NUEVO";
        mensajeSecundario = "  Elemento nuevo  Agregado";
    } else if (respuesta == 1) {
        colorTexto = TFT_YELLOW;
        mensajePrincipal = "ADVERTENCIA";
        mensajeSecundario = "Elemento existente Reasignando ID";
    } else {
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

    if (respuesta == 0 || respuesta == 3) {  // Error
        tft.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_RED);
        tft.drawLine(58, 104, 70, 116, TFT_WHITE);
        tft.drawLine(70, 104, 58, 116, TFT_WHITE);
    } 
    else if (respuesta == 1) {  // Éxito
        tft.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_GREEN);
        tft.drawLine(58, 110, 62, 114, TFT_WHITE);
        tft.drawLine(62, 114, 70, 106, TFT_WHITE);
    } 
    else if (respuesta == 2) {  // Advertencia

    // === Triángulo amarillo ===
    // 'iconSize' representará aproximadamente la "mitad" de la base del triángulo.
    // Para un triángulo equilátero, la altura es base * (√3 / 2) ≈ base * 0.866
    // Si 'iconSize' es 10, entonces la base es 20, y la altura ~17.
    
    int triangleSize   = iconSize * 2;           // base del triángulo
    float triangleHeight = triangleSize * 0.866; // altura (base * √3/2)
    
    tft.fillTriangle(
        iconCenterX,                        // Vértice superior (X)
        iconCenterY - (triangleHeight / 2), // Vértice superior (Y)
        iconCenterX - (triangleSize / 2),   // Vértice inferior izquierdo (X)
        iconCenterY + (triangleHeight / 2), // Vértice inferior izquierdo (Y)
        iconCenterX + (triangleSize / 2),   // Vértice inferior derecho (X)
        iconCenterY + (triangleHeight / 2), // Vértice inferior derecho (Y)
        TFT_YELLOW
    );

    // === Símbolo de exclamación (!) en negro ===
    // Lo dividimos en 2 partes: la barra y el punto.
    // Ajustamos la posición para que quede centrada en el triángulo.
    
    // Altura total del signo de exclamación (ajusta a tu gusto):
    int exclamationHeight = (int)(triangleHeight * 0.5); 
    // Coordenada Y donde comienza la barra vertical
    int barY = iconCenterY - (exclamationHeight / 2);
    
    // 1) Barra vertical (2 px de ancho)
    tft.fillRect(
        iconCenterX - 1,   // Para centrar la barra en X
        barY + 1, 
        2,                 // Ancho de la barra
        exclamationHeight - 4,  // Alto de la barra (reservamos espacio para el punto)
        TFT_BLACK
    );

    // 2) Punto (pequeño rectángulo o círculo)
    // Lo situamos debajo de la barra, dejando 2 px de separación
    int dotSize = 3; 
    tft.fillRect(
        iconCenterX - 1, 
        barY + (exclamationHeight - 4) + 3, 
        dotSize, 
        dotSize, 
        TFT_BLACK
    );
    }

    // Esperar y regresar al menú
    delay(dTime);
    drawCurrentElement();
}

byte BOTONERA_::getNextAvailableID() {
    byte nextID = 0x01;

    fs::File root = SPIFFS.open("/");
    fs::File file = root.openNextFile();

    // Escanea todos los archivos y busca la ID más alta para incrementarla
    while (file) {
        byte existingID;
        file.seek(OFFSET_ID);
        file.read(&existingID, 1);

        if (existingID >= nextID) {
            nextID = existingID + 1; 
        }
        file = root.openNextFile();
    }
    root.close();

    Serial.printf("✅ Nueva ID disponible: %d\n", nextID);
    return nextID;
}


