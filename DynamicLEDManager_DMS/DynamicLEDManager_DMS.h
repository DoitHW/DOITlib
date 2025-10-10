#pragma once
#include <FastLED.h>
#include <Frame_DMS/Frame_DMS.h> // para COLORPAD_BTNMAP
#include <vector>

// Forward declaration de COLORHANDLER_ para evitar ciclos de inclusión
class COLORHANDLER_;

// =============================================================
// Clase base de efectos dinámicos
// =============================================================
class DynamicEffect {
public:
    DynamicEffect(COLORHANDLER_& handler, int index, unsigned int updateInterval);
    virtual ~DynamicEffect();
    virtual void update() = 0;
    virtual void reset() {};

protected:
    COLORHANDLER_&  colorHandler;
    int             ledIndex;      // -1 => efecto global (si aplica)
    unsigned long   lastUpdate;
    unsigned int    interval;      // ms
};

// =============================================================
// Efecto existente (tu clásico): transición A <-> B
// =============================================================
class FadeEffect : public DynamicEffect {
public:
    // periodMs = intervalo de actualización; internamente la fase avanza a ritmo suave
    FadeEffect(COLORHANDLER_& handler, int index, CRGB colorA, CRGB colorB, unsigned int periodMs);
    void update() override;

private:
    CRGB    ca, cb;
    uint8_t t;  // fase 0..255
};

// =============================================================
// Onda (fade color->negro->color). Velocidad vía 'interval'
// =============================================================
class WaveEffect : public DynamicEffect {
public:
    WaveEffect(COLORHANDLER_& handler, int index, CRGB base, unsigned int updateMs);
    void update() override;

private:
    CRGB    color;
    uint8_t t;      // fase 0..255
};

// =============================================================
// Arcoíris en bucle por LED (HSV), ignora color base
// =============================================================
class RainbowLoopEffect : public DynamicEffect {
public:
    RainbowLoopEffect(COLORHANDLER_& handler, int index, uint8_t startHue, unsigned int updateMs);
    void update() override;

private:
    uint8_t hue;
};

// =============================================================
// Burbujeo: chisporroteos aleatorios sobre color base
//  - Si ledIndex >= 0 → actúa en un LED
//  - Si ledIndex == -1 → versión simple “global”
// =============================================================
class BubblesEffect : public DynamicEffect {
public:
    BubblesEffect(COLORHANDLER_& handler, int index, CRGB base, unsigned int updateMs, uint8_t density = 25);
    void update() override;

private:
    CRGB    color;
    uint8_t dens;   // probabilidad 0..255 por tick
};

/* ============================================================
 *    HeartbeatEffect - Latido de corazón realista
 * ============================================================ */
class HeartbeatEffect : public DynamicEffect {
private:
    CRGB color;
    uint8_t beatPhase;   // 0..5
    uint16_t beatTimer;  // ms acumulados en subfase

public:
    HeartbeatEffect(COLORHANDLER_& handler, int index, CRGB heartColor, unsigned int updateMs);
    void update() override;
    void reset() override { beatPhase = 0; beatTimer = 0; }
};

/* ============================================================
 *    AuroraEffect - Aurora boreal suave
 * ============================================================ */
class AuroraEffect : public DynamicEffect {
private:
    uint16_t phase;
    CRGB color1, color2;

public:
    AuroraEffect(COLORHANDLER_& handler, int index, CRGB c1, CRGB c2, unsigned int updateMs);
    void update() override;
    void reset() override { phase = 0; }
};

/* ============================================================
 *    StrobeEffect - Estroboscópico configurable
 * ============================================================ */
class StrobeEffect : public DynamicEffect {
private:
    CRGB color;
    uint8_t flashCount;
    uint8_t currentFlash;
    bool isOn;
    unsigned long flashDuration;
    unsigned long pauseDuration;
    unsigned long phaseTimer;   // <- necesario

public:
    StrobeEffect(COLORHANDLER_& handler, int index, CRGB strobeColor,
                 unsigned int updateMs, uint8_t flashes = 3);
    void update() override;
    void reset() override { currentFlash = 0; isOn = false; phaseTimer = 0; }
};

/* ============================================================
 *    ColorWipeEffect - Barrido progresivo de color
 * ============================================================ */
class ColorWipeEffect : public DynamicEffect {
private:
    CRGB color;
    int currentLed;
    int direction;
    bool fill;

public:
    ColorWipeEffect(COLORHANDLER_& handler, int index, CRGB wipeColor,
                    unsigned int updateMs, bool fillMode = true);
    void update() override;
    void reset() override { currentLed = 0; direction = 1; }
};

/* ============================================================
 *    TheaterChaseEffect - Persecución estilo marquesina
 * ============================================================ */
class TheaterChaseEffect : public DynamicEffect {
private:
    CRGB color;
    uint8_t spacing;
    uint8_t offset;

public:
    TheaterChaseEffect(COLORHANDLER_& handler, int index, CRGB chaseColor,
                       unsigned int updateMs, uint8_t space = 3);
    void update() override;
    void reset() override { offset = 0; }
};

// =============================================================
// Gestor de efectos (no cambio firmas públicas)
// =============================================================
class DynamicLEDManager {
public:
    DynamicLEDManager(COLORHANDLER_& handler);
    ~DynamicLEDManager();

    void addEffect(DynamicEffect* effect); // toma propiedad y lo deletea en clear/dtor
    void clearEffects();                   // borra y limpia todos los efectos
    void update();                         // llama a update() de cada efecto y hace FastLED.show()

private:
    std::vector<DynamicEffect*> effects;
    COLORHANDLER_&              colorHandler;
};

// Declaración externa del manager (si ya la usas en otros módulos)
extern DynamicLEDManager ledManager;