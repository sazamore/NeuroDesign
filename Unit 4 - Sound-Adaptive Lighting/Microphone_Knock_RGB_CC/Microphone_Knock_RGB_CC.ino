/* -----------------------------------------------
// SparkFun Electret Mic Breakout + RGB LED
// Knock detector — triggers white LED on knock/loud sound
// -----------------------------------------------
// Wiring:
//   Mic VCC → 3.3V  |  Mic GND → GND  |  Mic AUD → A0
//   RGB Red   → Pin 6  (with 220Ω resistor)
//   RGB Green → Pin 5  (with 220Ω resistor)
//   RGB Blue  → Pin 3  (with 220Ω resistor)
//   RGB Common Cathode → GND
// -----------------------------------------------
*/

const int MIC_PIN     = A0;
const int RED_PIN     = 6;
const int GREEN_PIN   = 5;
const int BLUE_PIN    = 3;

const int SAMPLE_WINDOW_MS = 250;  // collect samples for 250ms (4Hz)
const float TRIGGER_VOLTS  = 1.0;  // adjust this threshold to taste
const int LED_ON_MS        = 500;  // how long the LED stays on after a knock

unsigned long ledOnAt = 0;
bool ledActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);
}

void loop() {
  // --- Sample the microphone ---
  unsigned long start = millis();
  int signalMin = 1023;
  int signalMax = 0;

  while (millis() - start < SAMPLE_WINDOW_MS) {
    int sample = analogRead(MIC_PIN);
    if (sample > signalMax) signalMax = sample;
    if (sample < signalMin) signalMin = sample;
  }

  int peakToPeak = signalMax - signalMin;
  float volts = (peakToPeak * 3.3) / 1024.0;

  Serial.println(volts);

  // --- Trigger LED if loud enough ---
  if (volts >= TRIGGER_VOLTS) {
    ledActive = true;
    ledOnAt = millis();
  }

  // --- Turn LED on or off based on timer ---
  if (ledActive) {
    // note that the values are NOT 255. This effectively dims the LED
    analogWrite(RED_PIN,   200);
    analogWrite(GREEN_PIN, 200);
    analogWrite(BLUE_PIN,  200);

    if (millis() - ledOnAt >= LED_ON_MS) {
      ledActive = false;
      // THINK! How would the code below change if the LED were to fade off?
      analogWrite(RED_PIN,   0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN,  0);
    }
  }
}