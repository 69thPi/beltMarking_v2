/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                       PIN DEFINITIONS                                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    NAME      |    PIN   |    MODE   |         RELATED FUNCTIONS            |             COMMENTS
//-------------------------------------------------------------------------------------------------------------------------------
// ## SENSORS
// LEVEL LDR    |    33    |   INPUT   | bool readLDR(int thresh=1000);       | ANALOG IN_PULLED LOW 1K RESISTOR
// Press. Tx    |    32    |   INPUT   | bool pressureCheck(int thresh=1000); | ANALOG IN_3 WIRE. NO NEED TO PULL LOW/HIGH
//
// ## LIMIT SWITCHES
// PT_LMT_L     |    39    |   INPUT   | bool readLMTL();                     | PULLED_HIGH
// PT_LMT_R     |    25    |   INPUT   | bool readLMTR();                     | PULLED_HIGH
// UP_DWN_LMT   |    35    |   INPUT   | bool readLMT_UP();                   | PULLED_HIGH
// F_BCK_LMT    |    34    |   INPUT   | bool readLMT_F_BCK();                | PULLED_HIGH
//
// ## BUTTONS
// STAMP_TGL    |     4    |   INPUT   | bool togglePressed_mode();           | PULLED_HIGH
// PAUSE_PLAY   |    16    |   INPUT   | bool buttonPressed_play();           | PULLED_HIGH
// P_DWN        |    17    |   INPUT   | bool buttonPressed_down();           | PULLED_HIGH
// P_UP         |     5    |   INPUT   | bool buttonPressed_up();             | PULLED_HIGH
//
// ## ENCODERS
// ENC_L_1      |    26    |   INPUT   |
// ENC_L_2      |    27    |   INPUT   |
// ENC_R_1      |    14    |   INPUT   |
// ENC_R_2      |    12    |   INPUT   |
//
// ## STEPPERS
// STP_1_STEP   |    15    |  OUTPUT   | void stepper_move(int val=0,int unit=50,int mult=1);
// STP_2_STEP   |     2    |  OUTPUT   |
// STP_DIR      |    13    |  OUTPUT   |
//
// ## RELAYS
// R_RED        |    23    |  OUTPUT   |  void front();                       | R_SLIDE
//              |          |           |   void back();                       |
// R_GREEN      |    22    |  OUTPUT   |  void pickDown();                    | R_PICK
//              |          |           |   void pickUP();                     |
// R_BLUE       |    21    |  OUTPUT   |  void stampDWN();                    | R_STAMP
//              |          |           |   void stampUP();                    |
// R_STICKY     |    19    |  OUTPUT   |  void stickyON();                    | NO SUPPLY. ONLY SHORTING
//              |          |           |  void stickyOFF();                   |
// R_POWER      |    18    |  OUTPUT   | void powerON();                      |
//              |          |           | void powerOFF();                     |
//
////////////////////////////////////////////////////////// END //////////////////////////////////////////////////////////////////
*/
