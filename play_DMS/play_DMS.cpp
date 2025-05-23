#include <play_DMS/play_DMS.h>




void DOITSOUNDS_::begin(){
    
  if (player.begin(Serial2)) {
    player.volume(26);
    player.playFolder(01, 8);
                                                                        #ifdef DEBUG
                                                                        //Serial.println("DFPlayer Mini inicializado");
                                                                        #endif
  } else {
                                                                        #ifdef DEBUG
                                                                        Serial.println("Error al inicializar DFPlayer Mini");
                                                                        #endif
  }
  delay(10);

}

void DOITSOUNDS_::play_file(byte bankin, byte filein){
  player.playFolder(bankin, filein);
}

void DOITSOUNDS_::stop_file(){
  player.stop();
}

bool DOITSOUNDS_::is_playing(){   // devuelve true mientras este sonando.
 
  return (player.readState() == 513);
}

void DOITSOUNDS_::get_available_folders(){
  int j= 0;
  for(int i= 1; i < 100; i++){

    if(player.readFileCountsInFolder(i)) availableFolders[j++]= i;
  }
}