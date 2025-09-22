#include <play_DMS/play_DMS.h>




void DOITSOUNDS_::begin(){
    
  if (player.begin(Serial2, false, true)) {
    delay(800);
    player.volume(24);
    player.playFolder(1, 9);
                                                                        #ifdef DEBUG
                                                                        //DEBUG__________ln("DFPlayer Mini inicializado");
                                                                        #endif
  } else {
                                                                        #ifdef DEBUG
                                                                        DEBUG__________ln("Error al inicializar DFPlayer Mini");
                                                                        #endif
  }
  delay(10);

}

void DOITSOUNDS_::play_file(byte bankin, byte filein){
  player.stop();
  delay(30);              // 20–50 ms suele bastar
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