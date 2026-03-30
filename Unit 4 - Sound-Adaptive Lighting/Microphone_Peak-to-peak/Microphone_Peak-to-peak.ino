/* -----------------------------------------------
// SparkFun Electret Mic Breakout - Amplitude measure
// -----------------------------------------------
// Wiring:
//   Mic VCC → Arduino 3.3V   (use 3.3V, not 5V, for best results)
//   Mic GND → Arduino GND
//   Mic AUD → Arduino A0
// -----------------------------------------------

Measures sound for 50 ms, then updates max and min values accordingly.
Open the serial plotter to see values. Make sure Baud is set to 9600.

*/

const int MIC_PIN = A0;

// How long to collect samples before calculating a result (in ms).
// 50ms = 20 updates per second — fast enough for sound, smooth in plotter.
const int SAMPLE_WINDOW_MS = 50;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long start = millis();

  int signalMin = 1023;  // start high so any real reading beats it
  int signalMax = 0;     // start low so any real reading beats it

  // Collect readings for the duration of SAMPLE_WINDOW_MS
  while (millis() - start < SAMPLE_WINDOW_MS) {
    int sample = analogRead(MIC_PIN);
    if (sample > signalMax) signalMax = sample;
    if (sample < signalMin) signalMin = sample;
  }

  // Peak-to-peak amplitude: the difference between the loudest
  // and quietest reading in the sample window.
  // In silence, the mic idles around 512 (half of 3.3V),
  // so min and max stay close together → low peak-to-peak value.
  // Loud sounds push them apart → high peak-to-peak value.
  int peakToPeak = signalMax - signalMin;

  // Convert to voltage for a more meaningful unit (0.0 – 3.3V)
  float volts = (peakToPeak * 3.3) / 1024.0;

  // Send both values to Serial Plotter.
  // Label format "Name:value" lets the plotter show a legend.
  Serial.print("PeakToPeak:");
  Serial.print(peakToPeak);
  Serial.print(" Volts:");
  Serial.println(volts);
}