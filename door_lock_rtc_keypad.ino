/*
  PIN configuration

  Arduino ---  LCD
    RS    ---  12
    RW    ---  GND
    EN    ---  13
    D4    ---  11
    D5    ---  16
    D6    ---  15
    D7    ---  14

    
  Arduino ---  RTC
    A5    ---  SCL
    A4    ---  SDA

  PIN 13 ===> STATUS LED
  ON ==> UNLOCKED
  OFF ==> LOCKED
*/
#include <Wire.h>
#include <DS1307.h>
#include <Keypad.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(12,11,10,16,15,14);
DS1307 rtc;

#define lock 13
String pwd="0000", new_pwd="0000";
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {2,3,4,5}; 
byte colPins[COLS] = {6,7,8,9};
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
bool locked=true;
short Lmin=0, Lhour=12;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup()
{
  Serial.begin(9600);
  while(!Serial);

  pinMode(lock, OUTPUT);
  digitalWrite(lock, locked);
  
  Serial.println("Init RTC...");
  Serial.println("Custom keypad");
  lcd.begin(16,2);
  lcd.print("RTC lock system");
  //only set the date+time one time
  //rtc.set(0, 8, 12, 31, 1, 2020); //08:00:00 24.12.2014 //sec, min, hour, day, month, year

  //stop/pause RTC
  // rtc.stop();

  //start RTC
  rtc.start();
}


void loop()
{
  char key = customKeypad.getKey();
  if(key){
    Serial.println(key);
      switch(key){
          case '*':{
            if(!locked){
                locked=true;
              }else{               
                lcd.clear();
                lcd.print(F("Enter Password:"));
                lcd.setCursor(0,1);
                get_pwd();
                
                if(pwd == new_pwd){
                  locked = false;
                }else{                  
                  lcd.clear();
                  lcd.print(F("Wrong Password"));
                  delay(500);
                }
              }
              break;
            }
          case '#':{          
                lcd.clear();
                lcd.print(F("Enter Password:"));
                lcd.setCursor(0,1);
                get_pwd();
              if(pwd == new_pwd){          
                  lcd.clear();
                  lcd.print(F("New Password:"));
                  lcd.setCursor(0,1);
                  get_pwd();
                  pwd = new_pwd;
                  Serial.println(pwd);
                }
              break;
            }
          case 'C':{
              break;
            }
          case 'D':{
              get_lock_time();
              break;
            }
          case 'A':{
              lcd.clear();
              lcd.print("Lock at:");
              lcd.setCursor(0,1);
              char buff[6];
              sprintf(buff, "%02d:%02d", Lhour,Lmin);
              lcd.print(buff);
              delay(2000);
              break;
            }
        }
    }
    delay(100);
    display_stat();
}
void get_pwd(){
  short pos=0;
  while(1){
      char key = customKeypad.getKey();
      if(pos==4)
        return;
      else if(key){
        lcd.print(key);
        new_pwd[pos] = key;
        pos++;
        }
    }  
  }

void display_stat(){
  lcd.clear();
  uint8_t sec, min, hour, day, month;
  uint16_t year;
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  char buff[16];
  sprintf(buff, "%d:%d:%d", hour,min,sec);
  lcd.print(buff);
  lcd.setCursor(0,1);
  if(Lhour == hour && Lmin == min)
    locked = true;
  digitalWrite(lock, locked);
  if(locked){
      lcd.print("Status: Locked");
    }else{
      lcd.print("Status: Unlocked");
    }
  }

void get_lock_time(){
  Lhour=0;
  Lmin=0;
  short pos=0;
  lcd.clear();
  lcd.print(F("Enter hours:"));
  lcd.setCursor(0,1);
  while(1){
      char key = customKeypad.getKey();
      if(pos==2)
        break;
      else if(key){
        lcd.print(key);
        Lhour *=10;
        Lhour += (int)key -48;
        pos++;
        }
    }  
  lcd.clear();
  lcd.print(F("Enter mins:"));
  lcd.setCursor(0,1);
  pos=0;
  while(1){
      char key = customKeypad.getKey();
      if(pos==2)
        break;
      else if(key){
        lcd.print(key);
        Lmin *= 10;
        Lmin += (int) key-48;
        pos++;
        }
    }
  }
