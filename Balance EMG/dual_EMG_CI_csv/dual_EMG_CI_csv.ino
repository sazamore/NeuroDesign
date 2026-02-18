/*
  Two-Channel EMG Normalization (Use rectified )
  
  Workflow:
    1) On power-up: 2 s baseline capture at rest.
    2) 3-second countdown for each channel -- mind which muscle goes to which channel!
    3) 8-second window for maximal contraction
    4) Afterwards, each sample is normalized to [0, 1] using that max deviation.

  Pins:
    A0 - EMG1 analog input (rectified)
    A2 - EMG2 analog input (rectified)

  Serial output columns:
    time_ms, raw_adc, baseline_adc, norm_float
*/

const int INTERVAL = 2;
unsigned long lastSample = 0;

// USE RECT SIGNALS ONLY.
const int EMG1_PIN = A0;
const int EMG2_PIN = A2;

// --- Calibration parameters ---
const unsigned long BASELINE_MS = 2000;   // 2 s baseline at rest
const unsigned long MAXTASK_MS  = 8000;   // 5 s window to capture max deviation

// --- State variables ---
float baseline = 0.0f;     // average rest level (ADC units)
float baseline1 = 0.0f;     // average rest level (ADC units)
float baseline2 = 0.0f;     // average rest level (ADC units)
float maxDeviation1 = 1.0f; // largest |raw - baseline| observed during maxTask (ADC units)
float maxDeviation2 = 1.0f; // largest |raw - baseline| observed during maxTask (ADC units)
bool  calibrated = false;
float countDown = 5;

// Simple time helper
unsigned long t0;

void waitCountdown(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    // just spin (no smoothing/averaging beyond what's needed during baseline)
  }
}

float readRaw(float mode) {
  // read once; no averaging, no filtering
  if (mode){
    return (float)analogRead(EMG1_PIN);
  }
  else return (float)analogRead(EMG2_PIN);
}

void captureBaseline() {
  unsigned long start = millis();
  unsigned long n = 0;
  double sum = 0.0;
  
  Serial.println("First baseline");
  while (millis() - start < BASELINE_MS) {
    sum += analogRead(EMG1_PIN); // accumulate
    n++;
  }
  if (n == 0) n = 1;
  baseline1 = (float)(sum / (double)n);
  
  n = 0;
  sum = 0.0;
  start = millis();
  Serial.println("Second baseline");
  while (millis() - start < BASELINE_MS) {
    sum += analogRead(EMG2_PIN); // accumulate
    n++;
  }
  if (n == 0) n = 1;
  baseline2 = (float)(sum / (double)n);
}

void captureMaxTask(int mode) {
  // Instruct user via Serial
  Serial.println(F("# Press and hold/perform your task NOW (5 s)…"));
    const unsigned long start = millis();

    float localMaxDev = 1.0f; // avoid divide-by-zero if signal is flat

    if (mode==0){
      Serial.println("Activate first muscle now.");
      baseline = baseline1;
    }
    else {
      Serial.println("Activate second muscle now.");
      baseline = baseline2;
    }

    while (millis() - start < MAXTASK_MS) {
      float raw = readRaw(mode);
      float dev = raw - baseline; // magnitude WITHOUT rectifying the stream elsewhere
      if (dev > localMaxDev) localMaxDev = dev;
    }
    if (mode){
    maxDeviation2 = localMaxDev;
    }
    else maxDeviation1 = localMaxDev;
    Serial.println("Rest.");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EMG1_PIN, INPUT);
  pinMode(EMG2_PIN, INPUT);
  // pinMode(BTN_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  while (!Serial) { /* wait for Serial on some boards */ }

  Serial.println(F("# Dual-Channel EMG Normalization (no smoothing, no rectification)"));
  Serial.println(F("# Hold still for 5 s. Capturing baselines on both channels..."));
  captureBaseline();
  Serial.print(F("# Baseline1 (ADC): "));
  Serial.println(baseline1, 2);  
  Serial.print(F("# Baseline2 (ADC): "));
  Serial.println(baseline2, 2);

  Serial.println(F("# Starting maximum voluntary contraction (MVC) capture."));

  for (int mode = 0; mode <2; mode++){ 
    int count = 3;

    while (count > 0){
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      count -= 1;
      Serial.print(round(count));
      Serial.println('...');
    }

    captureMaxTask(mode);
  }

  Serial.print(F("# Max deviation1 (ADC): "));
  Serial.println(maxDeviation1, 2);    
  Serial.print(F("# Max deviation2 (ADC): "));
  Serial.println(maxDeviation2, 2);

  // if (maxDeviation1 < 1.0f) {
  //   Serial.println(F("# Warning: very small deviation—normalization may amplify noise."));
  //   maxDeviation = 1.0f;
  // }

  calibrated = true;
  t0 = millis();
  Serial.println(F("time_ms,raw_adc,baseline_adc,norm"));

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

}

void loop() {
  float raw1 = readRaw(0);
  float raw2 = readRaw(1);

  // Normalize to [-1, +1] using baseline and maxDeviation (no smoothing/rectification)
  float norm1 = (raw1 - baseline1) / maxDeviation1;
  float norm2 = (raw2 - baseline2) / maxDeviation2;

  //calculate Co-activation ratio 
  float CI = min(norm1,norm2) / max(norm1,norm2);

  unsigned long t = millis() - t0;

  // Send serial data over UART
  if (millis() - lastSample >= INTERVAL) {

    // CSV-style print for easy plotting
    // Serial.print(baseline1, 2);
    // Serial.print(', ');
    // Serial.print(baseline2, 2);
    // Serial.print(', ');
    Serial.print(norm1, 1);  
    Serial.print(',');
    Serial.println(norm2, 1);  
    Serial.print(',');
    Serial.println(CI, 1);
  }

  // Small delay to keep output readable; remove if you need full sample rate
  // delay(2);
}