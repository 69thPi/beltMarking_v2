
#ifndef ALL_MOTOR_H
#define ALL_MOTOR_H

#include "root/3_motionHeader.h"

void changeMotion(String log,motionClass ign=motionClass::_DEFAULT , correctionClass ign1=correctionClass::_DEFAULT){
  if(ign!=motionClass::_DEFAULT){motionState = ign;}
  if(ign1!=correctionClass::_DEFAULT){correctionState = ign1;}
  Serial.println(log);
}

void IRAM_ATTR readLeftEncoder() {encLeft.countTicks();}

void IRAM_ATTR readRightEncoder() {encRight.countTicks();}

void IRAM_ATTR handleRightLMS(){
  encRight.pressedLMS();
  detachInterrupt(encRight.retPinLMS());
  }

void IRAM_ATTR handleLeftLMS(){
  encLeft.pressedLMS();
  detachInterrupt(encLeft.retPinLMS());
  }

void attachLMS(){
  attachInterrupt(digitalPinToInterrupt(encLeft.retPinLMS()), handleLeftLMS, FALLING);
  attachInterrupt(digitalPinToInterrupt(encRight.retPinLMS()), handleRightLMS, FALLING);
}

int roundMath(float a){
  if(a>=0){return int(a+0.5f);}
  else if(a<0){return int(a-0.5f);}
}

int ticks_to_pulse(int tck){
  return roundMath(float(10 * tck) / 36);
}

int pulse_to_ticks(int pulse){
  return roundMath(float(36 * pulse)/ 10);
}

int roundPulse(int a){
  return pulse_to_ticks(ticks_to_pulse(a));
}

void newSpeed(int ign){
  encLeft.setSpeed(ign);
  encRight.setSpeed(ign);
}

bool fromBottom=false;
bool fromTop=false;

void movePlatform(int mm){
      mm*=144;
      if(mm>0){
        if(!readLDR()){changeMotion("set before moving platform.",motionClass::LDR_STOP);}

        if(motionState==motionClass::LDR_STOP || motionState==motionClass::LIMIT_LEFT || motionState==motionClass::LIMIT_RIGHT){return;}

        if(motionState==motionClass::IDLE && correctionState==correctionClass::NONE){
              changeMotion("initiate upward motion",motionClass::MOVING_UP);
              encLeft.updateTarget(mm);
              encRight.updateTarget(mm);
        }
        if(motionState==motionClass::LIMIT_HOME){
              changeMotion("moving up from bottom",motionClass::MOVING_UP);
              encLeft.updateTarget(1440);
              encRight.updateTarget(1440);
              fromBottom=true;
        }
      }
      else if(mm<0){

        if(motionState==motionClass::LIMIT_HOME){return;}

        if(motionState==motionClass::LIMIT_LEFT){encRight.updateTarget(mm);}

        if(motionState==motionClass::LIMIT_RIGHT){encLeft.updateTarget(mm);}

        if(motionState==motionClass::IDLE){
              changeMotion("initiate downward motion",motionClass::MOVING_DOWN);
              encLeft.updateTarget(mm);
              encRight.updateTarget(mm);}

        if(motionState==motionClass::LDR_STOP){
              fromTop=true;
              detachLDR=true;
              changeMotion("initiate down from LDR",motionClass::MOVING_DOWN);
              encLeft.updateTarget(mm);
              encRight.updateTarget(mm);}
      }
}

void checkMotion(){

      if(motionState == motionClass::IDLE){return;}

      if(motionState==motionClass::LDR_ZONE && correctionState==correctionClass::NONE && !encLeft.isBusy() && !encRight.isBusy()){
        //if(motionState==motionClass::LDR_STOP){return;}
        detachLDR=false;
        int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
        Serial.println("MOVING UP LDR ZONE >> ");
        if(delta>0){changeMotion("LEFT UP MORE",motionClass::LDR_ZONE,correctionClass::LEFT);}
        if(delta<0){changeMotion("RIGHT UP MORE",motionClass::LDR_ZONE,correctionClass::RIGHT);}
        if(delta==0){changeMotion("level",motionClass::LDR_STOP,correctionClass::NONE);return;}
      }

      if(motionState==motionClass::LDR_ZONE && correctionState==correctionClass::LEFT && !encLeft.isBusy()){

        int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
        if(delta>0){
          newSpeed(80);
          encLeft.updateTarget(-1 * pulse_to_ticks(abs(delta)));
        }
        if(delta<=0){
          encLeft.setPos(roundPulse(encLeft.getPos()));
          encRight.setPos(roundPulse(encRight.getPos()));
          if(!readLDR()){
            changeMotion("backed down from ldr zone.",motionClass::IDLE,correctionClass::NONE);
            return;}
          if(readLDR()){changeMotion("...and ldr is still blocked.",motionClass::LDR_STOP,correctionClass::NONE);}

        }

      }

      if(motionState==motionClass::LDR_ZONE && correctionState==correctionClass::RIGHT && !encRight.isBusy()){

        int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
        if(delta<0){
          newSpeed(80);
          encRight.updateTarget(-1 * pulse_to_ticks(abs(delta)));}

        if(delta>=0){
          encLeft.setPos(roundPulse(encLeft.getPos()));
          encRight.setPos(roundPulse(encRight.getPos()));
          if(!readLDR()){
            changeMotion("backed out of ldr zone",motionClass::IDLE,correctionClass::NONE);
            return;}
          if(readLDR()){changeMotion("... and ldr is still blocked.",motionClass::LDR_STOP,correctionClass::NONE);}
        }
      }

      encLeft.evaluatePosition();
      encRight.evaluatePosition();

      if(!fromBottom){
          if(encLeft.pressedLMS() && motionState== motionClass::MOVING_DOWN){
            changeMotion("left limit reached.",motionClass::LIMIT_LEFT,correctionClass::NONE);}

          if(encLeft.pressedLMS() && (motionState== motionClass::LIMIT_RIGHT || motionState== motionClass::IDLE)){
            changeMotion("both home.",motionClass::LIMIT_HOME,correctionClass::NONE);}

          if(encRight.pressedLMS() && motionState== motionClass::MOVING_DOWN){
            changeMotion("right limit reached.",motionClass::LIMIT_RIGHT,correctionClass::NONE);}

          if(encRight.pressedLMS() && (motionState== motionClass::LIMIT_LEFT || motionState== motionClass::IDLE)){
            changeMotion("both home.",motionClass::LIMIT_HOME,correctionClass::NONE);}

      }

      if(motionState==motionClass::LIMIT_LEFT || motionState==motionClass::LIMIT_RIGHT ||motionState==motionClass::LIMIT_HOME || motionState==motionClass::LDR_STOP){return;}

      if(!encLeft.isBusy() && !encRight.isBusy() && correctionState == correctionClass::NONE){

        //part 1
        //if(motionState == motionClass::LIMIT_LEFT || motionState == motionClass::LIMIT_RIGHT){}
        //{ do nothing; }
        if(fromBottom){attachLMS();fromBottom=false;}
        if(fromTop){detachLDR=false;fromTop=false;}
        //CHECK FOR MISSED STEPS.
        if(motionState == motionClass::MOVING_UP){

          int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
          Serial.print("MOVING UP >> ");
          if(delta>0){changeMotion("LEFT UP MORE",motionClass::MOVING_UP,correctionClass::LEFT);}
          if(delta<0){changeMotion("RIGHT UP MORE",motionClass::MOVING_UP,correctionClass::RIGHT);}
          if(delta==0){
            if(fromTop){fromTop=false;}
            changeMotion("up null level platform",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
        }

        if(motionState == motionClass::MOVING_DOWN){

          int delta=ticks_to_pulse(encRight.getPos()) - ticks_to_pulse(encLeft.getPos());
          Serial.print("MOVING DOWN >> ");
          if(delta>0){changeMotion("LEFT DOWN MORE",motionClass::MOVING_DOWN,correctionClass::LEFT);}
          if(delta<0){changeMotion("RIGHT DOWN MORE",motionClass::MOVING_DOWN,correctionClass::RIGHT);}
          if(delta==0){
            changeMotion(" down null level platform",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
        }


      }

      if(motionState == motionClass::MOVING_UP && correctionState == correctionClass::LEFT && !encLeft.isBusy() && !encRight.isBusy()){

          int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
          if(delta>0){
            newSpeed(80);
            encLeft.updateTarget(-1 * pulse_to_ticks(abs(delta)));
          }
          if(delta<=0){
            changeMotion("up left correction successful",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
      }

      if(motionState == motionClass::MOVING_UP && correctionState == correctionClass::RIGHT && !encLeft.isBusy() && !encRight.isBusy()){

          int delta=ticks_to_pulse(encLeft.getPos()) - ticks_to_pulse(encRight.getPos());
          if(delta<0){
            newSpeed(80);
            encRight.updateTarget(-1 * pulse_to_ticks(abs(delta)));
          }

          if(delta>=0){
            changeMotion("up right correction successful",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
      }

      if(motionState == motionClass::MOVING_DOWN && correctionState == correctionClass::LEFT && !encLeft.isBusy() && !encRight.isBusy()){

          int delta=ticks_to_pulse(encRight.getPos()) -ticks_to_pulse(encLeft.getPos());
          if(delta>0){
            newSpeed(80);
            encLeft.updateTarget(1 * pulse_to_ticks(abs(delta)));
          }
          if(delta<=0){
            changeMotion("down left correction successful",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
      }

      if(motionState == motionClass::MOVING_DOWN && correctionState == correctionClass::RIGHT && !encLeft.isBusy() && !encRight.isBusy()){

          int delta=ticks_to_pulse(encRight.getPos()) - ticks_to_pulse(encLeft.getPos());
          if(delta<0){
            newSpeed(80);
            encRight.updateTarget(1 * pulse_to_ticks(abs(delta)));
          }
          if(delta>=0){
            changeMotion("down/right correction successful.",motionClass::IDLE,correctionClass::NONE);
            encLeft.setPos(roundPulse(encLeft.getPos()));
            encRight.setPos(roundPulse(encRight.getPos()));
          }
      }


}

void _init_motion(){

  pinMode(MOT_L_STEP, OUTPUT);
  pinMode(MOT_R_STEP, OUTPUT);
  pinMode(MOT_DIR, OUTPUT);
  pinMode(LMT_PT_L,INPUT);
  pinMode(LMT_PT_R,INPUT);
  pinMode(ENC_R_A,INPUT_PULLUP);
  pinMode(ENC_R_B,INPUT_PULLUP);
  pinMode(ENC_L_A,INPUT_PULLUP);
  pinMode(ENC_L_B,INPUT_PULLUP);

  encLeft.init();
  encRight.init();

  attachInterrupt(digitalPinToInterrupt(encLeft.retEncInterruptChannel()), readLeftEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encRight.retEncInterruptChannel()), readRightEncoder, CHANGE);
  attachLMS();
}


#endif //ALL_MOTOR_H
