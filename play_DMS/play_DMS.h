#pragma once
#ifndef PLAY_DMS_H
#define PLAY_DMS_H

#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Arduino.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include "DFRobotDFPlayerMini.h"

//DFRobotDFPlayerMini player;
// test de commit
class DOITSOUNDS_ : public ELEMENT_{

    public:
        DOITSOUNDS_();
        DFRobotDFPlayerMini player;
        
        void begin();
        bool readCard();
        void play_file(byte bankin, byte filein);

};



#endif