#include "RelayStateManager.h"
#include <unordered_map>
#include <SPIFFS_handler/SPIFFS_handler.h>   // para abrir ficheros
#include <defines_DMS/defines_DMS.h>         // OFFSET_MODES, SIZE_MODE, etc.
#include <botonera_DMS/botonera_DMS.h>       // getModeConfig, getModeFlag
#include <info_elements_DMS/info_elements_DMS.h>
#include <encoder_handler/encoder_handler.h> 


// — Mapas internos estáticos —
static std::unordered_map<uint8_t,bool> relayStateMap;
static std::unordered_map<uint8_t,bool> relayCapabilityMap;

void RelayStateManager::set(uint8_t id, bool on) {
    relayStateMap[id] = on;
}

bool RelayStateManager::get(uint8_t id) {
    auto it = relayStateMap.find(id);
    return (it != relayStateMap.end()) ? it->second : false;
}

void RelayStateManager::clear() {
    relayStateMap.clear();
    relayCapabilityMap.clear();
}

uint8_t RelayStateManager::getElementIDFromFile(const String &file) {
    // Manejo de ficheros especiales
    if (file == "Ambientes" || file == "Fichas") {
        INFO_PACK_T* opt = (file == "Ambientes") ? &ambientesOption : &fichasOption;
        return opt->ID;
    }
    if (file == "Apagar") {
        return BROADCAST;
    }

    // Ficheros SPIFFS
    uint8_t elementID = BROADCAST;
    fs::File f = SPIFFS.open(file, "r");
    if (f) {
        f.seek(OFFSET_ID, SeekSet);
        f.read(&elementID, 1);
        f.close();  // ✅ Llave bien cerrada antes del 'else'
    } else {
        #ifdef DEBUG
        DEBUG__________ln("Error al leer la ID de " + file);
        #endif
    }
    return elementID;
}

void RelayStateManager::initCapabilities(const std::vector<String>& elementFiles) {
    relayCapabilityMap.clear();
    for (const auto &file : elementFiles) {
        uint8_t id = RelayStateManager::getElementIDFromFile(file);

        // ←─── Si es uno de los especiales, no lo analizamos
        if (file == "Ambientes" || file == "Fichas" || file == "Apagar") {
            relayCapabilityMap[id] = false;
            continue;
        }

        bool anyRelay = false;
        byte cfg[2];
        // Recorremos hasta 16 modos “reales”
        for (int m = 0; m < 16; ++m) {
            if (getModeConfig(file, m, cfg) &&
                getModeFlag(cfg, HAS_RELAY)) {
                anyRelay = true;
                break;
            }
        }
        relayCapabilityMap[id] = anyRelay;
    }
}


bool RelayStateManager::hasRelay(uint8_t elementID) {
    auto it = relayCapabilityMap.find(elementID);
    return (it != relayCapabilityMap.end()) && it->second;
}
