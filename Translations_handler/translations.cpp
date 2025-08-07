#include "translations.h"

// Inicialización del idioma actual (por defecto ES)
Language currentLanguage = Language::ES;

// Array de traducciones. Puedes agregar más cadenas según tus necesidades.
const Translation translations[] = {
  // Clave               ES                           ES(MX)                      CA                                EU                                    FR                                   DE                             EN                            IT
  {"MENU_AJUSTES",       "AJUSTES",                   "AJUSTES",                  "CONFIG",                         "EZKARRIA",                           "REGLAGES",                          "EINSTELLUNGEN",               "SETTINGS",                   "IMPOSTAZIONI"},
  {"MODOS",              "MODOS",                     "MODOS",                    "MODES",                          "MODUAK",                             "MODES",                             "MODI",                        "MODES",                      "MODALITA"},
  {"ENCENDER",           "Encender",                  "Encender",                 "Encendre",                       "Piztu",                              "Allumer",                           "Einschalten",                 "Turn On",                    "Accendere"},
  {"APAGAR",             "Apagar",                    "Apagar",                   "Apagar",                         "Itzal",                              "Eteindre",                          "Ausschalten",                 "Turn Off",                   "Spegnere"},
  {"UPDATE_SALA",        "Actualizar sala",           "Actualizar sala",          "Actualitzar sala",               "Gela eguneratu",                     "Mettre a jour la salle",            "Raum aktualisieren",          "Update room",                "Aggiornare la stanza"},
  {"IDIOMA",             "Idioma",                    "Idioma",                   "Llengua",                        "Hizkuntza",                          "Langue",                            "Sprache",                     "Language",                   "Lingua"},
  {"SONIDO",             "Sonido",                    "Sonido",                   "So",                             "Soinu",                              "Son",                               "Ton",                         "Sound",                      "Suono"},
  {"BRILLO",             "Brillo",                    "Brillo",                   "Lluminositat",                   "Distira",                            "Luminosite",                        "Helligkeit",                  "Brightness",                 "Luminosita"},
  {"FORMATEAR",          "Formatear",                 "Formatear",                "Formatar",                       "Formatatu",                          "Formater",                          "Formatieren",                 "Format",                     "Formatta"},
  {"VOLVER",             "Volver",                    "Regresar",                 "Tornar",                         "Itzuli",                             "Retour",                            "Zurück",                      "Back",                       "Indietro"},
  {"APAGANDO_ELEMENTO",  "Apagando elemento...",      "Apagando elemento...",     "Apagant l'element...",           "Elementua itzaltzen...",             "Extinction de l'element...",        "Element ausschalten...",      "Turning off element...",     "Spegnendo elemento..."},
  {"APAGANDO_ELEMENTOS", "Apagando elementos...",     "Apagando elementos...",    "Apagant l'elements...",          "Elementuak itzaltzen...",            "Extinction de l'elements...",       "Elements ausschalten...",     "Turning off elements...",    "Spegnendo elementos..."},
  {"APAGANDO_SALA",      "Apagando Sala...",          "Apagando Sala...",         "Apagant la Sala...",             "Gela itzaltzen...",                  "Extinction de la Salle...",         "Raum ausschalten...",         "Turning off Room...",        "Spegnendo la Stanza..."},
  {"AMBIENTES",          "Ambientes",                 "Ambientes",                "Ambients",                       "Ambienteak",                         "Ambiances",                         "Ambiente",                    "Ambiences",                  "Ambienti"},
  {"FICHAS",             "Fichas",                    "Fichas",                   "Fitxes",                         "Fitxak",                             "Fiches",                            "Fichas",                      "Tokens",                     "Gettoni"},
  {"BASICO",             "BASICO",                    "BASICO",                   "BASIC",                          "BASIQUE",                            "BASIC",                             "BASIC",                       "BASIC",                      "BASE"},
  {"PAREJAS",            "PAREJAS",                   "PAREJAS",                  "PARELLES",                       "BIKOIZKAKO",                         "COUPLES",                           "PAARE",                       "COUPLES",                    "COPPIE"},
  {"ADIVINAR",           "ADIVINAR",                  "ADIVINAR",                 "ENDEVINAR",                      "ASMATU",                             "DEVINER",                           "RATEN",                       "GUESS",                      "INDOVINARE"},
  {"MUJER",              "Mujer",                     "Mujer",                    "Dona",                           "Emakumea",                           "Femme",                             "Frau",                        "Woman",                      "Donna"},
  {"HOMBRE",             "Hombre",                    "Hombre",                   "Home",                           "Gizona",                             "Homme",                             "Mann",                        "Man",                        "Uomo"},
  {"CON_NEG",            "Con respuesta negativa",    "Con respuesta negativa",   "Amb resposta negativa",          "Erantzun negatiboarekin",            "Avec réponse négative",             "Mit negativer Antwort",       "With negative resp.",        "Con risposta negativa"},
  {"SIN_NEG",            "Sin respuesta negativa",    "Sin respuesta negativa",   "Sense resposta negativa",        "Erantzun negatiborik gabe",          "Sans réponse négative",             "Ohne negative Antwort",       "Without negative resp.",     "Senza risposta negativa"},
  {"VOL_NORMAL",         "Volumen normal",            "Volumen normal",           "Volum normal",                   "Bolumen normala",                    "Volume normal",                     "Normale Lautstärke",          "Normal volume",              "Volume normale"},
  {"VOL_ATENUADO",       "Volumen atenuado",          "Volumen atenuado",         "Volum atenuat",                  "Bolumen ahuldua",                    "Volume atténué",                    "Gedämpfte Lautstärke",        "Soft volume",                "Volume attenuato"},
  {"AJUSTES_SONIDO",     "SONIDO",                    "SONIDO",                   "SO",                             "SOINU",                              "SON",                               "TON",                         "SOUND",                      "SUONO"},
  {"CONFIRMAR",          "Confirmar",                 "Confirmar",                "Confirmar",                      "Berretsi",                          "Confirmer",                          "Bestätigen",                   "Confirm",                    "Conferma"},
  {"CONTROL_SALA_MENU",  "Control",                   "Formatear",                "Formatejar",                     "Formateatu",                         "Formater",                          "Formatieren",                 "Format",                     "Formattare"},
  {"DELETE_ELEMENT_MENU","Eliminar",                  "Eliminar",                 "Eliminar",                       "Elementua",                          "Supprimer",                         "Löschen",                     "Delete",                     "Elimina"},
  {"DELETE_ELEMENT",     "Eliminar elemento",         "Eliminar elemento",        "Eliminar element",               "Elementua ezabatu",                  "Supprimer élément",                 "Element löschen",             "Delete element",             "Elimina elemento"},
  {"FORMATEAR",          "Formatear",                 "Formatear",                "Formatear",                      "Berreskuratu",                       "Formater",                          "Formatieren",                 "Format",                     "Formattare"},
  {"CONFIRM_DELETE",     "Confirmar",                 "Confirmar",                "Confirmar",                      "Berretsi",                           "Confirmer",                         "Bestätigen",                  "Confirm",                    "Conferma"},
  {"CANCEL",             "Cancelar",                  "Cancelar",                 "Cancel·lar",                     "Ezeztatu",                           "Annuler",                           "Abbrechen",                   "Cancel",                     "Annulla"},
  {"YES_DELETE",         "Si",                        "Si",                       "Si",                             "Bai",                                "Oui",                               "Ja",                          "Yes",                        "Si"},
  {"ACTIVIDADES",        "Actividades",               "Actividades",              "Activitats",                     "Jarduerak",                          "Activites",                         "Aktivitaten",                 "Activities",                 "Attivita"},
  {"COGNITIVAS",         "Cognitivas",                "Cognitivas",               "Cognitives",                     "Kognitiboak",                        "Cognitives",                        "Kognitive",                   "Cognitive",                  "Cognitive"},
  {"SALIR",              "Salir",                     "Salir",                    "Sortir",                         "Irten",                              "Sortir",                            "Verlassen",                   "Exit",                       "Esci"},
  {"SHOW_ID",            "Mostrar ID",                "Mostrar ID",               "Mostrar ID",                     "ID erakutsi",                        "Afficher l'ID",                     "ID anzeigen",                 "Show ID",                    "Mostra ID"},
  {"RESTORE_ELEM",       "Restablecer elementos",     "Restablecer elementos",    "Restableix elements",            "Berreskuratu elementuak",            "Restaurer les éléments",            "Elemente wiederherstellen",   "Restore elements",           "Ripristina elementi"},
  {"RESTORE_ELEM",       "Restablecer elementos",     "Restablecer elementos",    "Restableix elements",            "Berreskuratu elementuak",            "Restaurer les éléments",            "Elemente wiederherstellen",   "Restore elements",           "Ripristina elementi"},
  { "UPDATING_1_2",      "Actualizando (1/2)",        "Actualizando (1/2)",       "Actualitzant (1/2)",             "Eguneratzen (1/2)",                  "Mise à jour (1/2)",                 "Aktualisiere (1/2)",          "Updating (1/2)",             "Aggiornamento (1/2)" },
  { "UPDATING_2_2",      "Actualizando (2/2)",        "Actualizando (2/2)",       "Actualitzant (2/2)",             "Eguneratzen (2/2)",                  "Mise à jour (2/2)",                 "Aktualisiere (2/2)",          "Updating (2/2)",             "Aggiornamento (2/2)" },
  { "SEARCHING",         "Buscando",                  "Buscando",                 "Cercant",                        "Bilatzen",                           "Recherche",                         "Suche",                       "Searching",                  "Ricerca" },
  { "SUCCESS",           "EXITO",                     "EXITO",                    "EXIT",                           "ARRAKASTA",                          "SUCCES",                            "ERFOLG",                      "SUCCESS",                    "SUCCESSO" },
  { "ROOM_UPDATED",      "Sala actualizada",          "Sala actualizada",         "Sala actualitzada",              "Gela eguneratua",                    "Salle mise a jour",                 "Raum aktualisiert",           "Room updated",               "Stanza aggiornata" },
  {"COMUNICADOR",        "Comunicador",               "Comunicador",              "Comunicador",                    "Komunikatzailea",                    "Communicateur",                     "Kommunikator",                "Communicator",               "Comunicatore"},
  {nullptr,              nullptr,                     nullptr,                    nullptr,                          nullptr,                              nullptr,                             nullptr,                       nullptr,                      nullptr} // Entrada final
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
