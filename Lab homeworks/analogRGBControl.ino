const int pinRedLED = 11;
const int pinGreenLED = 10;
const int pinBlueLED = 9;

const int pinRedPOT = A0;
const int pinGreenPOT = A1;
const int pinBluePOT = A2;

int valueRedPOT = 0;
int mappedRedValuePOT = 0;

int valueGreenPOT = 0;
int mappedGreenValuePOT = 0;

int valueBluePOT = 0;
int mappedBlueValuePOT = 0;


void setup() {
  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);
  
  pinMode(pinRedPOT, INPUT);
  pinMode(pinGreenPOT, INPUT);
  pinMode(pinBluePOT, INPUT);
}

void loop() {
  valueRedPOT = analogRead(pinRedPOT);
  mappedRedValuePOT = map(valueRedPOT, 0, 1032, 0, 255);

  valueGreenPOT = analogRead(pinGreenPOT);
  mappedGreenValuePOT = map(valueGreenPOT, 0, 1032, 0, 255);

  valueBluePOT = analogRead(pinBluePOT);
  mappedBlueValuePOT = map(valueBluePOT, 0, 1032, 0, 255);
  
  setColor(mappedRedValuePOT, mappedGreenValuePOT, mappedBlueValuePOT);
}

void setColor(int red, int green, int blue) {
  analogWrite(pinRedLED, red);
  analogWrite(pinGreenLED, green);
  analogWrite(pinBlueLED, blue);
}
