// RelayStateManager.h
#pragma once
#include <cstdint>
#include <vector>
#include <Arduino.h>            // para String

class RelayStateManager {
public:
    // --- Estado dinámico ---
    static void   set(uint8_t elementID, bool on);
    static bool   get(uint8_t elementID);
    static void   clear();

    // --- Capacidades de relé (precálculo) ---
    static void   initCapabilities(const std::vector<String>& elementFiles);
    static bool   hasRelay(uint8_t elementID);

    // --- Nuevo: helper para extraer la ID desde un nombre de fichero ---
    static uint8_t getElementIDFromFile(const String &file);
    static bool getModeConfigForID(uint8_t id, uint8_t modeCfg[2]);
};
