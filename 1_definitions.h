
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//RELAYS
#define R_POWER 18
#define powerON digitalWrite(R_POWER,LOW)
#define powerOFF digitalWrite(R_POWER,HIGH)

#define R_STICK 19
#define stickyON digitalWrite(R_STICK,LOW)
#define stickyOFF digitalWrite(R_STICK,HIGH)

//RED
#define R_RED_SLIDE 23
#define front digitalWrite(R_RED_SLIDE,LOW)
#define back digitalWrite(R_RED_SLIDE,HIGH)

//GREEN
#define R_GREEN_PICK 22
#define pickDown digitalWrite(R_GREEN_PICK,LOW)
#define pickUp digitalWrite(R_GREEN_PICK,HIGH)

//BLUE
#define R_BLUE_STAMP 21
#define stampDown digitalWrite(R_BLUE_STAMP,LOW)
#define stampUp digitalWrite(R_BLUE_STAMP,HIGH)

//SENSORS
//LDR
#define LEVEL_LDR 33
bool readLDR(int thresh=1500){
  //Serial.print("LDR >> ");
//  Serial.println(analogRead(LEVEL_LDR));
  return analogRead(LEVEL_LDR)>thresh?true:false;}

//PRESSURE SENSOR
#define PSI_TX 32
bool pressureCheck(int thresh=400)
{return analogRead(PSI_TX)>thresh?true:false;}

//LIMIT SWITCH
#define LMT_UP 35
bool readLMT_UP(){return digitalRead(LMT_UP);} //up is true

//LIMIT SWITCH
#define LMT_BCK 34
bool readLMT_BCK(){return digitalRead(LMT_BCK);} //back is true

//LIMIT SWITCH
#define STAMP_TGL 4
bool togglePressed_mode(){return !digitalRead(STAMP_TGL);}

//BUTTON
#define PAUSE_PLAY 16
bool buttonPressed_play(){return !digitalRead(PAUSE_PLAY);}

//BUTTON
#define P_DWN 17
bool buttonPressed_down(){return !digitalRead(P_DWN);}

//BUTTON
#define P_UP 5
bool buttonPressed_up(){return !digitalRead(P_UP);}

///////////////////////////////////////////////////////////
#endif //DEFINITIONS_H
