#include "DynamicLEDManager_DMS.h"
#include <Colors_DMS/Color_DMS.h>  
#include <Frame_DMS/Frame_DMS.h>
#include <FastLED.h>


namespace {
// Utilidades locales (nombres distintos para evitar colisiones)
static uint8_t ease8InOutCubic_u(uint8_t x) {
    if (x < 128) {
        uint16_t xx = x * 2;
        return (xx * xx * xx) / 16384;
    } else {
        uint16_t xx = (255 - x) * 2;
        return 255 - ((xx * xx * xx) / 16384);
    }
}

static CRGB lerpColor_u(const CRGB& a, const CRGB& b, uint8_t t) {
    return CRGB(
        lerp8by8(a.r, b.r, t),
        lerp8by8(a.g, b.g, t),
        lerp8by8(a.b, b.b, t)
    );
}

inline void writeSingleOrAll(COLORHANDLER_& h, int ledIndex, const CRGB& c) {
    if (!h.leds || h.numLeds <= 0) return;
    if (ledIndex >= 0 && ledIndex < h.numLeds) {
        h.leds[ledIndex] = c;
    } else {
        fill_solid(h.leds, h.numLeds, c);
    }
}
} // namespace

/* ============================================================
 *    DynamicEffect (base)
 * ============================================================ */
DynamicEffect::DynamicEffect(COLORHANDLER_& handler, int index, unsigned int updateInterval)
    : colorHandler(handler), ledIndex(index), lastUpdate(0), interval(updateInterval)
{
}

DynamicEffect::~DynamicEffect() {}

/* ============================================================
 *    FadeEffect  (A <-> B, tu efecto clásico)
 * ============================================================ */
FadeEffect::FadeEffect(COLORHANDLER_& handler, int index, CRGB colorA, CRGB colorB, unsigned int periodMs)
    : DynamicEffect(handler, index, periodMs), ca(colorA), cb(colorB), t(0)
{
}

void FadeEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    // Onda triangular sobre 0..255
    t = uint8_t(t + 3);
    uint8_t level = (t < 128) ? (t << 1) : ((255 - t) << 1); // 0..255..0

    // Interpolación entre A y B con 'level'
    auto mix8 = [&](uint8_t a, uint8_t b, uint8_t lv) -> uint8_t {
        // lv 0 => a, lv 255 => b (aprox)
        uint16_t x = (uint16_t(a) * (255 - lv)) + (uint16_t(b) * lv);
        return uint8_t(x >> 8);
    };

    CRGB out(
        mix8(ca.r, cb.r, level),
        mix8(ca.g, cb.g, level),
        mix8(ca.b, cb.b, level)
    );

    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds && colorHandler.leds) {
        colorHandler.leds[ledIndex] = out;
    }
}

/* ============================================================
 *    WaveEffect  (color ↔ negro)
 * ============================================================ */
WaveEffect::WaveEffect(COLORHANDLER_& handler, int index, CRGB base, unsigned int updateMs)
    : DynamicEffect(handler, index, updateMs), color(base), t(0)
{
}

void WaveEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    t = uint8_t(t + 5); // velocidad por tick
    uint8_t level = (t < 128) ? (t << 1) : ((255 - t) << 1); // 0..255..0

    CRGB out(
        (uint16_t(color.r) * level) >> 8,
        (uint16_t(color.g) * level) >> 8,
        (uint16_t(color.b) * level) >> 8
    );

    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds && colorHandler.leds) {
        colorHandler.leds[ledIndex] = out;
    }
}

/* ============================================================
 *    RainbowLoopEffect (HSV hue++ por LED)
 * ============================================================ */
RainbowLoopEffect::RainbowLoopEffect(COLORHANDLER_& handler, int index, uint8_t startHue, unsigned int updateMs)
    : DynamicEffect(handler, index, updateMs), hue(startHue)
{
}

void RainbowLoopEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    hue += 2; // giro del color
    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds && colorHandler.leds) {
        colorHandler.leds[ledIndex] = CHSV(hue, 255, 255);
    }
}

/* ============================================================
 *    BubblesEffect  (destellos aleatorios)
 * ============================================================ */
BubblesEffect::BubblesEffect(COLORHANDLER_& handler, int index, CRGB base, unsigned int updateMs, uint8_t density)
    : DynamicEffect(handler, index, updateMs), color(base), dens(density)
{
}

void BubblesEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    auto spark = [&](int idx){
        // chispa: levantar brillo hacia blanco mezclado con base
        CRGB mix = color;
        mix.r = qadd8(mix.r, 255 - (mix.r >> 1));
        mix.g = qadd8(mix.g, 255 - (mix.g >> 1));
        mix.b = qadd8(mix.b, 255 - (mix.b >> 1));
        colorHandler.leds[idx] = mix;
    };

    if (!colorHandler.leds || colorHandler.numLeds <= 0) return;

    if (ledIndex >= 0) {
        // Modo local: chispa ocasional en un LED
        if (random8() < dens) spark(ledIndex);
        else                  colorHandler.leds[ledIndex] = color; // reposo
    } else {
        // Modo global simple: unas pocas chispas dispersas
        int bursts = max(1, colorHandler.numLeds / 6);
        for (int n = 0; n < bursts; ++n) {
            if (random8() < dens) {
                int idx = random8() % colorHandler.numLeds;
                spark(idx);
            }
        }
    }
}

/* ============================================================
 *    HeartbeatEffect - Latido de corazón
 * ============================================================ */
 HeartbeatEffect::HeartbeatEffect(COLORHANDLER_& handler, int index, CRGB heartColor, unsigned int updateMs)
: DynamicEffect(handler, index, updateMs), color(heartColor), beatPhase(0), beatTimer(0) {}

void HeartbeatEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    uint16_t dt = (uint16_t)(now - lastUpdate);
    lastUpdate = now;

    beatTimer += dt;

    // Duraciones de sub-fases (ms)
    const uint16_t T_UP1   = 80;
    const uint16_t T_DOWN1 = 120;
    const uint16_t T_PAUSE = 80;
    const uint16_t T_UP2   = 60;
    const uint16_t T_DOWN2 = 140;
    const uint16_t T_GAP   = 400;

    uint8_t b = 0;

    switch (beatPhase) {
        case 0: // subida 1
            if (beatTimer >= T_UP1) { beatTimer = 0; beatPhase = 1; }
            b = ease8InOutCubic_u(scale8((uint8_t)map(beatTimer, 0, T_UP1, 0, 255), 255));
            break;
        case 1: // bajada 1
            if (beatTimer >= T_DOWN1) { beatTimer = 0; beatPhase = 2; }
            b = 255 - ease8InOutCubic_u(scale8((uint8_t)map(beatTimer, 0, T_DOWN1, 0, 255), 255));
            b = std::max<uint8_t>(b, 40);
            break;
        case 2: // pausa
            if (beatTimer >= T_PAUSE) { beatTimer = 0; beatPhase = 3; }
            b = 30;
            break;
        case 3: // subida 2 (más corta)
            if (beatTimer >= T_UP2) { beatTimer = 0; beatPhase = 4; }
            b = ease8InOutCubic_u(scale8((uint8_t)map(beatTimer, 0, T_UP2, 0, 255), 255));
            b = scale8(b, 200);
            break;
        case 4: // bajada 2
            if (beatTimer >= T_DOWN2) { beatTimer = 0; beatPhase = 5; }
            b = 200 - ease8InOutCubic_u(scale8((uint8_t)map(beatTimer, 0, T_DOWN2, 0, 255), 255));
            b = std::max<uint8_t>(b, 20);
            break;
        default: // 5: pausa larga
            if (beatTimer >= T_GAP) { beatTimer = 0; beatPhase = 0; }
            b = 15;
            break;
    }

    CRGB out = color; out.nscale8(b);
    writeSingleOrAll(colorHandler, ledIndex, out);
}


/* ============================================================
 *    Aurora - Aurora boreal
 * ============================================================ */
AuroraEffect::AuroraEffect(COLORHANDLER_& handler, int index, CRGB c1, CRGB c2, unsigned int updateMs)
: DynamicEffect(handler, index, updateMs), phase(0), color1(c1), color2(c2) {}

void AuroraEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    if (!colorHandler.leds || colorHandler.numLeds <= 0) return;

    phase += 23; // velocidad moderada

    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds) {
        uint8_t t = (sin8(phase) + cos8(phase / 2)) / 2;
        CRGB c = lerpColor_u(color1, color2, t);
        colorHandler.leds[ledIndex] = c;
        return;
    }

    // Tira completa: ondas superpuestas que se deslizan
    for (int i = 0; i < colorHandler.numLeds; ++i) {
        uint8_t s1 = sin8(phase + i * 7);     // onda lenta
        uint8_t s2 = cos8((phase / 2) + i * 13); // componente secundaria
        uint8_t t  = (uint16_t(s1) * 3 + uint16_t(s2)) >> 2; // mezcla ponderada
        // Suavizado adicional
        t = ease8InOutCubic_u(t);
        colorHandler.leds[i] = lerpColor_u(color1, color2, t);
    }
}

/* ============================================================
 *    STROBE - Estroboscópico
 * ============================================================ */
 StrobeEffect::StrobeEffect(COLORHANDLER_& handler, int index, CRGB strobeColor, 
                           unsigned int updateMs, uint8_t flashes)
: DynamicEffect(handler, index, updateMs),
  color(strobeColor),
  flashCount(std::max<uint8_t>(1, flashes)),
  currentFlash(0),
  isOn(false),
  flashDuration(std::max<unsigned long>(updateMs, 20UL)), // destello corto
  pauseDuration(updateMs * 6UL), // pausa entre ráfagas
  phaseTimer(0) {}

void StrobeEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    unsigned long dt = now - lastUpdate;
    lastUpdate = now;

    phaseTimer += dt;

    if (currentFlash < flashCount) {
        // En secuencia de destellos
        if (!isOn) {
            // Encender flash
            writeSingleOrAll(colorHandler, ledIndex, color);
            isOn = true;
            phaseTimer = 0;
        } else {
            // Mantener encendido hasta flashDuration
            if (phaseTimer >= flashDuration) {
                // Apagar y preparar el siguiente
                writeSingleOrAll(colorHandler, ledIndex, CRGB::Black);
                isOn = false;
                currentFlash++;
                phaseTimer = 0;
            }
        }
    } else {
        // Pausa entre ráfagas
        if (phaseTimer >= pauseDuration) {
            currentFlash = 0;
            isOn = false;
            phaseTimer = 0;
        } else {
            // Asegurar apagado durante la pausa
            writeSingleOrAll(colorHandler, ledIndex, CRGB::Black);
        }
    }
}
 /* ============================================================
 *    COLOR_WIPE - Barrido de color
 * ============================================================ */
 ColorWipeEffect::ColorWipeEffect(COLORHANDLER_& handler, int index, CRGB wipeColor, 
                                 unsigned int updateMs, bool fillMode)
: DynamicEffect(handler, index, updateMs), color(wipeColor), currentLed(0), direction(1), fill(fillMode) {}

void ColorWipeEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    if (!colorHandler.leds || colorHandler.numLeds <= 0) return;

    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds) {
        // Caso LED único: simple pulso de encendido secuencial (on/off)
        static bool on = false;
        colorHandler.leds[ledIndex] = on ? color : CRGB::Black;
        on = !on;
        return;
    }

    if (fill) {
        // Pintado progresivo
        // Si es el primer LED, limpiar al reiniciar ciclo
        if (currentLed == 0) fill_solid(colorHandler.leds, colorHandler.numLeds, CRGB::Black);

        if (currentLed < colorHandler.numLeds) {
            colorHandler.leds[currentLed] = color;
            currentLed++;
        } else {
            // Lleno -> limpiar y reiniciar
            fill_solid(colorHandler.leds, colorHandler.numLeds, CRGB::Black);
            currentLed = 0;
        }
    } else {
        // Pixel viajero
        fill_solid(colorHandler.leds, colorHandler.numLeds, CRGB::Black);
        colorHandler.leds[currentLed] = color;

        currentLed += direction;
        if (currentLed >= colorHandler.numLeds) currentLed = 0;
        if (currentLed < 0) currentLed = colorHandler.numLeds - 1;
    }
}

 /* ============================================================
 *    THEATER_CHASE - Persecución teatral
 * ============================================================ */
TheaterChaseEffect::TheaterChaseEffect(COLORHANDLER_& handler, int index, CRGB chaseColor, 
                                       unsigned int updateMs, uint8_t space)
: DynamicEffect(handler, index, updateMs), color(chaseColor), spacing(std::max<uint8_t>(2, space)), offset(0) {}

void TheaterChaseEffect::update() {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    if (!colorHandler.leds || colorHandler.numLeds <= 0) return;

    if (ledIndex >= 0 && ledIndex < colorHandler.numLeds) {
        // Para un único LED, alterna on/off con patrón
        static bool on = false;
        colorHandler.leds[ledIndex] = on ? color : CRGB::Black;
        on = !on;
        return;
    }

    fill_solid(colorHandler.leds, colorHandler.numLeds, CRGB::Black);

    for (int i = offset; i < colorHandler.numLeds; i += spacing) {
        colorHandler.leds[i] = color;
    }

    offset++;
    if (offset >= spacing) offset = 0;
}

/* ============================================================
 *    DynamicLEDManager
 * ============================================================ */
DynamicLEDManager::DynamicLEDManager(COLORHANDLER_& handler)
    : colorHandler(handler)
{
}

DynamicLEDManager::~DynamicLEDManager() {
    clearEffects();
}

void DynamicLEDManager::addEffect(DynamicEffect* effect) {
    if (!effect) return;
    effects.push_back(effect);
}

void DynamicLEDManager::clearEffects() {
    for (auto* e : effects) delete e;
    effects.clear();
}

void DynamicLEDManager::update() {
    // Actualizar todos los efectos
    for (auto* e : effects) {
        if (e) e->update();
    }
    // Render
    if (colorHandler.leds) {
        FastLED.show();
    }
}

// Nota: la definición del objeto global 'ledManager' debe estar
// en un .cpp único de tu proyecto (no aquí) si ya lo estás usando.
// Aquí solo lo declaramos vía 'extern' en el .h.
