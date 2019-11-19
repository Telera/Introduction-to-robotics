const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int pinSW = 0; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

unsigned int digitIndex = 0;
int digit1 = 0;
int digit2 = 0;
int digit3 = 0;
int digit4 = 0;

const int standardSwitchValue = HIGH; //button return HIGH if not pressed
int switchValue = LOW;
int xValue = 0;
int yValue = 0;
bool xJoyMoved = false;
bool yJoyMoved = false;

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

int dpState = LOW;

int currentNumber = 0;
unsigned long delayCounting = 20;  // incrementing interval
unsigned long lastIncreasing = 0;

int minThreshold= 400;
int maxThreshold= 600;

bool lockNumber = false;
//array of values
int values[noOfDisplays] = {
  digit1, digit2, digit3, digit4
};

// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
 };   
 
byte digitMatrix[noOfDigits][segSize - 1] = {
// a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};


void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  // write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);

}

// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}

//show on the 4 digit 7 segment display the updated value and turn on the dot of the number selected
void displayArrayValues (int numberSelected) {
  for (int i=0; i < noOfDisplays; i++) {
      showDigit(i);
      if (i == numberSelected ) {
        displayNumber(values[i], HIGH);
        delay(5);
      }
      else {
        displayNumber(values[i], LOW);
        delay(5);
      }
  }  
}

void setup() {
  for (int i = 0; i < segSize - 1; i++) {
    pinMode(segments[i], OUTPUT);  
  }
  for (int i = 0; i < noOfDisplays; i++) {
    pinMode(digits[i], OUTPUT);  
  }
  pinMode(pinSW, INPUT_PULLUP); //activate pull-up resistor on the 
                                // push-button pin
  pinMode(pinDP, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  switchValue = digitalRead(pinSW);
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  displayArrayValues(digitIndex);

  if (xValue < minThreshold && lockNumber == false && xJoyMoved == false) {
    xJoyMoved = true;
    if (digitIndex > 0) {
      digitIndex--;
    } else {
      digitIndex = (noOfDisplays - 1);
    }   
  }
  if(xValue > maxThreshold && lockNumber == false && xJoyMoved == false) {
    xJoyMoved = true;
    if (digitIndex < (noOfDisplays - 1)) {
      digitIndex++;
    } 
    else {
      digitIndex = 0;
    }
  }
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    xJoyMoved = false;
  }
  if(switchValue != standardSwitchValue && millis() - lastIncreasing >= delayCounting ) {
    lastIncreasing = millis();
    lockNumber = !lockNumber;
  }

  if ( lockNumber == true && millis() - lastIncreasing >= delayCounting) {
    lastIncreasing = millis();
    if (yValue < minThreshold && yJoyMoved == false) {
      yJoyMoved = true;
      if (values[digitIndex] > 0) {
        values[digitIndex]--;
      } 
      else {
        values[digitIndex] = 9;
      }    
    }
    if(yValue > maxThreshold && yJoyMoved == false) {
      yJoyMoved = true;
      if (values[digitIndex] < 9) {
        values[digitIndex]++;
        values[digitIndex] = values[digitIndex];
      } 
      else {
        values[digitIndex] = 0;
      } 
    }
    if (yValue >= minThreshold && yValue <= maxThreshold) {
      yJoyMoved = false;
    }
  }
}
