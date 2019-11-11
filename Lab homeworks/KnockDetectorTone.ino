const int passiveBuzzerPin = A0; //knock detector
const int activeBuzzerPin = 12; //speaker
const int buttonPin = 13; //button to switch off the music

int passiveBuzzerValue = 0; 
int buttonValue = 0;

int knockTime = 0;
const int interval = 5000; // interval from the knock to the beginning of music

const int threshold = 100; // set the sensitivity of knock detector

bool flag = false; //boolean flag used to switch off the music once the button is pressed

void setup() { 
  pinMode(passiveBuzzerPin, INPUT_PULLUP); 
  pinMode(buttonPin, INPUT);
  pinMode(activeBuzzerPin, OUTPUT);
} 

void loop() { 
  int currentTime = millis();
  passiveBuzzerValue = analogRead(passiveBuzzerPin);
  buttonValue = digitalRead(buttonPin);
    
  // if passiveBuzzerValue > threshold a knock is detected
  if (passiveBuzzerValue > threshold) { 
    knockTime = currentTime;
    flag = true; //knock detected, flag used to remind that we want to turn on the music
  } 
  // check if a knock was detected and if it's been 5 second from the knock 
  if((flag == true) && (currentTime - knockTime >= interval)) {
    playTone();
    if ( buttonValue == LOW) { // check if the button is pressed
      flag = false;
    }
  }
} 

//function used to reproduce a tone
void playTone() {
  tone(activeBuzzerPin, 1000, 500);
}
