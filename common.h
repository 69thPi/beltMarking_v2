
int detachLDR=false;
bool startup=false;
bool btnMid=false;
bool stopAllCommand=false;
bool endDown=false;

#include "bin/stateMachine.h"

void checkMode(){
  startup=false;
  if(togglePressed_mode()){startup=true;}
}

void evaluateLDR(){
  if(!detachLDR && !readLDR() && motionState!=motionClass::LDR_ZONE){
        if(motionState==motionClass::LDR_STOP){return;}/////////////
        Serial.println("ldr zone");
        motionState=motionClass::LDR_ZONE;
        correctionState=correctionClass::NONE;
        encLeft.stopMotor();
        encLeft.setTarget(encLeft.getPos());
        encRight.stopMotor();
        encRight.setTarget(encRight.getPos());
        detachLDR=true;}
}

void doThings_midButton(bool option=false){
      if(stopAllCommand){
        if(motionState== motionClass::IDLE
          || motionState == motionClass::LDR_STOP
          || motionState == motionClass::LDR_ZONE){

              // if(motionState == motionClass::LDR_ZONE){powerOFF;}
              if(!encLeft.isBusy() && !encRight.isBusy()){
                      if(!endDown){movePlatform(-5);endDown=true;}
                      if(endDown && motionState == motionClass::IDLE){powerOFF;}}
      }}

      if(!option){
          if(nextPhase!=phase::IDLE){    
                nextPhase=phase::IDLE;
                stickyOFF;
                stampUp;
                pickUp;
                delay(500);
                back;
                powerOFF;
                stopAllCommand=true;}
          else{
                delay(800);
                nextPhase = phase::PLATFORM_SET;
                savePhase = phase::BACK_WAIT;
                phaseStartTime = 0;
                btnMid=false;}
      }

      if(option){
        digitalWrite(R_BLUE_STAMP,!digitalRead(R_BLUE_STAMP));
        delay(1000);
        btnMid=false;}

    }

////////////////////////////////////////////////////////////////////

void doThings_upButton(){
  if(savePhase==phase::IDLE && !encLeft.isBusy()  && !encRight.isBusy()){
    if(motionState ==  motionClass::IDLE || motionState == motionClass::LIMIT_HOME){
      movePlatform(20);}}
}

void doThings_downButton(){
  if(savePhase==phase::IDLE && !encLeft.isBusy() && !encRight.isBusy()){
    if( motionState ==  motionClass::IDLE|| motionState == motionClass::LDR_STOP
    || motionState == motionClass::LIMIT_LEFT || motionState == motionClass:: LIMIT_RIGHT)
    {movePlatform(-20);}}
}

void declarations() {

   //relays
  pinMode(R_BLUE_STAMP,OUTPUT);
  stampUp;
  pinMode(R_RED_SLIDE,OUTPUT);
  back;
  pinMode(R_GREEN_PICK,OUTPUT);
  pickUp;
  pinMode(R_STICK,OUTPUT);
  stickyOFF;

  //inputs
  pinMode(LEVEL_LDR,INPUT);

  pinMode(PSI_TX,INPUT);

  pinMode(LMT_UP,INPUT);
  pinMode(LMT_BCK,INPUT);
  pinMode(STAMP_TGL,INPUT);

  pinMode(PAUSE_PLAY,INPUT);
  pinMode(P_DWN,INPUT);
  pinMode(P_UP,INPUT);

  _init_motion();
}

////////////////////////////////////////////////////////////////////
