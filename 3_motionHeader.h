#ifndef MOTION_HEADER_H
#define MOTION_HEADER_H

#define MOT_L_STEP  2
#define MOT_R_STEP  15
#define MOT_DIR     13

#define stepClock digitalWrite(MOT_DIR,HIGH)
#define stepAntiClock digitalWrite(MOT_DIR,LOW)

#define LMT_PT_L 25
#define LMT_PT_R 39

#define ENC_R_A 14
#define ENC_R_B 12

#define ENC_L_A 26
#define ENC_L_B 27

#define LEDC_TIMER        0 // LEDC timer to use (0 or 1)
#define LEDC_MODE         LEDC_HIGH_SPEED_MODE // Use high-speed mode for better performance


enum class motionClass{
  _DEFAULT,
  IDLE,
  MOVING_UP,
  MOVING_DOWN,
  LIMIT_LEFT,
  LIMIT_RIGHT,
  LIMIT_HOME,
  LDR_STOP,
  LDR_ZONE
};

motionClass motionState=motionClass::IDLE;

// void printMotion(){
//     Serial.print("motionClass ::");
//   switch(motionState){
//     case motionClass::IDLE:Serial.println("IDLE");break;
//     case motionClass::MOVING_UP:Serial.println("MOVING_UP");break;
//     case motionClass::MOVING_DOWN:Serial.println("MOVING_DOWN");break;
//     case motionClass::LIMIT_LEFT:Serial.println("LIMIT_LEFT");break;
//     case motionClass::LIMIT_RIGHT:Serial.println("LIMIT_RIGHT");break;
//     case motionClass::LIMIT_HOME:Serial.println("LIMIT_HOME");break;
//     case motionClass::LDR_STOP:Serial.println("LDR_STOP");break;
//     case motionClass::LDR_ZONE:Serial.println("LDR_ZONE");break;
//   }
// }

enum class correctionClass{
  _DEFAULT,
  NONE,
  LEFT,
  RIGHT
};

correctionClass correctionState=correctionClass::NONE;

// void printCorrection(){
//   Serial.print("correctionClass ::");
//   switch(correctionState){
//     case correctionClass::NONE:Serial.println("NONE");break;
//     case correctionClass::LEFT:Serial.println("LEFT");break;
//     case correctionClass::RIGHT:Serial.println("RIGHT");break;
//   }
// }

class motionEncoder{
  int STEP_PIN;
  int DIR_PIN;
  int dir;
  int bottomLMS;
  String label;

  int LEDC_TIMER_BIT;
  int PULSE_FREQ;
  int LEDC_CHANNEL;
  int dutyCycle;

  bool inMotion;
  bool targetReached;

  int journeyZero;
  int targetDisp;


  // ENCODER PART
  int ENC_CLK;
  int ENC_DT;

  volatile int encoderPos;
  volatile int targetPos;

  volatile int lastStateCLK;


  //ACTION FUNCTIONS- MOTOR
  void setDirection(int a){
   dir=a;
   dir<0?stepClock:stepAntiClock;
   delayMicroseconds(50);
  }


  void startMotor(int dir=1){
    setDirection(dir);
    ledcChangeFrequency(LEDC_CHANNEL, PULSE_FREQ , LEDC_TIMER_BIT);
    ledcWrite(LEDC_CHANNEL,dutyCycle);}

  void axlControl(){
    // printMotion();
    // printCorrection();
      if(correctionState==correctionClass::NONE){
          int u1=80;
          int u2=300;
          int rampPerc=20;
          int targetFreq = 80;
          int journeyPerc = int(( float(abs(encoderPos - journeyZero)) / targetDisp) * 1000.f);
          if(journeyPerc<0 || journeyPerc>1000){journeyPerc=0;}

          if (journeyPerc > rampPerc && journeyPerc < (1000 - rampPerc)) {targetFreq = u2;}
          else {
            if(journeyPerc >= (1000 - rampPerc)){journeyPerc= 1000 - journeyPerc;}
            targetFreq = u1 +  (int)(((float)(u2 - u1) / rampPerc) * journeyPerc);}

          if (PULSE_FREQ != targetFreq) {

              PULSE_FREQ = targetFreq;
              Serial.print(">> ");
              Serial.print(journeyPerc);
              Serial.print(" % :: ");
              Serial.println(PULSE_FREQ);
              ledcChangeFrequency(LEDC_CHANNEL, PULSE_FREQ, LEDC_TIMER_BIT);
              ledcWrite(LEDC_CHANNEL, dutyCycle); }
          }
  }

public:
  motionEncoder(String lbl, int ENC1, int ENC2, int pin_step, int pin_dir, int btm_LMS, int chnl=0){

    label=lbl;
    STEP_PIN=pin_step;
    DIR_PIN=pin_dir;
    dir=1;

    bottomLMS=btm_LMS;

    LEDC_CHANNEL=chnl;
    LEDC_TIMER_BIT=10;
    dutyCycle = (1 << (LEDC_TIMER_BIT - 1));
    PULSE_FREQ=200; //Hertz // 200 pulses per second- 1 rotation
    ledcChangeFrequency(LEDC_CHANNEL, PULSE_FREQ , LEDC_TIMER_BIT);

    inMotion=false;
    targetReached=false;
    encoderPos=0;
    targetPos=0;

    //ENCODER PART
    ENC_CLK = ENC1;
    ENC_DT = ENC2;
    lastStateCLK=HIGH;
  }

  void init(){
    lastStateCLK=digitalRead(ENC_CLK);
    ledcSetup(LEDC_CHANNEL, 1000, LEDC_TIMER_BIT);
    ledcAttachPin(STEP_PIN, LEDC_CHANNEL);
    ledcWrite(LEDC_CHANNEL, 0);
    setSpeed(300);
  }

  // motor functions

  void stopMotor(){
    inMotion=false;
    ledcWrite(LEDC_CHANNEL,0);
    delay(2);
    }

//AUXILARY FUNCTIONS
  void resetPos(){encoderPos=0;targetPos=0;lastStateCLK=digitalRead(ENC_CLK);}

  bool pressedLMS(){
    bool ign_status=!digitalRead(bottomLMS);
    if(ign_status && inMotion){
        if(dir<0){stopMotor();}
        resetPos();
    }
    return ign_status;
  }

  void countTicks(){
    int currentStateCLK=digitalRead(ENC_CLK);
    int currentStateDT=digitalRead(ENC_DT);
    if(currentStateCLK!= lastStateCLK){
      if(currentStateDT !=currentStateCLK){encoderPos++;}
      else{encoderPos--;}
    lastStateCLK=currentStateCLK;}
  }

  //REACTION FUCTIONS FOR ENCODER
  void updateTarget(int addOn){ // in ticks
    if(inMotion || addOn==0){return;} //do nothing if moving

    targetPos+=addOn;
    inMotion=true;
    targetDisp = abs(addOn);
    journeyZero=encoderPos;
    startMotor(addOn<0?-1:1);
  }

  void evaluatePosition(){

    if(!inMotion){return;} //check only if motors are moving.

    axlControl();////////////////////////
    bool targetReached=false;

///////////////////////////////////
    if(dir > 0 && encoderPos >= targetPos){targetReached=true;}
    if(dir < 0 && encoderPos <= targetPos){targetReached=true;}

    if(targetReached){

      stopMotor();

      Serial.print(label);
      Serial.print(" == ");
      Serial.print(targetPos);
      Serial.print(" == ");
      Serial.println(encoderPos);
    }

  }

  int retEncInterruptChannel(){return ENC_CLK;}

  int retPinLMS(){return bottomLMS;}

  bool isBusy(){return inMotion;}

  int getPos(){return encoderPos;}

  void setPos(int a){encoderPos =a;}

  void setTarget(int a){targetPos=a;}

  void setSpeed(int ign_freq=200){PULSE_FREQ=ign_freq;}

};

motionEncoder encLeft("Left", ENC_L_A,ENC_L_B,MOT_L_STEP,MOT_DIR,LMT_PT_L,0);
motionEncoder encRight("right", ENC_R_B,ENC_R_A,MOT_R_STEP,MOT_DIR,LMT_PT_R,1);

#endif //MOTION_HEADER_H
