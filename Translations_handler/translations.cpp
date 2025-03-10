#include "translations.h"

// Inicialización del idioma actual (por defecto ES)
Language currentLanguage = Language::ES;

// Array de traducciones. Puedes agregar más cadenas según tus necesidades.
const Translation translations[] = {
  // Clave               ES                           ES(MX)                      CA                        EU                            FR                                   DE                             EN                            IT
  {"MENU_AJUSTES",       "AJUSTES",                   "AJUSTES",                  "AJUSTOS",                "EZKARRIA",                   "REGLAGES",                          "EINSTELLUNGEN",               "SETTINGS",                   "IMPOSTAZIONI"},
  {"MODOS",              "MODOS",                     "MODOS",                    "MODES",                  "MODUAK",                     "MODES",                             "MODI",                        "MODES",                      "MODALITA"},
  {"ENCENDER",           "Encender",                  "Encender",                 "Encendre",               "Piztu",                      "Allumer",                           "Einschalten",                 "Turn On",                    "Accendere"},
  {"APAGAR",             "Apagar",                    "Apagar",                   "Apagar",                 "Itzal",                      "Eteindre",                          "Ausschalten",                 "Turn Off",                   "Spegnere"},
  {"BUSCAR_ELEMENTO",    "Buscar elemento",           "Buscar elemento",          "Cercar element",         "Bilatu elementua",           "Rechercher element",                "Element suchen",              "Search element",             "Cerca elemento"},
  {"IDIOMA",             "Idioma",                    "Idioma",                   "Llengua",                "Hizkuntza",                  "Langue",                            "Sprache",                     "Language",                   "Lingua"},
  {"SONIDO",             "Sonido",                    "Sonido",                   "So",                     "Soinu",                      "Son",                               "Ton",                         "Sound",                      "Suono"},
  {"BRILLO",             "Brillo",                    "Brillo",                   "Brillantor",             "Distira",                    "Luminosite",                        "Helligkeit",                  "Brightness",                 "Luminosita"},
  {"FORMATEAR",          "Formatear",                 "Formatear",                "Formatar",               "Formatatu",                  "Formater",                          "Formatieren",                 "Format",                     "Formatta"},
  {"VOLVER",             "Volver",                    "Regresar",                 "Tornar",                 "Itzuli",                     "Retour",                            "Zurück",                      "Back",                       "Indietro"},
  {"APAGANDO_ELEMENTO",  "Apagando elemento...",      "Apagando elemento...",     "Apagant l'element...",   "Elementua itzaltzen...",     "Extinction de l'element...",        "Element ausschalten...",      "Turning off element...",     "Spegnendo elemento..."},
  {"APAGANDO_SALA",      "Apagando Sala...",          "Apagando Sala...",         "Apagant la Sala...",     "Gela itzaltzen...",          "Extinction de la Salle...",         "Raum ausschalten...",         "Turning off Room...",        "Spegnendo la Stanza..."},
  {"AMBIENTES",          "Ambientes",                 "Ambientes",                "Ambients",               "Ambienteak",                 "Ambiances",                         "Ambiente",                    "Ambiences",                  "Ambienti"},
  {"FICHAS",             "Fichas",                    "Fichas",                   "Fitxes",                 "Fitxak",                     "Fiches",                            "Fichas",                      "Tokens",                     "Gettoni"},
  {"APAGAR",             "Apagar",                    "Apagar",                   "Apagar",                 "Itzal",                      "Eteindre",                          "Ausschalten",                 "Turn Off",                   "Spegnere"},
  {"BASICO",             "BASICO",                    "BASICO",                   "BASIC",                  "BASIQUE",                    "BASIC",                             "BASIC",                       "BASIC",                      "BASE"},
  {"PAREJAS",            "PAREJAS",                   "PAREJAS",                  "PARELLES",               "BIKOIZKAKO",                 "COUPLES",                           "PAARE",                       "COUPLES",                    "COPPIE"},
  {"ADIVINAR",           "ADIVINAR",                  "ADIVINAR",                 "ENDEVINAR",              "ASMATU",                     "DEVINER",                           "RATEN",                       "GUESS",                      "INDOVINARE"},
  {nullptr,              nullptr,                     nullptr,                    nullptr,                  nullptr,                      nullptr,                             nullptr,                       nullptr,                      nullptr} // Entrada final
};

const char* getTranslation(const char* key) {
  for (int i = 0; translations[i].key != nullptr; i++) {
    if (strcmp(translations[i].key, key) == 0) {
      switch (currentLanguage) {
        case Language::ES:     return translations[i].es;
        case Language::ES_MX:  return translations[i].es_mx;
        case Language::CA:     return translations[i].ca;
        case Language::EU:     return translations[i].eu;
        case Language::FR:     return translations[i].fr;
        case Language::DE:     return translations[i].de;
        case Language::EN:     return translations[i].en;
        case Language::IT:     return translations[i].it;
        default:               return translations[i].es;
      }
    }
  }
  // Si no se encuentra la clave, se retorna la misma clave
  return key;
}
