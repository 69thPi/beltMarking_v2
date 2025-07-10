#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

enum class error{
  NONE,
  STAMP_NOT_DOWN,
  STAMP_NOT_UP,
  STICK_NO_FRONT,
  STICK_NO_BACK,
  HINDERANCE_UP,
  HINDERANCE_FRONT,
  NO_AIR,
  SLIP
};

error flag=error:: NONE;


void display_errors(){
  switch(flag){

    case error::STAMP_NOT_DOWN :Serial.println("Timeout>> Stamp not down");
                                flag=error::NONE;
                                break;
    case error::STAMP_NOT_UP :Serial.println("Timeout>> Stamp not up");
                                flag=error::NONE;
                                break;
    case error::STICK_NO_FRONT :Serial.println("Tiemout>> Tray not front");
                                flag=error::NONE;
                                break;
    case error::STICK_NO_BACK :Serial.println("Timeout>> Tray not back");
                                flag=error::NONE;
                                break;
    case error::HINDERANCE_UP :Serial.println("Hinderance>> LM-SW Up-Down");
                                flag=error::NONE;
                                break;
    case error::HINDERANCE_FRONT :Serial.println("Hinderance>> LM-SW Front-Back");
                                flag=error::NONE;
                                break;
    case error::SLIP:Serial.println("TILT>> Platform not level");
                      flag=error::NONE;
                      break;
    case error::NO_AIR:Serial.println("AIR>> Low Pressure");
                      flag=error::NONE;
                      break;
    case error::NONE:break;
  }
}

#endif //ERRORHANDLING_H
