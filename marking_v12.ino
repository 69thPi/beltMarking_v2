#include "common.h"


void setup() {
  pinMode(R_POWER,OUTPUT);
  powerON;
  Serial.begin(115200);
  declarations();
  checkMode();
 }

void loop(){ //NO DELAYS
  if(pressureCheck()){

        if(buttonPressed_up()){Serial.println("## EVENT ## buttonPressed_UP");doThings_upButton();}

        if(buttonPressed_down()){Serial.println("## EVENT ## buttonPressed_DOWN");doThings_downButton();}

        evaluateLDR();

        checkMotion();

        if(buttonPressed_play() && btnMid==false){Serial.println("## EVENT ## buttonPressed_play");btnMid=true;}

        if(btnMid){doThings_midButton(startup);}

        do_things();}
  display_errors();
}
