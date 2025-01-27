#include <encoder_handler/encoder_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include "icons_64x64_DMS/icons_64x64_DMS.h"



bool writeBytesChecked(fs::File &f, const uint8_t* data, size_t length) {
    size_t written = f.write(data, length);
    if(written != length) {
        Serial.printf("Error: se esperaban escribir %u bytes, se escribieron %u\n",(unsigned)length,(unsigned)written);
        return false;
    }
    return true;
}


void formatSPIFFS() {
    Serial.print("Formateando SPIFFS...");
    SPIFFS.end();
    if (!SPIFFS.format()) {
        Serial.println("Error al formatear SPIFFS.");
    }
    if (!SPIFFS.begin(true)) {
        Serial.println("Error al montar SPIFFS después del formateo.");
    }
}


String generateUniqueFileName(const char* baseName) {
    String baseFileName = String("/element_") + baseName + ".bin";
    String fileName = baseFileName;
    int index = 2;
    while (SPIFFS.exists(fileName)) {
        fileName = String("/element_") + baseName + "_" + String(index) + ".bin";
        index++;
    }
    return fileName;
}

bool saveElementFieldByField(const char* baseName, const uint16_t* iconData) {
    //Serial.println("Guardando elemento en SPIFFS...");

    String uniqueFileName = generateUniqueFileName(baseName);
    String uniqueElementName = uniqueFileName.substring(9, uniqueFileName.length() - 4); 
    char elementName[24];
    memset(elementName, 0, 24);
    strncpy(elementName, uniqueElementName.c_str(), 24);

    fs::File f = SPIFFS.open(uniqueFileName, "w");
    if (!f) {
        Serial.println("Error creando archivo del elemento");
        return false;
    }

    f.seek(OFFSET_NAME, SeekSet);
    if (!writeBytesChecked(f, (uint8_t*)elementName, 24)) return false;
    //Serial.printf("Nombre único escrito: %s\n", elementName);

    const char* desc = "Prueba desc elemento mostrando icono correctamente";
    byte descBuf[192];
    memset(descBuf, 0, 192);
    strncpy((char*)descBuf, desc, 192);
    f.seek(OFFSET_DESC, SeekSet);
    if (!writeBytesChecked(f, descBuf, 192)) return false;
    //Serial.println("Descripcion escrita.");

    byte serialBuf[2] = {0x12, 0x34};
    f.seek(OFFSET_SERIAL, SeekSet);
    if (!writeBytesChecked(f, serialBuf, 5)) return false;
    //Serial.println("SerialNum escrito.");

    byte id = BROADCAST;

    if (strcmp(baseName, "Columna") == 0) id = 0x04;
    else if (strcmp(baseName, "LED strip") == 0) id = 0xDD;

        f.seek(OFFSET_ID, SeekSet);
        if (!writeBytesChecked(f, &id, 1))
            return false;
        // Serial.println("ID escrito.");

        byte currentMode = 0;
        f.seek(OFFSET_CURRENTMODE, SeekSet);
        if (!writeBytesChecked(f, &currentMode, 1))
            return false;
        // Serial.println("CurrentMode escrito.");

        // Guardar modos
        byte modeBuf[SIZE_MODE];
        memset(modeBuf, 0, SIZE_MODE);

        // Modo Básico
        const char *modoName1 = "Basico";
        const char *modoDesc1 = "Descripcion modo basico";
        strncpy((char *)modeBuf, modoName1, 24);
        strncpy((char *)(modeBuf + 24), modoDesc1, 192);
        modeBuf[24 + 192] = 0x00;
        modeBuf[24 + 192 + 1] = 0x01;

        f.seek(OFFSET_MODES, SeekSet);
        if (!writeBytesChecked(f, modeBuf, SIZE_MODE))
            return false;
        // Serial.println("Modo 0 (Básico) escrito.");

        // Si el elemento es "Escalera", añadir modo "Divertido"
        if (strcmp(baseName, "Escalera") == 0)
        {
            // Modo Básico ya añadido arriba
            memset(modeBuf, 0, SIZE_MODE);
            const char *modoName2 = "Divertido";
            const char *modoDesc2 = "Descripcion modo divertido";
            strncpy((char *)modeBuf, modoName2, 24);
            strncpy((char *)(modeBuf + 24), modoDesc2, 192);
            modeBuf[24 + 192] = 0x00;
            modeBuf[24 + 192 + 1] = 0x02; // Identificador del modo Divertido
            if (!writeBytesChecked(f, modeBuf, SIZE_MODE))
                return false;
            // Serial.println("Modo 1 (Divertido) escrito.");

            // Añadir espacios vacíos para posiciones 3, 4, y 5
            byte emptyMode[SIZE_MODE];
            memset(emptyMode, 0, SIZE_MODE);
            for (int i = 2; i < 6; i++)
            {
                if (!writeBytesChecked(f, emptyMode, SIZE_MODE))
                    return false;
            }
            // Serial.println("Huecos vacíos escritos para posiciones 3, 4 y 5.");

            // Modo Pasivo
            memset(modeBuf, 0, SIZE_MODE);
            const char *modoName6 = "Pasivo";
            const char *modoDesc6 = "Descripcion modo pasivo";
            strncpy((char *)modeBuf, modoName6, 24);
            strncpy((char *)(modeBuf + 24), modoDesc6, 192);
            modeBuf[24 + 192] = 0x00;
            modeBuf[24 + 192 + 1] = 0x06; // Identificador del modo Pasivo
            if (!writeBytesChecked(f, modeBuf, SIZE_MODE))
                return false;
            // Serial.println("Modo 6 (Pasivo) escrito.");
        }

        // Modos vacíos para completar los 16
        byte emptyMode[SIZE_MODE];
        memset(emptyMode, 0, SIZE_MODE);
        int startModeIndex = (strcmp(baseName, "Escalera") == 0) ? 2 : 1; // Si es "Escalera", iniciar en el índice 2
        for (int i = startModeIndex; i < 16; i++)
        {
            if (!writeBytesChecked(f, emptyMode, SIZE_MODE))
                return false;
        }
        // Serial.println("Modos vacíos escritos.");

        // Guardar icono
        f.seek(OFFSET_ICONO, SeekSet);
        for (int y = 0; y < 64; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                uint16_t pixel = iconData[y * 64 + x];
                if (!writeBytesChecked(f, (uint8_t *)&pixel, 2))
                    return false;
            }
        }
        // Serial.println("Icono escrito en SPIFFS.");

        f.flush();
        size_t finalSize = f.size();
        f.close();
        Serial.printf("Elemento guardado con éxito, archivo: %s, tamaño final: %u bytes\n", uniqueFileName.c_str(), (unsigned)finalSize);
        return true;
    }

void printElementInfo(const String &fileName) {
    
    Serial.println("Se va a abrir el archivo: " + fileName);
    fs::File f = SPIFFS.open(fileName, "r");
    if (!f) {
        Serial.println("Error al abrir el archivo del elemento para imprimir su info.");
        return;
    }

    Serial.println("#### Mostrando elemento en SPIFFFS ####");
    Serial.println();

    // Leer campos
    char name[25] = {0};
    f.seek(OFFSET_NAME, SeekSet);
    f.read((uint8_t*)name, 24);
    Serial.printf("✍️ Nombre: %s\n", name);

    char desc[193] = {0};
    f.seek(OFFSET_DESC, SeekSet);
    f.read((uint8_t*)desc, 192);
    Serial.printf("📋 Descripción: %s\n", desc);

    byte serialNum[4] = {0}; // Cambiado a tamaño 4 para coincidir con la lectura
    f.seek(OFFSET_SERIAL, SeekSet);
    f.read(serialNum, 4);
    Serial.printf("📋 Número de serie: 0x%02X%02X%02X%02X\n", serialNum[0], serialNum[1], serialNum[2], serialNum[3]);

    byte id;
    f.seek(OFFSET_ID, SeekSet);
    f.read(&id, 1);
    Serial.printf("📋 ID: 0x%02X\n", id); // Corregido el formato de impresión

    byte currentMode;
    f.seek(OFFSET_CURRENTMODE, SeekSet);
    f.read(&currentMode, 1);
    Serial.printf("📋 Modo actual: %d\n", currentMode);

    // Leer modos
    f.seek(OFFSET_MODES, SeekSet);
    for (int i = 0; i < 16; i++) {
        char modeName[25] = {0};
        char modeDesc[193] = {0};
        byte modeConfig[2] = {0};

        f.read((uint8_t*)modeName, 24);
        f.read((uint8_t*)modeDesc, 192);
        f.read(modeConfig, 2);

        

        if (strlen(modeName) > 0) {
            Serial.printf("Modo %d:\n", i);
            Serial.printf("  Nombre: %s\n", modeName);
            Serial.printf("  Descripción: %s\n", modeDesc);
            Serial.printf("  Configuración: 0x%02X%02X\n", modeConfig[0], modeConfig[1]);
            if (checkMostSignificantBit(modeConfig)) {
            Serial.println("El bit más significativo del modo " + String(i) + " es 1");
        } else {
            Serial.println("El bit más significativo del modo " + String(i) + " es 0");
        }
        }
    }

    f.close();
}


bool readElementData(fs::File& f, char* elementName, char* modeName, int& startX, int& startY) {
    // Leer nombre del elemento
    f.seek(OFFSET_NAME, SeekSet);
    f.read((uint8_t*)elementName, 24);
    elementName[23] = 0;

    // Leer nombre del modo actual
    byte currentMode;
    f.seek(OFFSET_CURRENTMODE, SeekSet);
    f.read(&currentMode, 1);

    f.seek(OFFSET_MODES + (SIZE_MODE * currentMode), SeekSet);
    f.read((uint8_t*)modeName, 24);
    modeName[23] = 0;

    // Calcular posiciones del icono
    startX = (tft.width() - 64) / 2;
    startY = (tft.height() - 64) / 2 - 20;

    return true;
}


void initializeDynamicOptions() {
    // ----- Ambientes -----
    memset(&ambientesOption, 0, sizeof(INFO_PACK_T));
    strncpy((char*)ambientesOption.name, "Ambientes", 24);
    strncpy((char*)ambientesOption.desc, "Configura los ambientes de la sala.", 192);
    ambientesOption.currentMode = 0;
    strncpy((char*)ambientesOption.mode[0].name, "Basico", 24);
    strncpy((char*)ambientesOption.mode[0].desc, "Modo basico para ambientes.", 192);
    ambientesOption.mode[0].config[0] = 0x80;
    ambientesOption.mode[0].config[1] = 0x01;
    memcpy(ambientesOption.icono, ambientes_64x64, sizeof(ambientesOption.icono));

    // ----- Fichas -----
    memset(&fichasOption, 0, sizeof(INFO_PACK_T));
    strncpy((char*)fichasOption.name, "Fichas", 24);
    strncpy((char*)fichasOption.desc, "Interacción con fichas NFC.", 192);
    fichasOption.currentMode = 0;
    strncpy((char*)fichasOption.mode[0].name, "Basico", 24);
    strncpy((char*)fichasOption.mode[0].desc, "Modo basico para fichas NFC.", 192);
    fichasOption.mode[0].config[0] = 0x82;
    fichasOption.mode[0].config[1] = 0x00;
    memcpy(fichasOption.icono, fichas_64x64, sizeof(fichasOption.icono));
    Serial.println("Nombre de fichasOption.name después de crear el icono: " + String((char*)fichasOption.name));

    // ----- ApagarSala -----
    memset(&apagarSala, 0, sizeof(INFO_PACK_T));
    strncpy((char*)apagarSala.name, "Apagar", 24);
    strncpy((char*)apagarSala.desc, "Apaga la sala por completo.", 192);
    apagarSala.currentMode = 0;
    // Puedes añadir uno o varios modos, aquí un ejemplo con un solo modo "OFF".
    strncpy((char*)apagarSala.mode[0].name, "OFF", 24);
    strncpy((char*)apagarSala.mode[0].desc, "Modo para apagar la sala.", 192);
    apagarSala.mode[0].config[0] = 0x83; // El valor que tú desees para este modo
    apagarSala.mode[0].config[1] = 0x00;

    // Asumiendo que tienes un icono llamado apagar_64x64:
    memcpy(apagarSala.icono, apagar_sala_64x64, sizeof(apagarSala.icono));
}


void loadElementsFromSPIFFS() {
    elementFiles.clear();
    selectedStates.clear();

    // Agregar las opciones dinámicas
    elementFiles.push_back("Ambientes");
    selectedStates.push_back(false);

    elementFiles.push_back("Fichas");
    selectedStates.push_back(false);

    elementFiles.push_back("Apagar");
    selectedStates.push_back(false);


    // Cargar elementos desde SPIFFS
    fs::File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Error: No se pudo abrir el directorio raíz de SPIFFS.");
        return;
    }

    fs::File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin") && fileName.indexOf("_icon") < 0) {
            elementFiles.push_back(fileName);
            selectedStates.push_back(false); // Inicialmente no seleccionado
        }
        file.close();
        file = root.openNextFile();
    }

    Serial.println("Elementos encontrados:");
    for (size_t i = 0; i < elementFiles.size(); i++) {
        Serial.printf("%d: %s\n", (int)i, elementFiles[i].c_str());
    }
    Serial.printf("Total de elementos: %d\n", (int)elementFiles.size());
}

byte getCurrentElementID() {
    byte elementID = BROADCAST;  
    String currentFile = elementFiles[currentIndex];

    if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        return option->ID;
    }

    // Leer la ID desde SPIFFS solo si está seleccionado
    fs::File f = SPIFFS.open(currentFile, "r");
    if (f) {
        f.seek(OFFSET_ID, SeekSet);
        f.read(&elementID, 1);
        f.close();
    } else {
        Serial.println("Error al leer la ID del archivo.");
    }
    return elementID;
}

bool isCurrentElementSelected() {
    return selectedStates[currentIndex];  // Devuelve true si está seleccionado
}

bool checkMostSignificantBit(byte modeConfig[2]) {
    // Devuelve true si el bit más significativo del primer byte es 1, de lo contrario false
    Serial.println("Devolviendo el bit más significativo del modo. ");
    return (modeConfig[0] & 0x80) != 0;
}


