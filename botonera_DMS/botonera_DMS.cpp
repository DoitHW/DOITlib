#include <botonera_DMS/botonera_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <encoder_handler/encoder_handler.h>
#include <display_handler/display_handler.h>




BOTONERA_::BOTONERA_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_BOTONERA);
        }

void BOTONERA_::botonera_begin(){

    
            
}


void BOTONERA_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }

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
    case ELEM_ICON_ROW_63_SECTOR:{

    break;
    }
    default:
    
        break;
    }
}

byte BOTONERA_::buscar_elemento_nuevo() {
    const int max_reintentos = 5;  
    bool error_detectado = false;

    // Crear la estructura en el heap de forma segura
    INFO_PACK_T *aux = new INFO_PACK_T;
    if (!aux) {
        Serial.println("Error: Memoria insuficiente.");
        return 0;
    }
    memset(aux, 0, sizeof(INFO_PACK_T));  // ✅ Memoria completamente inicializada

    // ✅ Petición inicial del serial antes del bucle principal
    send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, 0x45, SPANISH_LANG, ELEM_SERIAL_SECTOR));
    frameReceived = false;
    unsigned long startTime = millis();

    while (!frameReceived && millis() - startTime < 2000) {
        // Esperar hasta recibir una trama válida
    }

    if (frameReceived) {
        frameReceived = false;
        LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
        
        if (LEF.data.size() >= 3 && LEF.data[0] == ELEM_SERIAL_SECTOR) {
            memcpy(aux->serialNum, &LEF.data[1], 2);
            
            // ✅ Comprobación en SPIFFS
            if (serialExistsInSPIFFS(aux->serialNum)) {
                Serial.println("✅ Elemento ya existente. Abortando.");
                delete aux;
                return 2;
            }
            Serial.println("✅ Número de serie nuevo. Continuando búsqueda.");
        } else {
            Serial.println("❌ Error: Trama del serial recibida inválida.");
            delete aux;
            return 0;
        }
    } else {
        Serial.println("❌ ERROR: No se recibió respuesta al pedir serial.");
        delete aux;
        return 0;
    }

    // ✅ Continuar con la búsqueda de sectores
    for (int sector = ELEM_NAME_SECTOR; sector <= ELEM_ICON_ROW_63_SECTOR; ++sector) {
        int intentos = 0;
        bool sector_completado = false;

        while (intentos < max_reintentos && !sector_completado) {
            send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, 0x45, SPANISH_LANG, sector));
            frameReceived = false;
            startTime = millis();

            // ✅ Modificado: Esperar solo hasta recibir la trama
            while (!frameReceived && millis() - startTime < 2000) {
                // Esperando recepción sin bloquear
            }

            if (frameReceived) {
                frameReceived = false;
                LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

                // ✅ Validación del sector recibido
                if (LEF.data.size() > 1 && LEF.data[0] == sector) {
                    switch (sector) {
                        case ELEM_NAME_SECTOR:
                            memcpy(aux->name, &LEF.data[1], min(sizeof(aux->name), LEF.data.size() - 1));
                            break;
                        case ELEM_DESC_SECTOR:
                            memcpy(aux->desc, &LEF.data[1], min(sizeof(aux->desc), LEF.data.size() - 1));
                            break;
                        case ELEM_SERIAL_SECTOR:
                            memcpy(aux->serialNum, &LEF.data[1], min(sizeof(aux->serialNum), LEF.data.size() - 1));
                            break;
                        case ELEM_ID_SECTOR:
                            aux->ID = LEF.data[1];
                            break;
                        case ELEM_CMODE_SECTOR:
                            aux->currentMode = LEF.data[1];
                            break;
                        default:
                            if (sector >= ELEM_MODE_0_NAME_SECTOR && sector <= ELEM_MODE_15_FLAG_SECTOR) {
                                int modeIndex = (sector - ELEM_MODE_0_NAME_SECTOR) / 3;
                                int fieldIndex = (sector - ELEM_MODE_0_NAME_SECTOR) % 3;
                                if (fieldIndex == 0)
                                    memcpy(aux->mode[modeIndex].name, &LEF.data[1], min(sizeof(aux->mode[modeIndex].name), LEF.data.size() - 1));
                                else if (fieldIndex == 1)
                                    memcpy(aux->mode[modeIndex].desc, &LEF.data[1], min(sizeof(aux->mode[modeIndex].desc), LEF.data.size() - 1));
                                else if (fieldIndex == 2)
                                    memcpy(aux->mode[modeIndex].config, &LEF.data[1], min(sizeof(aux->mode[modeIndex].config), LEF.data.size() - 1));
                            }
                            // ✅ Almacenamiento seguro del icono
                            else if (sector >= ELEM_ICON_ROW_0_SECTOR && sector <= ELEM_ICON_ROW_63_SECTOR) {
                                int rowIndex = sector - ELEM_ICON_ROW_0_SECTOR;
                                if (LEF.data.size() == 129) {
                                    for (int col = 0; col < ICON_COLUMNS; ++col) {
                                        uint8_t msb = LEF.data[2 * col + 1];
                                        uint8_t lsb = LEF.data[2 * col + 2];
                                        aux->icono[rowIndex][col] = (uint16_t(msb) << 8) | lsb;
                                    }
                                } else {
                                    Serial.printf("❌ Error: Trama incompleta en fila %d.\n", rowIndex);
                                }
                            }
                            break;
                    }
                    Serial.printf("Datos correctamente recibidos y guardados para sector: %d\n", sector);
                    sector_completado = true;  // ✅ Salir sin esperar más
                }
            }
            intentos++;
        }

        if (!sector_completado) {
            Serial.printf("❌ Error: Sector %d no respondió tras %d intentos.\n", sector, max_reintentos);
            error_detectado = true;
            break;
        }
    }

    // ✅ Finalización y limpieza de memoria
    if (error_detectado) {
        Serial.println("❌ ERROR: Fallo de comunicación. Liberando memoria.");
        delete aux;
        return 0;
    } else {
        Serial.println("✅ Elemento nuevo encontrado. Almacenando.");
        //anadir_elemento_nuevo(aux);  // Guardar en memoria SPIFFS
        print_info_pack(aux);
        delete aux;
        return 1;
    }
}


byte BOTONERA_::anadir_elemento_nuevo(const INFO_PACK_T *infoPack){
    INFO_PACK_T *aux = new INFO_PACK_T;

    
   
}

void BOTONERA_::print_info_pack(const INFO_PACK_T *infoPack) {
    Serial.println("---- INFO DEL ELEMENTO ALMACENADO ----");
    
    Serial.print("Nombre: ");
    Serial.println((char*)infoPack->name);

    Serial.print("Descripción: ");
    Serial.println((char*)infoPack->desc);

    Serial.print("Número de Serie: ");
    Serial.printf("0x%02X%02X%02X%02X%02X\n", infoPack->serialNum[0], infoPack->serialNum[1], infoPack->serialNum[2], infoPack->serialNum[3], infoPack->serialNum[4]);

    Serial.print("ID: ");
    Serial.println(infoPack->ID);

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

bool BOTONERA_::serialExistsInSPIFFS(byte serialNum[2]) {
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
        Serial.println("Antes del file.read");

        // Validar que el archivo tenga suficiente tamaño antes de leer
        if (file.size() >= OFFSET_SERIAL + 2) {
            file.seek(OFFSET_SERIAL);
            file.read(tempPack->serialNum, 2);
            Serial.printf("Serial leído: %02X%02X\n", tempPack->serialNum[0], tempPack->serialNum[1]);

            if (tempPack->serialNum[0] == serialNum[0] && tempPack->serialNum[1] == serialNum[1]) {
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




