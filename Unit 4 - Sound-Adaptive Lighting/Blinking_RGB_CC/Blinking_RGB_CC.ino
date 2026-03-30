/*
Simple RGB LED code - Common Cathode

Wiring:
  Common pin to GND
  RED to PWM pin 6
  GREEN to PWM pin 5
  BLUE to PWM pin 3

Uses millis() for future compatibility with microphone(s). Think of millis() as a stopwatch.
Call it once to start the watch (startMillis)
Call it again to change how much time has passed (currentMillis)
Subtract the two values to find the time elapsed: (currentMillis - startMillis)

*/  

const int RED_PIN   = 6;
const int GREEN_PIN = 5;
const int BLUE_PIN  = 3;

const unsigned long INTERVAL_MS = 500;    // delay time length

unsigned long startMillis = 0;   // Building compatibility with upcoming microphone work
bool ledOn = false;

void setup() {
  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - startMillis >= INTERVAL_MS) {
    // The time has passed the declared interval

    startMillis = currentMillis;  // update start time (reset the stopwatch)

    if (ledOn){
      analogWrite(RED_PIN,  255);
      analogWrite(GREEN_PIN, 0);  
      analogWrite(BLUE_PIN,  0);
    }
    else {
      analogWrite(RED_PIN,  0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN,  0);
    }

    ledOn = !ledOn; // update boolean to toggle light on/off

  }

}