#include <LiquidCrystal.h>

#include <string.h>
#include <EEPROM.h>

#define MAX_NAME_LEN 10


// LCD setup
const int pinRS = 12;
const int pinE = 11;
const int pinD4 = 5;
const int pinD5 = 4;
const int pinD6 = 3;
const int pinD7 = 2;
LiquidCrystal lcd(pinRS, pinE, pinD4, pinD5, pinD6, pinD7);

// Joystick setup and variables
const int pinX = A0;
const int pinY = A1;
const int buttonPin = 0;
int buttonValue = 1;
int lastButtonValue = 1;
// INPUT_PULLUP settings
int buttonPressedValue = 0;
int buttonStandardValue = 1;

int switchValue = LOW;
int xValue = 0;
int yValue = 0;

bool xJoyMoved = false; 
bool yJoyMoved = false;

int minThreshold= 400;
int maxThreshold= 600;


//Game variables
bool gameJustStarted = true;
int currentLives = 3;
unsigned int startTime = 0;
unsigned int incrementTime = 0;
int startingLevelValue = 0;
int gameHighScore = 0;

char bestPlayer[MAX_NAME_LEN + 1] = "Unknown";
char currentPlayer[MAX_NAME_LEN + 1] = "Player";

int levelValue = 0;
int currentScore = 0;


// Menu variables
bool menuClicked = false;
int menuSelected = 1;

int settingSelected = 1;
bool settingIndexBlocked = false;

unsigned long int startingTime = 0;
unsigned long int lastDisplayMillis = 0;
int displayDelay = 200;


void setup() {
  lcd.begin(16, 2);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  //clearDisk();
  loadHighscore();

}
//EEPROM Functions

//Save the highscore in EEPROM
void saveHighscore() {
  int v[MAX_NAME_LEN + 1];
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    v[i] = (int)bestPlayer[i];
  }
  EEPROM[0] = gameHighScore;
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    EEPROM[i + 1] = v[i];
  }
}

//Load the highscore from the EEPROM
void loadHighscore() {
  if (EEPROM[0] == 0) {
    return;
  }
  int v[MAX_NAME_LEN + 1];
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    v[i] = EEPROM[i + 1];
  }
  Serial.print("\n");
  gameHighScore = EEPROM[0];
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    bestPlayer[i] = (char)v[i];
    Serial.print(v[i]);
    Serial.print(" ");
    Serial.println(bestPlayer[i]);
  }
}

//Clear the EEPROM
void clearDisk() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

//function that manage the button from the joystick
bool buttonPressed() {
  buttonValue = digitalRead(buttonPin);
  if (buttonValue == buttonPressedValue && lastButtonValue == buttonStandardValue) {
    lastButtonValue = buttonValue;
    return true;
  }
  if (buttonValue == buttonStandardValue && lastButtonValue == buttonPressedValue) {
    lastButtonValue = buttonValue;
  }
  return false;
}

//Menu selextions
void menuSelect() {
  int xValue = analogRead(pinX);
  if (xValue < minThreshold && (!xJoyMoved)) {
    xJoyMoved = true;
    menuSelected--;
    if (menuSelected < 1) {
      menuSelected = 3;
    }
  }
  else if (xValue > maxThreshold && (!xJoyMoved)) {
    xJoyMoved = true;    
    menuSelected++;
    if (menuSelected > 3) {
      menuSelected = 1;
    }
  }
  if (xValue >= minThreshold && xValue <= maxThreshold){
    xJoyMoved = false;
  }
  displayMenu();
}

// function that show the menu
void displayMenu() {
  if(millis() - lastDisplayMillis > displayDelay) {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (menuSelected == 1) {
      lcd.print(">");
    }
    else {
      lcd.print(" ");
    }
    lcd.print("Start ");
    if (menuSelected == 2) {
      lcd.print(">");
    }
    else {
      lcd.print(" ");
    }
    lcd.print("Setup");
    
    lcd.setCursor(0, 1);
    if (menuSelected == 3) {
      lcd.print(">");
    }
    else {
      lcd.print(" ");
    }
    lcd.print("HighScore ");
    
    lastDisplayMillis = millis();
  }
}

//StartGame function
void start() {
  if (gameJustStarted) {
    currentLives = 3;
    startTime = millis();
    incrementTime= millis();
    levelValue = startingLevelValue;
    gameJustStarted = false;
  }
  if (millis() - incrementTime > 5000) {
    levelValue++;
    incrementTime = millis();
  }
  currentScore = levelValue * 3;
  if (currentScore > gameHighScore) {
    gameHighScore = currentScore;
    strncpy(bestPlayer, currentPlayer, MAX_NAME_LEN + 1);
    saveHighscore();
  }
  displayStatus(currentLives, levelValue, currentScore);
  if (endGame()) {
    lcd.clear();
    lcd.print("Congratulations!");
    lcd.setCursor(0, 1);
    lcd.print("Press the button");
    while (1) {
      if (buttonPressed()) {
        menuClicked = false;
        gameJustStarted = true;
        break;
      }
    }
  }
}

//function that show the status of lives, level and score
void displayStatus (int lives, int level, int score) {
    if (millis() - lastDisplayMillis > displayDelay) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lives:");
    lcd.print(lives);
    lcd.print(" Level:");
    lcd.print(level);
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    lastDisplayMillis = millis();
  }
}

//function that calculate if the time is ended
int endGame() {
  if (millis() - startTime > 9900) {
    return true;
  }
  return false;
}

//function that show the settings
void showSettings() {
    lcd.clear();   
    lcd.print("Level: ");
    lcd.print(startingLevelValue);
    lcd.print("  ");  
    lcd.setCursor(0, 1);         
    lcd.print(currentPlayer);
    lastDisplayMillis = millis();
}

//Setting function
void settings() {
  if (millis() - lastDisplayMillis > displayDelay) {
    showSettings();
  }
  xValue = analogRead(pinX);
  if (xValue < minThreshold && (!xJoyMoved)) {
    startingLevelValue--;
    if (startingLevelValue < 0) {
      startingLevelValue = 9;
    }
    xJoyMoved = true;
  }
  else if (xValue > maxThreshold && (!xJoyMoved)) {
    startingLevelValue++;
    if (startingLevelValue > 9) {
      startingLevelValue = 0;
    }
    xJoyMoved = true;
  }
  if (xValue >= minThreshold && xValue <= maxThreshold){
    xJoyMoved = false;
  }
  if (buttonPressed()) {
    menuClicked = false;
  }      
}

//highscore function
void highscore() {
  lcd.clear();
  lcd.print(bestPlayer);
  lcd.print("  ");
  lcd.print(gameHighScore);
  while (1) {
    if (buttonPressed()) {
      menuClicked = false;
      break;
    }
  }
}
void loop() {
  if (!menuClicked) {
    if (buttonPressed()){
      menuClicked = true;
    }
    else {
      menuSelect();
    }     
  }
  else {
    if (menuSelected == 1) {
      start();    
    }
    else if (menuSelected == 2) {
      settings();
    }
    else if (menuSelected == 3) {
      highscore();
    }
  }

}
