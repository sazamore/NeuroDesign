/*
  Two-Channel EMG Normalization (Use rectified )
  
  Workflow:
    1) On power-up: 2 s baseline capture at rest.
    2) Press the button to start a 5 s "maxTask" capture to learn the largest
       magnitude deviation from baseline (both directions).
    3) Afterwards, each sample is normalized to [0, +1] using that max deviation.

  Pins:
    A0 - EMG analog input
  
  Serial output columns:
    time_ms, raw_adc, baseline_adc, norm_float
*/

const int EMG_PIN = A0;

// --- Calibration parameters ---
const unsigned long BASELINE_MS = 2000;   // 2 s baseline at rest
const unsigned long MAXTASK_MS  = 8000;   // 5 s window to capture max deviation

// --- State variables ---
float baseline = 0.0f;     // average rest level (ADC units)
float maxDeviation = 1.0f; // largest |raw - baseline| observed during maxTask (ADC units)
bool  calibrated = false;
float countDown = 3;

// Simple time helper
unsigned long t0;

void waitCountdown(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    // just spin (no smoothing/averaging beyond what's needed during baseline)
  }
}

float readRaw() {
  // read once; no averaging, no filtering
  return (float)analogRead(EMG_PIN);
}

void captureBaseline() {
  const unsigned long start = millis();
  unsigned long n = 0;
  double sum = 0.0;

  while (millis() - start < BASELINE_MS) {
    sum += analogRead(EMG_PIN); // accumulate
    n++;
  }
  if (n == 0) n = 1;
  baseline = (float)(sum / (double)n);
}

void captureMaxTask() {
  // Instruct user via Serial
  Serial.println(F("Activate target muscle NOW (5 s)…"));
  const unsigned long start = millis();

  float localMaxDev = 1.0f; // avoid divide-by-zero if signal is flat

  while (millis() - start < MAXTASK_MS) {
    float raw = readRaw();
    float dev = raw - baseline; // magnitude WITHOUT rectifying the stream elsewhere
    // float dev = fabs(raw - baseline); // magnitude WITHOUT rectifying the stream elsewhere
    if (dev > localMaxDev) localMaxDev = dev;
  }
  maxDeviation = localMaxDev;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EMG_PIN, INPUT);

  Serial.begin(115200);
  while (!Serial) { /* wait for Serial on some boards */ }

  Serial.println(F("# Single-Channel EMG Normalization (no smoothing, no rectification)"));
  Serial.println(F("# Hold still. Capturing 2 s baseline…"));
  captureBaseline();
  Serial.print(F("# Baseline (ADC): "));
  Serial.println(baseline, 2);

  Serial.println(F("# Press button to start 5 s maxTask capture."));

  while (countDown > 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    countDown -= 1;
    Serial.print(countDown);
    Serial.println('...');
  }
  
  if (countDown == 0){
    captureMaxTask();
    Serial.print(F("# Max deviation (ADC): "));
    Serial.println(maxDeviation, 2);

    if (maxDeviation < 1.0f) {
      Serial.println(F("# Warning: very small deviation—normalization may amplify noise."));
      maxDeviation = 1.0f;
    }

    calibrated = true;
    t0 = millis();
    Serial.println(F("time_ms,raw_adc,baseline_adc,norm"));
  }

  // Blink 3 times to indicate calibration complete
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  // }
}

void loop() {
  float raw = readRaw();

  // Normalize to [0, +1] using baseline and maxDeviation (no smoothing/rectification)
  float norm = (raw - baseline) / maxDeviation;

  // Hard clip to [0, 1] to avoid occasional out-of-range spikes
  // if (norm > 1.0f)  norm = 1.0f;
  // if (norm < 0f) norm = 0f;

  unsigned long t = millis() - t0;

  // CSV-style print for easy plotting
  // Serial.print(raw, 2);
  // Serial.print(',');
  Serial.print(baseline, 2);
  Serial.print(',');
  Serial.println(norm, 4);

  // Small delay to keep output readable; remove if you need full sample rate
  delay(2);
}