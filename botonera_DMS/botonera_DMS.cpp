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
        Serial.println("❌ Error: Memoria insuficiente.");
        return 0;
    }
    memset(aux, 0, sizeof(INFO_PACK_T));  // ✅ Memoria completamente inicializada

    iniciarEscaneoElemento("Escaneando ...");
    byte tempID = DEFAULT_DEVICE;
    // ✅ Petición inicial del serial antes del bucle principal
    send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, tempID, SPANISH_LANG, ELEM_SERIAL_SECTOR));
    frameReceived = false;
    unsigned long startTime = millis();

    while (!frameReceived && millis() - startTime < 2500) {
        // Esperar hasta recibir una trama válida
    }

    if (frameReceived) {
        frameReceived = false;
        LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
        
        if (LEF.data.size() >= 3 && LEF.data[0] == ELEM_SERIAL_SECTOR) {
            memcpy(aux->serialNum, &LEF.data[1], 5);
            
            // ✅ Comprobación en SPIFFS
            if (serialExistsInSPIFFS(aux->serialNum)) {
                Serial.println("✅ Elemento ya existente. Abortando.");
                delete aux;
                return 2;
            }
            Serial.println("🔍  Número de serie nuevo. Continuando búsqueda. 🔍 ");
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
            send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, tempID, SPANISH_LANG, sector));
            frameReceived = false;
            startTime = millis();

            // ✅ Modificado: Esperar solo hasta recibir la trama
            while (!frameReceived && millis() - startTime < 2500) {
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
                    float progreso = (sector * 100.0) / ELEM_ICON_ROW_63_SECTOR;
                    actualizarBarraProgreso(progreso);
                    Serial.printf("✅ Datos correctamente recibidos y guardados para sector: %d\n", sector);
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
        finalizarEscaneoElemento();
        print_info_pack(aux);
        anadir_elemento_nuevo(aux);
        delete aux;
        return 1;
    }
}


byte BOTONERA_::anadir_elemento_nuevo(const INFO_PACK_T *infoPack) {
    if (!infoPack) {
        Serial.println("❌ Error: El paquete de información es nulo.");
        return 0;
    }

    // Crear una copia temporal del número de serie para pasarla como byte*
    byte serialNumCopy[5];
    memcpy(serialNumCopy, infoPack->serialNum, sizeof(serialNumCopy));

    // Verificar si ya existe un elemento con el mismo número de serie usando la función existente
    if (serialExistsInSPIFFS(serialNumCopy)) {
        Serial.println("⚠️ Número de serie ya existente.");
        // Abrir el archivo correspondiente para comprobar la ID
        fs::File root = SPIFFS.open("/");
        fs::File file = root.openNextFile();
        while (file) {
            byte existingSerialNum[5];
            file.seek(OFFSET_SERIAL);
            file.read(existingSerialNum, 5);
            if (memcmp(existingSerialNum, serialNumCopy, 5) == 0) {
                file.seek(OFFSET_ID);
                byte existingID;
                file.read(&existingID, 1);
                if (existingID == 0xDD) {
                    byte newID = getNextAvailableID();
                    file.seek(OFFSET_ID);
                    file.write(&newID, 1);
                    Serial.printf("✅ ID reasignada a: %d\n", newID);
                }
                file.close();
                return 2;
            }
            file = root.openNextFile();
        }
    }

    // Generar un nombre de archivo único si el nombre ya existe
    String uniqueFileName = generateUniqueFileName((char*)infoPack->name);

    // Abrir archivo para escritura
    fs::File file = SPIFFS.open(uniqueFileName, "w");
    if (!file) {
        Serial.println("❌ Error al abrir archivo en SPIFFS.");
        return 0;
    }

    // Guardar información del elemento directamente
    file.write((const uint8_t*)infoPack->name, 24);
    file.write((const uint8_t*)infoPack->desc, 192);
    file.write((const uint8_t*)infoPack->serialNum, 5);
    file.write(&infoPack->ID, 1);
    file.write(&infoPack->currentMode, 1);

    // Guardar modos
    for (int i = 0; i < 16; ++i) {
        file.write((const uint8_t*)infoPack->mode[i].name, 24);
        file.write((const uint8_t*)infoPack->mode[i].desc, 192);
        file.write((const uint8_t*)infoPack->mode[i].config, 2);
    }

    // Guardar icono
    for (int y = 0; y < 64; ++y) {
        file.write((const uint8_t*)infoPack->icono[y], 64 * 2);
    }

    file.close();
    Serial.println("✅ Elemento añadido correctamente.");

    // Actualizar la lista de elementos y dibujar el elemento añadido
    loadElementsFromSPIFFS();
    drawCurrentElement();

    return 1;
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
        if (file.size() >= OFFSET_SERIAL + 2) {
            file.seek(OFFSET_SERIAL);
            file.read(tempPack->serialNum, 2);
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

void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
    tft.fillScreen(TFT_BLACK);
    dibujarMarco(TFT_WHITE);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2); // Usa Font 2
    tft.drawString(mensajeInicial, 64, 30);
    
    actualizarBarraProgreso(0);
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
    //drawCurrentElement();  // Volver a la pantalla principal
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
    } else if (respuesta == 1) {
        colorTexto = TFT_GREEN;
        mensajePrincipal = "NUEVO";
        mensajeSecundario = "Elemento nuevo    Agregando...";
    } else if (respuesta == 2) {
        colorTexto = TFT_YELLOW;
        mensajePrincipal = "ADVERTENCIA";
        mensajeSecundario = "Elemento ya existe Reasignando ID";
    } else {
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Error desconocido";
    }

    // Mostrar mensaje principal centrado
    tft.setTextFont(4);
    if (respuesta == 2) tft.setTextFont(2);  // Fuente aún más pequeña
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

    while (file) {
        byte existingID;
        file.seek(OFFSET_ID);
        file.read(&existingID, 1);
        if (existingID >= nextID) {
            nextID = existingID + 1;
        }
        file = root.openNextFile();
    }

    Serial.printf("✅ Nueva ID disponible: %d\n", nextID);
    return nextID;
}




