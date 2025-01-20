#include "DynamicLEDManager_DMS.h"
#include <Colors_DMS/Color_DMS.h>  
#include <FastLED.h>

/* -------------------------------------------------
 *    Implementación de DynamicEffect
 * ------------------------------------------------- */
DynamicEffect::DynamicEffect(COLORHANDLER_& handler, int index, unsigned int updateInterval)
    : colorHandler(handler), ledIndex(index), lastUpdate(0), interval(updateInterval)
{
}

DynamicEffect::~DynamicEffect() {
}

/* -------------------------------------------------
 *    Implementación de FadeEffect
 * ------------------------------------------------- */
FadeEffect::FadeEffect(COLORHANDLER_& handler, int index, CRGB c1, CRGB c2, unsigned int updateInterval)
    : DynamicEffect(handler, index, updateInterval), color1(c1), color2(c2), progress(0.0f), direction(1)
{
}

void FadeEffect::update() {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate >= interval) {
        lastUpdate = currentTime;

        // Actualizar progreso del fade
        progress += direction * 0.05f;
        if (progress >= 1.0f) {
            progress = 1.0f;
            direction = -1;  // Cambiar dirección
        } 
        else if (progress <= 0.0f) {
            progress = 0.0f;
            direction = 1;   // Cambiar dirección
        }

        // Interpolar entre color1 y color2
        CRGB interpolatedColor = color1.lerp16(color2, static_cast<uint16_t>(progress * 65535));
        colorHandler.leds[ledIndex] = interpolatedColor; // Actualizar LED en colorHandler
    }
}

/* -------------------------------------------------
 *    Implementación de DynamicLEDManager
 * ------------------------------------------------- */
DynamicLEDManager::DynamicLEDManager(COLORHANDLER_& handler)
    : colorHandler(handler)
{
}

DynamicLEDManager::~DynamicLEDManager() {
    clearEffects();
}

void DynamicLEDManager::addEffect(DynamicEffect* effect) {
    effects.push_back(effect);
}

void DynamicLEDManager::clearEffects() {
    for (auto effect : effects) {
        delete effect; // libera memoria de cada DynamicEffect
    }
    effects.clear();
}

void DynamicLEDManager::update() {
    // Primero actualizamos todos los efectos
    for (auto effect : effects) {
        effect->update();
    }

    // Mostrar el estado de los LEDs antes de enviarlos al hardware
    //Serial.println("Estado actual de los LEDs:");

    // // Opción 1: si numLeds es público en la clase COLORHANDLER_
    // for (int i = 0; i < colorHandler.numLeds; i++) {
    //     Serial.print("LED ");
    //     Serial.print(i);
    //     Serial.print(": ");
    //     Serial.print(colorHandler.leds[i].r);
    //     Serial.print(", ");
    //     Serial.print(colorHandler.leds[i].g);
    //     Serial.print(", ");
    //     Serial.println(colorHandler.leds[i].b);
    // }

    // Opción 2 (alternativa): si tienes un getter, colorHandler.getNumLeds()
    // for (int i = 0; i < colorHandler.getNumLeds(); i++) { ... }

    FastLED.show(); // Mostrar cambios en hardware
}
