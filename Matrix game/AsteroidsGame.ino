#include <LiquidCrystal.h>
#include "LedControl.h"
#include <string.h>
#include <EEPROM.h>

#define MAX_NAME_LEN 10 //Max characters name player
#define NUM_NAME_SPACESHIP 10 //Max characters name spaceship

const int buzzerPin = 6;

const int pinDIN = 12;
const int pinCLK = 11;
const int pinLOAD = 10;
const int pinNoDriver = 1; // 1 as we are only using 1 MAX7219
LedControl lc = LedControl(pinDIN, pinCLK, pinLOAD, pinNoDriver);

//matrixVariables
int xAx = 4;
int yAx = 0;

// LCD setup
const int pinRS = 9;
const int pinE = 8;
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
unsigned long startTime = 0;
unsigned long incrementTime = 0;
int startingLevelValue = 0;
int gameHighScore = 0;

char bestPlayer[MAX_NAME_LEN + 1] = "Unknown";
char currentPlayer[MAX_NAME_LEN + 1] = "Player";

int levelValue = 0;
int currentScore = 0;
bool newLevelPassed = false;
unsigned int lastNewLevelPassed = 0;

// Menu variables
bool menuClicked = false;
int menuSelected = 1;

int settingSelected = 1;
bool settingIndexBlocked = false;

unsigned long startingTime = 0;
unsigned long lastDisplayMillis = 0;
int displayDelay = 200;

int infoIndex = 0;

int spaceshipNameIndex = 0;


String spaceships[NUM_NAME_SPACESHIP] = {"Player   ", "Tardis   ", "Enterpris", "Columbia ", "Challenge", "Discovery", "Atlantis ", "Millenniu", "Serenity ", "E.T.     " };

int indexLink = 0;




//PlayGame Variables
unsigned long previewTimeSpaceship = 0;
unsigned long previewTimeAsteroid = 0;
bool canMove = true;
bool canMoveAsteroid = true;
unsigned long delaySpeed = 100;
unsigned long delaySpeedAsteroid = 400;
unsigned long lastDeath = 0;
unsigned long deathDelay = 800;
long xAxAsteroid;
long yAxAsteroid;
long newAsteroid;
const int easyTime = 350;
const int mediumTime = 250;
const int hardTime = 150;
int difficulty = 0;

//direction of each asteroid
enum directionAsteroid {
  left, 
  top, 
  right, 
  nothing
};
// asteroid Variables
directionAsteroid direct = nothing;
int choiceDirection = 0;

bool asteroidPresence = false;




//lcd custom characters
byte heart[8] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
}; 
byte emptyHeart[] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000
};
byte arrowUp[] = {
  B00100,
  B01110,
  B11111,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100
};
byte arrowDown[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B11111,
  B01110,
  B00100
};

byte arrowLeft[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B11111,
  B01000,
  B00100,
  B00000
};
byte arrowRight[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B11111,
  B00010,
  B00100,
  B00000
};
byte arrowUpDown[] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};


//Animation LCD 
//delay() used to create the animation
void introAnimation() {
      lcd.clear();
      delay(1000);
      lcd.print("           SPACE");
      lcd.setCursor(0, 1);
      delay(2000);
      lcd.print("            TRIP");
      for (int i = 0; i < 16; i++) {
        tone(buzzerPin, 1000 - (i*80), 100);
        delay (250); 
        lcd.scrollDisplayLeft();
      }
      delay(500);
}

//EEPROM Functions

//Save the highscore in EEPROM
void saveHighscore() {
  int v[MAX_NAME_LEN + 1];
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    v[i] = (int)bestPlayer[i];
  }
  EEPROM[0] = gameHighScore; //HighScore EEPROM[0]
  for (int i = 0; i <= MAX_NAME_LEN; i++) {
    EEPROM[i + 1] = v[i];
  }
}

//Load the highscore from the EEPROM
void loadHighscore() {
  if (EEPROM[0] == 0) { //No highscore saved
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

//Menu selections
void menuSelect() {
  int xValue = analogRead(pinX);
  if (xValue < minThreshold && (!xJoyMoved)) {
    xJoyMoved = true;
    menuSelected--;
    if (menuSelected < 1) {
      menuSelected = 4;
    }
  }
  else if (xValue > maxThreshold && (!xJoyMoved)) {
    xJoyMoved = true;    
    menuSelected++;
    if (menuSelected > 4) {
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
    lcd.print("   ");
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
    
    if (menuSelected == 4) {
      lcd.print(">");
    }
    else {
      lcd.print(" ");
    }
    lcd.print("Info ");
    
    lastDisplayMillis = millis();
  }
}

//function that move the spaceship through the screen
void moveSpaceship() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (canMove) {
    lc.setLed(0, xAx, yAx, true);
    canMove = false;
  }
  if ( millis() - previewTimeSpaceship > delaySpeed) {
    lc.setLed(0, xAx, yAx, false);
    previewTimeSpaceship = millis ();
    canMove = true;
    if (xValue < minThreshold) {
      xAx--;
      if (xAx < 0) {
        xAx = 0;
      }
    }

    if (xValue > maxThreshold) {
      xAx++;
      if (xAx > 7) {
        xAx = 7;
      }
    }

    if (yValue < minThreshold) {
      yAx++;
      if (yAx > 7) {
        yAx = 7;
      }
    }
    if (yValue > maxThreshold) {
      yAx--;
      if (yAx < 0) {
        yAx = 0;
      }
    }
  } 
}

//function that generate a given asteroid in the screen
void generateAsteroid(long *x, long *y) {
  if ( direct == nothing) { //if direct == nothing new asteroid
    choiceDirection = random(4);
    if (*x == 0) {
      direct = left; 
    }
    if (*y == 7) {
      if (choiceDirection == 0)
        direct = left; 
      if (choiceDirection == 1)
        direct = right;
      if (choiceDirection == 2 || choiceDirection == 3)
        direct = top; 
    }
    if (*x == 7 && direct == nothing) {
      direct = right;  
    }
  }
  if (canMoveAsteroid) {
    lc.setLed(0, *x, *y, true); 
    canMoveAsteroid = false;   
  }

  if ( millis() - previewTimeAsteroid > delaySpeedAsteroid) {
    lc.setLed(0, *x, *y, false);
    previewTimeAsteroid = millis();
    canMoveAsteroid = true;
    switch (direct) {
      case left:
        *x = *x + 1;
        *y = *y - 1;
        if (*x == 7) {
          direct = right;
        }
        break;
      case top:
        *y = *y - 1;
        break;
      case right:
        *x = *x - 1;
        *y = *y - 1;
        if (*x == 0) {
          direct = left;
        }
        break;
    }

    //check if the next position is outside the matrix
    if ((*x > 7) || (*y > 7) || (*x < 0) || (*y < 0) ) {
      asteroidPresence = false;
      direct = nothing;
      currentScore++;
    }
  }
}

//function that generate a random starting point asteroid
void createNewAsteroid() {
  newAsteroid = random(4);
  if (newAsteroid == 0) {
    xAxAsteroid = 0;
    yAxAsteroid = random(3, 8);
  }
  if (newAsteroid == 1) {
    xAxAsteroid = random(3, 8);
    yAxAsteroid = 7;        
  }
  if (newAsteroid == 2) {
    xAxAsteroid = 7;
    yAxAsteroid = random(3, 8);    
  }
  if (newAsteroid == 3) {
    xAxAsteroid = xAx;
    yAxAsteroid = 7;
  }
}
//asteroid game
void playGame() {
  moveSpaceship();
  if (!asteroidPresence) {
    createNewAsteroid();
    asteroidPresence = true;  
  }
  generateAsteroid(&xAxAsteroid, &yAxAsteroid);
  if ((xAx == xAxAsteroid && yAx == yAxAsteroid) && (millis() - lastDeath > deathDelay)) {
    currentLives--;
    lastDeath = millis();
    tone(buzzerPin, 200, 600);

  }
}

//Animation arrow
//delay() used for the animation
void startingAnimation() {
  lcd.clear();
  lcd.print(" ARE YOU READY? ");
  lcd.setCursor(0, 1);
  
  for (int i = 0; i < 12; i++) {
    lc.setLed(0, 0, 0 + i, true);
    lc.setLed(0, 1, 1 + i, true);
    lc.setLed(0, 2, 2 + i, true);
    lc.setLed(0, 3, 3 + i, true);
  
    lc.setLed(0, 7, 0 + i, true);
    lc.setLed(0, 6, 1 + i, true);
    lc.setLed(0, 5, 2 + i, true);
    lc.setLed(0, 4, 3 + i, true);

    lc.setLed(0, 2, 0 + i, true);
    lc.setLed(0, 2, -1 + i, true);
    lc.setLed(0, 2, -2 + i, true);
    lc.setLed(0, 2, -3 + i, true);
    lc.setLed(0, 5, 0 + i, true);
    lc.setLed(0, 5, -1 + i, true);
    lc.setLed(0, 5, -2 + i, true);
    lc.setLed(0, 5, -3 + i, true);

    lc.setLed(0, 1, 0 + i, true);
    lc.setLed(0, 6, 0 + i, true);

    tone(buzzerPin, 1000 - (i*100), 250);
    delay(250);  
    
    lc.setLed(0, 0, 0 + i, false);
    lc.setLed(0, 1, 1 + i, false);
    lc.setLed(0, 2, 2 + i, false);
    lc.setLed(0, 3, 3 + i, false);
  
    lc.setLed(0, 7, 0 + i, false);
    lc.setLed(0, 6, 1 + i, false);
    lc.setLed(0, 5, 2 + i, false);
    lc.setLed(0, 4, 3 + i, false);

    lc.setLed(0, 2, 0 + i, false);
    lc.setLed(0, 2, -1 + i, false);
    lc.setLed(0, 2, -2 + i, false);
    lc.setLed(0, 2, -3 + i, false);
    lc.setLed(0, 5, 0 + i, false);
    lc.setLed(0, 5, -1 + i, false);
    lc.setLed(0, 5, -2 + i, false);
    lc.setLed(0, 5, -3 + i, false);


    lc.setLed(0, 1, 0 + i, false);
    lc.setLed(0, 6, 0 + i, false);
   
  }
}

//StartGame function
void start() {
  if (gameJustStarted) {

    startingAnimation();
    currentLives = 3;
    startTime = millis();
    incrementTime= millis();
    levelValue = startingLevelValue;
    currentScore = 0;
    //easy
    if (levelValue == 0) {
      delaySpeedAsteroid = easyTime;
    }
    //medium
    if (levelValue == 1) {
      delaySpeedAsteroid = mediumTime;  
    }
    //hard
    if (levelValue == 2) {
      delaySpeedAsteroid = hardTime;
    }
    gameJustStarted = false;
  }
  playGame();
  if (millis() - incrementTime > 2000) {
    if (delaySpeedAsteroid > 50) { //each 50 millisec the speed will increase
      delaySpeedAsteroid = delaySpeedAsteroid - 20;  
    }
    if (delaySpeedAsteroid <= hardTime && levelValue < 2) {
      lastNewLevelPassed = millis();
      levelValue = 2;
      newLevelPassed = true;
    }
    if (delaySpeedAsteroid <= mediumTime && delaySpeedAsteroid >= hardTime && levelValue < 1) {
      levelValue = 1; 
      lastNewLevelPassed = millis();
      newLevelPassed = true;
    }
    if (delaySpeedAsteroid <= easyTime && delaySpeedAsteroid >= mediumTime && levelValue != 0) {
      levelValue = 0; 
      lastNewLevelPassed = millis(); 
      newLevelPassed = true;
    }
    incrementTime = millis();
  }
  
  displayStatus(currentLives, currentScore);
  if (endGame()) { //if lives are ended
    lc.clearDisplay(0);
    lcd.clear();
    if (currentScore > gameHighScore) {
      gameHighScore = currentScore;
      strncpy(bestPlayer, currentPlayer, MAX_NAME_LEN + 1);
      saveHighscore();
      lcd.print("New highscore!");
    }
    else {
      lcd.print("Try again!");      
    }
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
void displayStatus (int lives, int score) {
    if (millis() - lastDisplayMillis > displayDelay) {
      if (newLevelPassed == false) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Lives:");
        if (lives == 3) {
          lcd.write(1);
          lcd.write(1);
          lcd.write(1);
        }
        if (lives == 2) {
          lcd.write(1);
          lcd.write(1);
          lcd.write(2);        
        }
        if (lives == 1) {
          lcd.write(1);
          lcd.write(2);
          lcd.write(2);
        }
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(score);        
      }

      lastDisplayMillis = millis();
      if (newLevelPassed == true) { // show on LCD display "NEW LEVEL!" and difficulty when the level will change
        lcd.clear();
        lcd.print ("   NEW LEVEL!   ");
        lcd.setCursor(0, 1);
        if (levelValue == 1)
          lcd.print("     Medium     ");
        if (levelValue == 2)
          lcd.print("     Hard       ");
      }
      if (millis() - lastNewLevelPassed > 3500) {
        newLevelPassed = false;
        lastNewLevelPassed = millis();
      }
    }
}

//dead Animation
//delay() used for the animation
void deadAnimation() {  
  lcd.clear();
  lcd.print("   GAME OVER!   ");
  for (int x = 0; x < 8; x++) {
    lc.setLed(0, xAx+x, yAx+x, true);
    lc.setLed(0, xAx-x, yAx-x, true);
    lc.setLed(0, xAx+x, yAx-x, true);
    lc.setLed(0, xAx-x, yAx+x, true);
    lc.setLed(0, xAx-x, yAx, true);
    lc.setLed(0, xAx+x, yAx, true);
    lc.setLed(0, xAx, yAx+x, true);
    lc.setLed(0, xAx, yAx-x, true);

    tone(buzzerPin, 200 - (x*2), 100);
    delay (250);
  }
  lcd.clear();
  lcd.print("  FINAL SCORE");
  lcd.setCursor(0, 1);
  lcd.print("      ");
  lcd.print(currentScore);
  delay(4000);
}

//function that calculate if the lives are ended
int endGame() {
  if (currentLives == 0) {
    deadAnimation();
    return true;
  }
  return false;
}

//function that show the settings
void showSettings() {
    lcd.clear();   
    lcd.print("Level: ");
    if (startingLevelValue == 0) {
      lcd.print("Easy   ");
    }
    if (startingLevelValue == 1) {
      lcd.print("Medium ");      
    }
    if (startingLevelValue == 2) {
      lcd.print("Hard   ");
    }
    lcd.write(5);
    lcd.write(6);  
    lcd.setCursor(0, 1);
    lcd.print("Name: ");      
    lcd.print(currentPlayer);
    lcd.write(7);
    lastDisplayMillis = millis();
}

//Setting function
void settings() {
  if (millis() - lastDisplayMillis > displayDelay) {
    showSettings();
  }
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  spaceships[spaceshipNameIndex].toCharArray(currentPlayer, MAX_NAME_LEN);
  if (xValue < minThreshold && (!xJoyMoved)) {
    startingLevelValue--;
    if (startingLevelValue < 0) {
      startingLevelValue = 2;
    }
    xJoyMoved = true;
  }
  if (xValue > maxThreshold && (!xJoyMoved)) {
    startingLevelValue++;
    if (startingLevelValue > 2) {
      startingLevelValue = 0;
    }
    xJoyMoved = true;
  }
  if (xValue >= minThreshold && xValue <= maxThreshold){
    xJoyMoved = false;
  }
  
  if (yValue < minThreshold && (!yJoyMoved)) {
    spaceshipNameIndex--;
    if (spaceshipNameIndex < 0) {
      spaceshipNameIndex = NUM_NAME_SPACESHIP - 1;
    }
    yJoyMoved = true;
  }
  if (yValue > maxThreshold && (!yJoyMoved)) {
    spaceshipNameIndex++;
    if (spaceshipNameIndex > NUM_NAME_SPACESHIP - 1) {
      spaceshipNameIndex = 0;
    }
    yJoyMoved = true;
  }
  if (yValue >= minThreshold && yValue <= maxThreshold){
    yJoyMoved = false;
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
void info() {
    xValue = analogRead(pinX);
    if (xValue < minThreshold && (!xJoyMoved)) {
      infoIndex--;
      if (infoIndex < 0) {
        infoIndex = 3;
      }
      xJoyMoved = true;
    }
    else if (xValue > maxThreshold && (!xJoyMoved)) {
      infoIndex++;
      if (infoIndex > 3) {
        infoIndex = 0;
      }
      xJoyMoved = true;
    }
    if (xValue >= minThreshold && xValue <= maxThreshold){
      xJoyMoved = false;
    }
    if (buttonPressed()) {
      menuClicked = false;
    }  
  if (millis() - lastDisplayMillis > displayDelay) {
    lastDisplayMillis = millis();
    lcd.clear();

     
    if (infoIndex == 0) {
      lcd.print("<<   SPACE   >>");
      lcd.setCursor(0, 1);  
      lcd.print("     TRAVEL    ");  
    }
    if (infoIndex == 1) {
      lcd.print("<< Created by >>");  
      lcd.setCursor(0, 1);  
      lcd.print(" Saverio Telera ");  
    }
    if (infoIndex == 2) {
      yValue = analogRead(pinY);
      if (yValue < minThreshold && (!yJoyMoved)) {
        indexLink--;
        if (indexLink < 0) {
          indexLink = 2;
        }
        yJoyMoved = true;
      }
      if (yValue > maxThreshold && (!yJoyMoved)) {
        indexLink++;
        if (indexLink > 2) {
          indexLink = 0;
        }
        yJoyMoved = true;
      }
      if (yValue >= minThreshold && yValue <= maxThreshold){
        yJoyMoved = false;
      }
      if (indexLink == 0) {
        lcd.print("<<   Souce    >>");  
        lcd.setCursor(0, 1);
        lcd.print("https://github ");
        lcd.write(4);
      }
      if (indexLink == 1) {
        lcd.print(".com/Telera/In ");
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print("troduction-to- ");
        lcd.write(4);

      }
      if (indexLink == 2) {
        lcd.print("robotics       ");
        lcd.write(3);       
      }
    }
    if (infoIndex == 3) {
      lcd.print("<<  More info >>");  
      lcd.setCursor(0, 1);  
      lcd.print("@UnibucRobotics");  
    }    
  }
       
}

void setup() {
  //create custom characters
  lcd.createChar(1, heart);
  lcd.createChar(2, emptyHeart);
  lcd.createChar(3, arrowUp);
  lcd.createChar(4, arrowDown);
  lcd.createChar(5, arrowLeft);
  lcd.createChar(6, arrowRight);
  lcd.createChar(7, arrowUpDown);


  lcd.begin(16, 2);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);
  randomSeed(analogRead(0));
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  //clearDisk();
  loadHighscore();
  introAnimation();

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
    else if (menuSelected == 4) {
      info();
    }
  }

}
