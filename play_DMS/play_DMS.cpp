#include <play_DMS/play_DMS.h>


DOITSOUNDS_::DOITSOUNDS_(){

}

void DOITSOUNDS_::begin(){
    

  if (player.begin(Serial2)) {
    player.volume(30);
    player.playFolder(01, 001);
  } else {
                                                                                        #ifdef DEBUG
                                                                                        Serial.println("Error al inicializar DFPlayer Mini");                                                                               
                                                                                        #endif
  }
  delay(10);
  play_file(1,10);

}

void DOITSOUNDS_::play_file(byte bankin, byte filein){
    player.playFolder(bankin, filein);
}