#include "RelayStateManager.h"
#include <unordered_map>
#include <SPIFFS_handler/SPIFFS_handler.h>   // para abrir ficheros
#include <defines_DMS/defines_DMS.h>         // OFFSET_MODES, SIZE_MODE, etc.
#include <botonera_DMS/botonera_DMS.h>       // getModeConfig, getModeFlag
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

uint8_t RelayStateManager::getElementIDFromFile(const String &file)
{
    // ---------- Elementos cargados en RAM ----------
    if (file == "Ambientes" || file == "Fichas" || file == "Comunicador") {
        INFO_PACK_T* opt = nullptr;
        if      (file == "Ambientes")   opt = &ambientesOption;
        else if (file == "Fichas")      opt = &fichasOption;
        else                            opt = &comunicadorOption;     // 👈 NUEVO
        return opt ? opt->ID : BROADCAST;
    }

    // ---------- Caso especial “Apagar” ----------
    if (file == "Apagar") {
        return BROADCAST;    // Se dirige a todos
    }

    // ---------- Elementos almacenados en SPIFFS ----------
    uint8_t elementID = BROADCAST;
    String path = file.startsWith("/") ? file : "/" + file;          // 👈 asegura la barra
    fs::File f = SPIFFS.open(path, "r");
    if (f) {
        f.seek(OFFSET_ID, SeekSet);
        f.read(&elementID, 1);
        f.close();
    #ifdef DEBUG
    } else {
        DEBUG__________ln("❌ Error al leer la ID de " + path);
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

bool RelayStateManager::getModeConfigForID(uint8_t id, uint8_t modeCfg[2])
{
    memset(modeCfg, 0, 2);

    /* —— elementos estáticos en RAM ———————————————— */
    const INFO_PACK_T* statics[] = { &ambientesOption, &fichasOption,
                                     &comunicadorOption, &apagarSala };
    for (const INFO_PACK_T* opt : statics) {
        if (opt->ID == id) {
            memcpy(modeCfg, opt->mode[opt->currentMode].config, 2);
            return true;
        }
    } 

    /* —— buscar en archivos SPIFFS ———————————————— */
    for (const String& f : elementFiles) {
        if (f == "Ambientes" || f == "Fichas" ||
            f == "Comunicador" || f == "Apagar") continue;

        String path = f.startsWith("/") ? f : "/" + f;
        fs::File file = SPIFFS.open(path, "r");
        if (!file) continue;

        uint8_t fid;
        file.seek(OFFSET_ID, SeekSet);
        file.read(&fid, 1);

        if (fid == id) {                         // ← encontrado
            uint8_t curMode;
            file.seek(OFFSET_CURRENTMODE, SeekSet);
            file.read(&curMode, 1);

            file.seek(OFFSET_MODES + curMode * SIZE_MODE + 216, SeekSet);
            file.read(modeCfg, 2);
            file.close();
            return true;
        }
        file.close();
    }
    return false;                                // no localizado
}