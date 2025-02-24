#pragma once

#ifndef ELEMENT_DMS_H
#define ELEMENT_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <stdint.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <FastLED.h>
#include <vector>
#include <Colors_DMS/Color_DMS.h>
#include <map>
#include <algorithm>
#include <Colors_DMS/Color_DMS.h>

extern byte globalID;
extern bool colorReceived;


class ELEMENT_ {

    public: 

        enum EVENT_TYPE{
            EV_START,
            EV_END,
            EV_MODE_CHANGE,
            EV_COLOR_CHANGE,
            EV_FLAG_CHANGE,
            EV_PATTERN_CHANGE,
            EV_ID_CHANGE
        };
        struct EventLogEntry {
            EVENT_TYPE eventType;
            int value;
            unsigned long timestamp;
            unsigned long duration; // en milisegundos, o -1 si no aplica
        };
        struct Statistics {
            unsigned long totalSessionTime;
            int mostUsedColor;
            int mostUsedPattern; // ADDED: Most Used Pattern
            int mostUsedMode;
            unsigned int totalSessions; // ADDED: Total Sessions Count
            std::map<int, unsigned long> accumulatedColorDurations;
            std::map<int, unsigned long> accumulatedPatternDurations;
            std::map<int, unsigned long> accumulatedModeDurations;

            Statistics() : totalSessionTime(0), mostUsedColor(-1), mostUsedPattern(-1), mostUsedMode(-1), totalSessions(0) {} // Constructor por defecto
        };
        std::vector<EventLogEntry> eventLog;
        unsigned long sessionStartTime;
        unsigned long lastTimedEventStartTime;
        EVENT_TYPE lastTimedEventType;
        int lastTimedEventValue;
        bool sessionActive;
        uint8_t  ID;
        byte activePattern;


        ELEMENT_() : sessionStartTime(0), lastTimedEventStartTime(0), lastTimedEventType(EV_START), lastTimedEventValue(-1), sessionActive(false), errDbg(false) {}

        void   begin();
        void   configurar_RF(int baudRate);

        void       event_register(EVENT_TYPE event, int value);
        bool       save_register();
        void       print_event_register_file();
        void       print_event_register_file_RF();
        void       print_stats_file_RF();
        void       print_statistics_file();
    Statistics     calculate_statistics();
        int        find_max_duration_value(const std::map<int, unsigned long>& durationMap);
        bool       save_statistics(const Statistics& stats);
        void       calculate_and_save_statistics();
    Statistics     load_statistics();
        String     get_statistic(String statisticName);
        void       handle_timed_event(EVENT_TYPE event, int value, unsigned long currentTimestamp);
        void       update_last_timed_event_duration(unsigned long duration);
        void       log_event(EVENT_TYPE event, int value, unsigned long timestamp, unsigned long duration);
        void       print_event_log();
        int        get_most_used_color();
        int        get_most_used_mode();
        int         get_most_used_pattern(); // ADDED: getter for most used pattern
    unsigned long  get_total_session_time();
    unsigned int   get_total_sessions(); // ADDED: getter for total sessions
        String     format_millis_time(unsigned long millisec);
        String     padStringRight(String text, int width);

        String     get_serial_from_file();
        String     get_mode_name(byte numMode);

        void      set_ID_to_file(byte ID);
        void      set_ID(byte IDin);
        byte      get_ID_from_file();
        byte      get_ID();
  
        void      set_type(byte typein);
        byte      get_type();

        void      set_active_pattern(byte patt);

        void      set_manager(byte managerin);
        byte      get_manager();

        void      set_mode(uint8_t mode);
        byte      get_currentMode();

        void      set_err_dbg(bool op){errDbg= op;}
        bool      get_err_dbg()       {return errDbg;}

        void      set_flag(byte flagNum, bool state);
        byte      get_flag();

    protected:
        virtual void  RX_main_handler(LAST_ENTRY_FRAME_T LEF){}

  
        byte     flag;
        uint8_t  currentMode;  
        byte     type;
        byte     exclusiveIDmanager;  
        bool     errDbg;  

    private:
        const char* get_event_type_name(EVENT_TYPE event);

};


#endif