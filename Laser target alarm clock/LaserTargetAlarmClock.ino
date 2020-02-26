#include <NewTone.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>


#define STANDARD_HOUR 1
#define STANDARD_MINUTE 1

#define CONTRAST 60

#define MAX_YEAR 2030
#define LOWER_YEAR 1970

#define MAX_MONTH 12
#define LOWER_MONTH 1

#define LOWER_DAY 1

#define MAX_HOUR 24
#define MAX_MINUTE 59

#define LOWER_HOUR 1
#define LOWER_MINUTE 1
int maxDay;

int buzzerTone = 1000;
const int pinBuzzer = 13;


const int pinSDA = A4;
const int pinSCL = A5;
const int receiverPin = 10;
const int pinPlus = 6;
const int pinMin = 7;
const int pinSet = 8;
const int pinAlar = 9;
const int pinRS = 12;
const int pinE = 11;
const int pinD4 = 5;
const int pinD5 = 4;
const int pinD6 = 3;
const int pinD7 = 2;
const int PinCntrst = A3;
LiquidCrystal lcd(pinRS, pinE, pinD4, pinD5, pinD6, pinD7);

IRrecv irrecv(receiverPin);
decode_results results;

RTC_DS1307 rtc;


//Alarm
bool alarmSet = false;
unsigned long alarmDelay = 5000;

//Buttons
int plusButton = 1;
int lastPlusButton = 1;

int minusButton = 1;
int lastMinusButton = 1;

int setButton = 1;
int lastSetButton = 1;

int alarmButton = 1;
int lastAlarmButton = 1;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long alarmClickedTime;


int loadHourAlarmEEPROM() {
  if (EEPROM[0] == 0) { //No Hour saved
    return STANDARD_HOUR;
  } 
  else {
    return EEPROM[0];
  }
}

int loadMinuteAlarmEEPROM() {
  if (EEPROM[1] == 0) { //No minute saved
    return STANDARD_MINUTE;
  } 
  else {
    return EEPROM[1];
  }  
}

void saveHourAlarmEEPROM(int h) {
  EEPROM[0] = h;
}
void saveMinuteAlarmEEPROM(int m) {
  EEPROM[1] = m;
}

//print the time on lcd screen
void printTime() {
  //Contrast is set manually
  analogWrite(PinCntrst, CONTRAST);
  lcd.setCursor(0, 0);
 
  DateTime now = rtc.now();

  //add a 0 to numbers with one digit
  if (now.day() < 10) {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  lcd.print('/');
  if (now.month() < 10) {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);

  lcd.setCursor(0, 1);
  if (now.hour() < 10) {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  if (now.minute() < 10) {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  //lcd.print(':');
  //lcd.print(now.second(), DEC);
 
}

//Return true if the button is clicked
bool checkSetButtonClicked() {
  int reading = digitalRead(pinSet);
  if (reading != lastSetButton) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != setButton) {
      setButton = reading;
      if (setButton == HIGH) {
        return true;
      }
    }
  }
  lastSetButton = reading;
  return false;
}

//Return true if the button is clicked
bool checkAlarmButtonClicked() {
  int reading = digitalRead(pinAlar);
  if (reading != lastAlarmButton) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != alarmButton) {
      alarmButton = reading;
      if (alarmButton == HIGH) {
        return true;
      }
    }
  }
  lastAlarmButton = reading;
  return false;
}

//Return true if the button is clicked
bool checkPlusButtonClicked() {
  int reading = digitalRead(pinPlus);
  if (reading != lastPlusButton) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != plusButton) {
      plusButton = reading;
      if (plusButton == HIGH) {
        return true;
      }
    }
  }
  lastPlusButton = reading;
  return false;
}

//Return true if the button is clicked
bool checkMinusButtonClicked() {
  int reading = digitalRead(pinMin);
  if (reading != lastMinusButton) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != minusButton) {
      minusButton = reading;
      if (minusButton == HIGH) {
        return true;
      }
    }
  }
  lastMinusButton = reading;
  return false;
}

int setYear() {
  int y = 2020;
  while (!checkSetButtonClicked()) {
    lcd.print("    Settings");
    lcd.setCursor(0, 1);
    lcd.print("Change year:");
    lcd.print(y, DEC);
    if (checkPlusButtonClicked()) {
      y = y + 1;
      if (y > MAX_YEAR) {
        y = LOWER_YEAR;
      }
    }
    if (checkMinusButtonClicked()) {
      y = y - 1;
      if (y < LOWER_YEAR) {
        y = MAX_YEAR;
      }
    }  
  }
  return y; 
}

//check if the year is leap or not
bool isLeapYear(int y) {
  if ((y%400==0) || (y%4==0 && !(y%100==0))) {
    return true;
  }
  else 
    return false;
}

int setMonth() {
  int m = 1;
  while (!checkSetButtonClicked()) {
    lcd.print("    Settings");
    lcd.setCursor(0, 1);
    lcd.print("Change month:");
    lcd.print(m, DEC);
    if (checkPlusButtonClicked()) {
      m = m + 1;
      if (m > MAX_MONTH) {
        m = LOWER_MONTH;
      }
    }
    if (checkMinusButtonClicked()) {
      m = m - 1;
      if (m < LOWER_MONTH) {
        m = MAX_MONTH;
      }
    }  
  }
  return m; 
}

int setDay(int m, int y) {
    int d = 1;
    if (m == 2) {
      if (isLeapYear(y)) {
        maxDay = 29;
      }
      else {
        maxDay = 28;
      }
    }
    else {
      if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) {
        maxDay = 31;
      }
      else {
        maxDay = 30;
      }
    }
  while (!checkSetButtonClicked()) {
    lcd.print("    Settings");
    lcd.setCursor(0, 1);
    lcd.print("Change day:");
    lcd.print(d, DEC);
    if (checkPlusButtonClicked()) {
      d = d + 1;
      if (d > maxDay) {
        d = LOWER_DAY;
      }
    }
    if (checkMinusButtonClicked()) {
      d = d - 1;
      if (d < LOWER_MONTH) {
        d = maxDay;
      }
    }  
  }
  return d; 
}


int setHour () {
  int h = 1;
  while (!checkSetButtonClicked()) {
    lcd.print("    Settings");
    lcd.setCursor(0, 1);
    lcd.print("Change hour:");
    lcd.print(h, DEC);
    if (checkPlusButtonClicked()) {
      h = h + 1;
      if (h > MAX_HOUR) {
        h = LOWER_HOUR;
      }
    }
    if (checkMinusButtonClicked()) {
      h = h - 1;
      if (h < LOWER_HOUR) {
        h = MAX_HOUR;
      }
    }  
  }
  lcd.clear();
  return h; 
}

int setMinute () {
  int m = 1;
  while (!checkSetButtonClicked()) {
    lcd.print("    Settings");
    lcd.setCursor(0, 1);
    lcd.print("Change min:");
    lcd.print(m, DEC);
    if (checkPlusButtonClicked()) {
      m = m + 1;
      if (m > MAX_MINUTE) {
        m = LOWER_MINUTE;
      }
    }
    if (checkMinusButtonClicked()) {
      m = m - 1;
      if (m < LOWER_MINUTE) {
        m = MAX_MINUTE;
      }
    }  
  }
  lcd.clear();
  return m; 
}


void settingTime() {  
  int newYear = setYear();
  int newMonth = setMonth();
  int newDay = setDay(newMonth, newYear);
  int newHour = setHour();
  int newMinute = setMinute();
  rtc.adjust(DateTime(newYear, newMonth, newDay, newHour, newMinute, 0));   // <----------------------SET TIME AND DATE: YYYY,MM,DD,HH,MM,SS
  lcd.clear();
  
}


//Display alarm screen
void displayAlarm() {
  int hourAlarm = loadHourAlarmEEPROM();
  int minuteAlarm = loadMinuteAlarmEEPROM();
  //Contrast is set manually  
  analogWrite(PinCntrst, CONTRAST);
  lcd.setCursor(0, 0);
 
  lcd.print("Alarm ");
  //print the alarm status
  if(alarmSet) {
    lcd.print("        ON");
  }
  else {
    lcd.print("       OFF");
  }
  lcd.setCursor(0, 1);
  if (hourAlarm < 10) {
    lcd.print("0");
  }
  lcd.print(hourAlarm, DEC);
  lcd.print(':');
  if (minuteAlarm < 10) {
    lcd.print("0");
  }
  lcd.print(minuteAlarm, DEC);
   
}

void setAlarm() {
  alarmClickedTime = millis();
  while (!checkAlarmButtonClicked() && (millis() - alarmClickedTime < alarmDelay)) {
    //if the + or - button is clicked the alarm status change
    if(checkPlusButtonClicked() || checkMinusButtonClicked()) {
      alarmSet = !alarmSet; 
    }
    //save the new alarm in the EEPROM
    if (checkSetButtonClicked()) {
        saveHourAlarmEEPROM(setHour());
        saveMinuteAlarmEEPROM(setMinute());
    }   
    displayAlarm();  
  }
}

void alarmSound() {
    NewTone(pinBuzzer, 200);
    alarmSet = true;
    //if the IR receiver, receive a signal the alarmSet variable is set to false and the buzzer stop to work
    if(irrecv.decode(&results)) {
      noNewTone(); 
      irrecv.resume(); 
      alarmSet = false;
  }
}

void setup() {
  Serial.begin(9600);
  //clock module
    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2016, 11, 19, 19, 45, 0));   // <----------------------SET TIME AND DATE: YYYY,MM,DD,HH,MM,SS
  }

  pinMode(pinBuzzer, OUTPUT);
  //Buttons
  pinMode(pinPlus, INPUT);
  pinMode(pinMin, INPUT);
  pinMode(pinSet, INPUT);
  pinMode(pinAlar, INPUT);
  

  //Remote Controll
  irrecv.enableIRIn();
  irrecv.blink13(true);

  pinMode(PinCntrst, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  
}

void loop() {
  DateTime now = rtc.now();
  int alarmHour = loadHourAlarmEEPROM();
  int alarmMinute = loadMinuteAlarmEEPROM();

  printTime();
  if (checkSetButtonClicked()) {
    lcd.clear();
    settingTime();
  }
  if (checkAlarmButtonClicked()) {
    lcd.clear();
    setAlarm();
  }
  if (now.hour() == alarmHour && now.minute() == alarmMinute && alarmSet) {
    alarmSound();
  }
  if(irrecv.decode(&results)) {
      noNewTone(); 
      irrecv.resume(); 
      alarmSet = false;
  }
}
