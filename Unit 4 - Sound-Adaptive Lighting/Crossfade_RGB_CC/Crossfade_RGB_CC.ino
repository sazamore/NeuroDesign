/*
Fading RGB LED code - Common Cathode

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

const unsigned long STEP_INT = 10;  // how often to update brightness
const int STEP = 5;                    // how much to change per update; used in stepColor function

unsigned long startMillis = 0;  // stopwatch starts at 0.

// current brightness values
int currentR = 255;
int currentG = 0;
int currentB = 0;

// target color to fade toward
int targetR = 0;
int targetG = 255;
int targetB = 0;

// list of colors to cycle through. list store multiple values. Here we store an RGB combo as one value.
int colors[6][3] = {
  {255,   0,   0},  // red
  {  0, 255,   0},  // green
  {  0,   0, 255},  // blue
  {255, 255, 255},  // yellow
};
// TO DO: Use the colorwheel to ADD purple, turquoise and yellow to this lis.

int colorIndex = 1;  // start fading toward green

// Function nudges values one step toward target
int stepColor(int current, int target) {
  if (current < target) return min(current + STEP, target);
  if (current > target) return max(current - STEP, target);
  return current;
}

void setup() {
  // SET PIN MODE
  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);
}

void loop() {

  unsigned long currentMillis = millis(); // get current time

  if (currentMillis - startMillis >= STEP_INT) {
    // compare start and current time; if it's past the interval, time to do stuff.
    startMillis = currentMillis;  // reset the stopwatch

    // step each channel toward the target
    currentR = stepColor(currentR, targetR);
    currentG = stepColor(currentG, targetG);
    currentB = stepColor(currentB, targetB);

    analogWrite(RED_PIN,   currentR);
    analogWrite(GREEN_PIN, currentG);
    analogWrite(BLUE_PIN,  currentB);

    // if target is reached, pick the next color in the array
    if (currentR == targetR && currentG == targetG && currentB == targetB) {
      colorIndex = (colorIndex + 1) % 6;
      targetR = colors[colorIndex][0];
      targetG = colors[colorIndex][1];
      targetB = colors[colorIndex][2];
    }
  }

}