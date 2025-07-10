
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "root/1_definitions.h"
#include "root/2_errorHandling.h"



enum class phase{
  IDLE,
  PLATFORM_SET,
  PLATFORM_HIT_LDR,
  STAMP_DOWN_INIT,
  STAMP_DOWN_WAIT,
  STAMP_UP_WAIT,
  FRONT_WAIT,
  DOWN_WAIT,
  MOVE_GAP,
  UP_WAIT,
  BACK_WAIT
};

phase nextPhase = phase::IDLE;
phase savePhase = phase::IDLE;

#include "allMotor.h"

const unsigned long STAMP_UP_DOWN_TIMEOUT = 800;
const unsigned long FRONT_TIMEOUT = 1000;
const unsigned long PICK_DOWN_DELAY = 1000;
const unsigned long STICK_DELAY = 500;//1000;
unsigned long phaseStartTime = 0;


void changeState(phase ign, String logMsg="", bool resetTimer=false){
  Serial.println(logMsg);
  savePhase=nextPhase;
  nextPhase=ign;
  if (resetTimer) {phaseStartTime=millis();}
}

bool _flag_down=false;

void do_things(){
  unsigned long elapsedTime= millis() - phaseStartTime;

  if(!readLMT_BCK()){               //if LMS pressed when its not supposed to.
    switch(nextPhase){
      case phase::IDLE:
      case phase::PLATFORM_SET:
      case phase::PLATFORM_HIT_LDR:
      case phase::STAMP_DOWN_INIT:
      case phase::STAMP_DOWN_WAIT:
      case phase::STAMP_UP_WAIT:
                                nextPhase==phase::IDLE;
                                flag=error::HINDERANCE_FRONT;
                                break;}}

  if(!readLMT_UP()){               //if LMS pressed when its not supposed to.
    switch (nextPhase) {
      case phase::IDLE:
      case phase::PLATFORM_SET:
      case phase::PLATFORM_HIT_LDR:
      case phase::FRONT_WAIT:
      case phase::DOWN_WAIT:
      case phase::MOVE_GAP:
      case phase::UP_WAIT:
      case phase::BACK_WAIT:
                            nextPhase==phase::IDLE;
                            flag=error::HINDERANCE_UP;
                            break;}}

  switch(nextPhase){
    case phase::IDLE:
                      if(savePhase!=phase::IDLE){
                        detachLDR=false;
                        stampUp;
                        stickyOFF;
                        pickUp;
                        delay(50);
                        back;
                        newSpeed(80);
                        savePhase=phase::IDLE;}
                      break;
    case phase::PLATFORM_SET:
                      if(savePhase==phase::BACK_WAIT){
                        detachLDR=false;
                        //newSpeed(300);
                        if(readLDR()){changeState(phase::PLATFORM_HIT_LDR, "finding stack height.");}
                        if(!readLDR() && !encLeft.isBusy() && !encRight.isBusy()){movePlatform(-5);break;}
                      }
                      break;
    case phase::PLATFORM_HIT_LDR:
                      if(savePhase==phase::PLATFORM_SET){
                        if(readLDR() && motionState == motionClass::IDLE && !encLeft.isBusy() && !encRight.isBusy()){movePlatform(5);}
                        if(!readLDR() && motionState == motionClass::LDR_STOP){changeState(phase::STAMP_DOWN_INIT, "PLATFORM level");}
                      }
                      break;
    case phase::STAMP_DOWN_INIT:
                      if(savePhase==phase::PLATFORM_HIT_LDR && readLMT_BCK() && motionState == motionClass::LDR_STOP){ // or LDR_STOP
                        detachLDR=true;
                        stampDown;
                        changeState(phase::STAMP_DOWN_WAIT, "STAMP_DOWN_INIT",true);}
                      break;
    case phase::STAMP_DOWN_WAIT:
                      if(savePhase==phase::STAMP_DOWN_INIT){
                          if(!readLMT_UP()){
                              stampUp;
                              changeState(phase::STAMP_UP_WAIT, "STAMP_DOWN_WAIT",true);}
                          if(elapsedTime > STAMP_UP_DOWN_TIMEOUT && flag!=error::STAMP_NOT_DOWN){
                              nextPhase = phase::IDLE;
                              flag=error::STAMP_NOT_DOWN;}}
                      break;
    case phase::STAMP_UP_WAIT:
                      if(savePhase==phase::STAMP_DOWN_WAIT){
                          if(elapsedTime > STAMP_UP_DOWN_TIMEOUT){
                              if(!readLMT_UP()){ //thing is still down.
                                if(flag!=error::STAMP_NOT_UP){
                                  nextPhase = phase::IDLE;
                                  flag=error::STAMP_NOT_UP;
                                }}
                              else{
                                front;
                                changeState(phase::FRONT_WAIT, "STAMP_UP_WAIT",true);}}}
                      break;
    case phase::FRONT_WAIT:
                     if(savePhase==phase::STAMP_UP_WAIT){
                        if(!readLMT_BCK()){
                            pickDown;
                            stickyON;
                            changeState(phase::DOWN_WAIT, "FRONT_WAIT",true);
                          }
                        if(elapsedTime > FRONT_TIMEOUT && flag!=error::STICK_NO_FRONT){
                            nextPhase = phase::IDLE;
                            flag=error::STICK_NO_FRONT;}}
                      break;
    case phase::DOWN_WAIT:
                      if(savePhase==phase::FRONT_WAIT){
                        if(elapsedTime > 0){changeState(phase::MOVE_GAP, "down wait done");}//PICK_DOWN_DELAY
                      }
                      break;
    case phase::MOVE_GAP:
                      if(savePhase==phase::DOWN_WAIT){
                      if(!encLeft.isBusy() && !encRight.isBusy() && !_flag_down){movePlatform(-5);_flag_down = true;}
                      if(!encLeft.isBusy() && !encRight.isBusy() && _flag_down){
                        _flag_down=false;
                        detachLDR=true;
                        pickUp;
                        changeState(phase::UP_WAIT, "ok",true);}
                      }
                      break;
    case phase::UP_WAIT:
                      if(savePhase==phase::MOVE_GAP && elapsedTime > STICK_DELAY){
                            back;
                            changeState(phase::BACK_WAIT, "UP_WAIT",true);}
                      break;
    case phase::BACK_WAIT:
                    if(savePhase==phase::UP_WAIT){
                        if(elapsedTime > FRONT_TIMEOUT){
                          stickyOFF;
                          if(readLDR() && motionState==motionClass::LDR_STOP){motionState=motionClass::IDLE;}
                          detachLDR=false;
                          changeState(phase::PLATFORM_SET, "BACK_WAIT");

                          if(!readLMT_BCK() && flag!=error::STICK_NO_BACK){
                              nextPhase= phase::IDLE;
                              flag=error::STICK_NO_BACK;}}}
                      break;
  }
}


#endif //STATE_MACHINE_H
