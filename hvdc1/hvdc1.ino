#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define SCL 22
#define SDA 21
#define minus 26
#define plus 27
#define pwm 4     //D4 Flyback Driver
#define ledChannel 0
#define resolution 8

int freq=15000;
int fS=170;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void disp(int val){
  int real=255-val;
  lcd.setCursor(0,0);
  lcd.print("Power : ");
  lcd.setCursor(11,0);
  lcd.print("  ");
  lcd.setCursor(11,0);
  lcd.print(String(real));
  delay(100);
  }

void genPWM(int dutyCyc=255){
  if(dutyCyc>170){
      ledcSetup(ledChannel, freq, resolution);
      ledcAttachPin(pwm, ledChannel);
      ledcWrite(ledChannel, dutyCyc);}
}

void action_change(int val) {

    if(val>0){fS++;}
    if(val<0){fS--;}
    if(fS>255){fS=255;}
    if(fS<170){fS=170;}
    genPWM(fS);
    disp(fS);
    EEPROM.write(0,fS);
    EEPROM.commit();
    Serial.println(String(fS));
  }




void setup(){
  Serial.begin(115200);
  EEPROM.begin(10);
  pinMode(plus,INPUT);
  pinMode(minus,INPUT);
  fS=255;
  int ig=EEPROM.read(0);
  if(ig>170 && ig <256){fS=ig;}
  Serial.println("EEPROM value:"+ String(ig));
  lcd.init();                 // initialize the lcd
  lcd.backlight();
  genPWM();
  genPWM(fS);
  disp(fS);
}

void loop(){

  if(digitalRead(plus)){
    Serial.println("plus");
    while(digitalRead(plus)){delay(1);}
    delay(100);
    action_change(-1);}

  if(digitalRead(minus)){
    Serial.println("minus");
    while(digitalRead(minus)){delay(1);}
    delay(100);
    action_change(1);}

}
