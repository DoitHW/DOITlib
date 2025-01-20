#pragma once
#include <FastLED.h>
#include <vector>

// Forward declaration de COLORHANDLER_ para evitar ciclos de inclusión
class COLORHANDLER_;

// Clase base para los efectos dinámicos
class DynamicEffect {
protected:
    COLORHANDLER_& colorHandler;  // Referencia a COLORHANDLER_
    int ledIndex;                 // Índice del LED
    unsigned long lastUpdate;     // Última vez que se actualizó
    unsigned int interval;        // Intervalo entre actualizaciones

public:
    DynamicEffect(COLORHANDLER_& handler, int index, unsigned int updateInterval);
    virtual ~DynamicEffect();

    // Método virtual puro que deberá implementarse en la subclase
    virtual void update() = 0;
};

// Clase para el efecto Fade
class FadeEffect : public DynamicEffect {
private:
    CRGB color1, color2; // Colores entre los que se realiza el fade
    float progress;      // Progreso del fade (0.0 a 1.0)
    int direction;       // Dirección del fade (1 = aumentar, -1 = disminuir)

public:
    FadeEffect(COLORHANDLER_& handler, int index, CRGB c1, CRGB c2, unsigned int updateInterval);

    void update() override;
};

// Clase para manejar todos los efectos dinámicos
class DynamicLEDManager {
private:
    std::vector<DynamicEffect*> effects; // Lista de efectos dinámicos
    COLORHANDLER_& colorHandler;         // Referencia a COLORHANDLER_

public:
    DynamicLEDManager(COLORHANDLER_& handler);
    ~DynamicLEDManager();

    void addEffect(DynamicEffect* effect);
    void clearEffects();
    void update();
};

extern DynamicLEDManager ledManager;