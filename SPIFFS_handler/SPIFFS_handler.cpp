#include <encoder_handler/encoder_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include "icons_64x64_DMS/icons_64x64_DMS.h"



bool writeBytesChecked(fs::File &f, const uint8_t* data, size_t length) {
    size_t written = f.write(data, length);
    if(written != length) {
        #ifdef DEBUG
          Serial.printf("Error: se esperaban escribir %u bytes, se escribieron %u\n",(unsigned)length,(unsigned)written);                                                                        
        #endif
        return false;
    }
    return true;
}


void formatSPIFFS() {
    #ifdef DEBUG
      Serial.print("Formateando SPIFFS...");                                                                            
    #endif
    
    SPIFFS.end();
    if (!SPIFFS.format()) {
        #ifdef DEBUG
          Serial.println("Error al formatear SPIFFS.");                                                                        
        #endif
        
    }
    if (!SPIFFS.begin(true)) {
        #ifdef DEBUG
          Serial.println("Error al montar SPIFFS después del formateo.");                                                                        
        #endif
        
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
    strncpy((char*)ambientesOption.name, "AMBIENTES", 24);
    strncpy((char*)ambientesOption.desc, "Configura los ambientes de la sala.", 192);
    ambientesOption.currentMode = 0;
    strncpy((char*)ambientesOption.mode[0].name, "BASICO", 24);
    strncpy((char*)ambientesOption.mode[0].desc, "Modo basico para ambientes.", 192);
    ambientesOption.mode[0].config[0] = 0x80;
    ambientesOption.mode[0].config[1] = 0x01;
    memcpy(ambientesOption.icono, ambientes_64x64, sizeof(ambientesOption.icono));

    // ----- Fichas -----
    memset(&fichasOption, 0, sizeof(INFO_PACK_T));
    strncpy((char*)fichasOption.name, "FICHAS", 24);
    strncpy((char*)fichasOption.desc, "Interacción con fichas NFC.", 192);
    fichasOption.currentMode = 0;
    strncpy((char*)fichasOption.mode[0].name, "BASICO", 24);
    strncpy((char*)fichasOption.mode[0].desc, "Modo basico para fichas NFC.", 192);
    fichasOption.mode[0].config[0] = 0x80;
    fichasOption.mode[0].config[1] = 0x01;
    strncpy((char*)fichasOption.mode[1].name, "PAREJAS", 24);
    strncpy((char*)fichasOption.mode[1].desc, "Modo parejas para fichas NFC.", 192);
    fichasOption.mode[1].config[0] = 0x80;
    fichasOption.mode[1].config[1] = 0x01;
    strncpy((char*)fichasOption.mode[2].name, "ADIVINAR", 24);
    strncpy((char*)fichasOption.mode[2].desc, "Modo adivinar para fichas NFC.", 192);
    fichasOption.mode[2].config[0] = 0x80;
    fichasOption.mode[2].config[1] = 0x01;

    memcpy(fichasOption.icono, fichas_64x64, sizeof(fichasOption.icono));
    //Serial.println("Nombre de fichasOption.name después de crear el icono: " + String((char*)fichasOption.name));

    // ----- ApagarSala -----
    memset(&apagarSala, 0, sizeof(INFO_PACK_T));
    strncpy((char*)apagarSala.name, "APAGAR", 24);
    strncpy((char*)apagarSala.desc, "Apaga la sala por completo.", 192);
    apagarSala.currentMode = 0;
    // Puedes añadir uno o varios modos, aquí un ejemplo con un solo modo "OFF".
    // strncpy((char*)apagarSala.mode[0].name, "OFF", 24);
    // strncpy((char*)apagarSala.mode[0].desc, "Modo para apagar la sala.", 192);
    // apagarSala.mode[0].config[0] = 0x7F; // El valor que tú desees para este modo
    // apagarSala.mode[0].config[1] = 0xFF;

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
                                                                                                                #ifdef DEBUG
                                                                                                                Serial.println("Error: No se pudo abrir el directorio raíz de SPIFFS.");                                                                           
                                                                                                                #endif
        
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
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("Elementos encontrados:");
                                                                                    #endif
    
    for (size_t i = 0; i < elementFiles.size(); i++) {
        Serial.printf("%d: %s\n", (int)i, elementFiles[i].c_str());
    }
                                                                                    #ifdef DEBUG
                                                                                    Serial.printf("Total de elementos: %d\n", (int)elementFiles.size());                                                                                
                                                                                    #endif
    
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
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("Error al leer la ID del archivo.");                                                                           
                                                                                    #endif
        
    }
    return elementID;
}

bool isCurrentElementSelected() {
    return selectedStates[currentIndex];  // Devuelve true si está seleccionado
}

bool checkMostSignificantBit(byte modeConfig[2]) {
    // Devuelve true si el bit más significativo del primer byte es 1, de lo contrario false
    //Serial.println("Devolviendo el bit más significativo del modo. ");
    return (modeConfig[0] & 0x80) != 0;
}

bool getModeConfig(const String& fileName, byte mode, byte modeConfig[2]) {
    memset(modeConfig, 0, 2); // Inicializar en 0

    if (fileName == "Ambientes" || fileName == "Fichas" || fileName == "Apagar") {
        INFO_PACK_T* option = (fileName == "Ambientes") ? &ambientesOption : &fichasOption;
        memcpy(modeConfig, option->mode[mode].config, 2);
        return true;
    } else {
       
        fs::File f = SPIFFS.open(fileName, "r");
        if (!f) {
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("❌ Error abriendo el archivo: " + fileName);                                                                       
                                                                                    #endif
            
            return false;
        }

        f.seek(OFFSET_MODES + (SIZE_MODE * mode) + 24 + 192, SeekSet);
        if (f.read(modeConfig, 2) != 2) {
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("❌ Error leyendo configuración del modo");                                                                      
                                                                                    #endif
            
            f.close();
            return false;
        }

        f.close();
        return true;
    }
}

void setAllElementsToBasicMode() {

    // Asegurarse de cargar la lista de elementos desde SPIFFS.
    // Si ya se ejecutó previamente loadElementsFromSPIFFS(), se puede omitir esta línea.
   // loadElementsFromSPIFFS();
    
    // Recorrer la lista de archivos obtenida
    for (size_t i = 0; i < elementFiles.size(); i++) {
        String fileName = elementFiles[i];
        // Sólo queremos actualizar los archivos de SPIFFS (aquellos que comienzan con "/element_")
        if (!fileName.startsWith("/element_")) {
            continue;
        }
        // También filtramos para que sean archivos .bin
        if (!fileName.endsWith(".bin")) {
            continue;
        }

        fs::File f = SPIFFS.open(fileName, "r+");
        if (!f) {
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("❌ Error abriendo " + fileName + " para escritura.");                                                                       
                                                                                    #endif
            continue;
        }
        byte basicMode = DEFAULT_BASIC_MODE; // Modo básico, normalmente 1
        f.seek(OFFSET_CURRENTMODE, SeekSet);
        f.write(&basicMode, 1);
        f.flush();
        f.close();
                                                                                    #ifdef DEBUG
                                                                                    Serial.println("✅ Modo básico actualizado en " + fileName);                                                                    
                                                                                    #endif
        
    }   
}
