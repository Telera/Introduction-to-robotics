const int pinRedLed = 11;
const int pinGreenLed = 10;
const int pinBlueLed = 9;

const int pinRedPot = A0;
const int pinGreenPot = A1;
const int pinBluePot = A2;

int valueRedPot = 0;
int valueGreenPot = 0;
int valueBluePot = 0;

int mappedRedValuePot = 0;
int mappedGreenValuePot = 0;
int mappedBlueValuePot = 0;

void setup() {
  pinMode(pinRedLed, OUTPUT);
  pinMode(pinGreenLed, OUTPUT);
  pinMode(pinBlueLed, OUTPUT);
  
  pinMode(pinRedPot, INPUT);
  pinMode(pinGreenPot, INPUT);
  pinMode(pinBluePot, INPUT);
}

void loop() {
  valueRedPot = analogRead(pinRedPot); //potentiometer value acquisition 
  mappedRedValuePot = map(valueRedPot, 0, 1023, 0, 255); //map the value of the potentiometer

  valueGreenPot = analogRead(pinGreenPot);
  mappedGreenValuePot = map(valueGreenPot, 0, 1023, 0, 255);

  valueBluePot = analogRead(pinBluePot);
  mappedBlueValuePot = map(valueBluePot, 0, 1023, 0, 255);
  
  setColor(mappedRedValuePot, mappedGreenValuePot, mappedBlueValuePot); //call the function using mapped values from potentiometers
}

//function that manage each leds of our RGB led
void setColor(int red, int green, int blue) {
  analogWrite(pinRedLed, red);
  analogWrite(pinGreenLed, green);
  analogWrite(pinBlueLed, blue);
}
