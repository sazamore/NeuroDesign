/*
 * EMG Signal Processing - Envelope Detection & Debouncing
 * 
 * Functions for processing noisy rectified EMG signals
 * - Moving average envelope detection (40ms window)
 * - Debouncing to prevent multiple triggers
 */

// ===== CONFIGURATION =====
#define EMG_PIN A0              // Analog pin for EMG signal
#define THRESHOLD 500           // Threshold value (0-1023 for 10-bit ADC)
#define DEBOUNCE_TIME 300       // Debounce time in milliseconds
#define ENVELOPE_WINDOW_MS 40   // Moving average window (40ms)
#define SAMPLE_RATE_MS 5        // How often to sample EMG (5ms = 200Hz)

// Toggle for using envelope detection
bool useEnvelope = true;  // Set to false to use raw signal

// ===== GLOBALS =====
// Moving average buffer
#define BUFFER_SIZE (ENVELOPE_WINDOW_MS / SAMPLE_RATE_MS)  // 40ms / 5ms = 8 samples
int emgBuffer[BUFFER_SIZE];
int bufferIndex = 0;
long bufferSum = 0;

// Debouncing
unsigned long lastTriggerTime = 0;
bool isTriggered = false;

// Timing
unsigned long lastSampleTime = 0;

// ===== FUNCTIONS =====

/**
 * Initialize EMG processing
 * Call this in setup()
 */
void initEMG() {
  // Initialize buffer with zeros
  for (int i = 0; i < BUFFER_SIZE; i++) {
    emgBuffer[i] = 0;
  }
  bufferSum = 0;
  bufferIndex = 0;
  lastTriggerTime = 0;
  isTriggered = false;
  
  pinMode(EMG_PIN, INPUT);
}

/**
 * Moving Average Envelope Detection
 * 
 * @param newValue - New rectified EMG reading
 * @return Smoothed envelope value
 */
int getEnvelope(int newValue) {
  // Remove oldest value from sum
  bufferSum -= emgBuffer[bufferIndex];
  
  // Add new value to buffer and sum
  emgBuffer[bufferIndex] = newValue;
  bufferSum += newValue;
  
  // Move to next position (circular buffer)
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  
  // Return average
  return bufferSum / BUFFER_SIZE;
}

/**
 * Debounced Threshold Detection
 * 
 * @param signalValue - Current signal value (raw or envelope)
 * @param threshold - Threshold value to detect
 * @return true if threshold crossed AND debounce period has elapsed
 */
bool detectThresholdCrossing(int signalValue, int threshold) {
  unsigned long currentTime = millis();
  
  // Check if signal exceeds threshold
  bool aboveThreshold = (signalValue >= threshold);
  
  // Check if debounce period has elapsed
  bool debounceElapsed = (currentTime - lastTriggerTime) >= DEBOUNCE_TIME;
  
  // Trigger only if above threshold AND debounce time has passed
  if (aboveThreshold && debounceElapsed && !isTriggered) {
    lastTriggerTime = currentTime;
    isTriggered = true;
    return true;  // New trigger detected!
  }
  
  // Reset trigger flag when signal drops below threshold
  if (!aboveThreshold) {
    isTriggered = false;
  }
  
  return false;  // No trigger
}

/**
 * Process EMG Signal
 * Call this in loop() - handles timing automatically
 * 
 * @return true if threshold crossing detected (debounced)
 */
bool processEMG() {
  unsigned long currentTime = millis();
  
  // Check if it's time to sample
  if (currentTime - lastSampleTime < SAMPLE_RATE_MS) {
    return false;  // Not time to sample yet
  }
  lastSampleTime = currentTime;
  
  // Read raw EMG value (assumed to be already rectified)
  int rawEMG = analogRead(EMG_PIN);
  
  // Choose signal processing path
  int processedSignal;
  if (useEnvelope) {
    processedSignal = getEnvelope(rawEMG);
  } else {
    processedSignal = rawEMG;
  }
  
  // Detect threshold crossing with debouncing
  bool triggered = detectThresholdCrossing(processedSignal, THRESHOLD);
  
  // Optional: Print for debugging
  // Serial.print("Raw: "); Serial.print(rawEMG);
  // Serial.print(" | Envelope: "); Serial.print(processedSignal);
  // Serial.print(" | Triggered: "); Serial.println(triggered);
  
  return triggered;
}

// ===== EXAMPLE USAGE =====

void setup() {
  Serial.begin(115200);
  
  // Initialize EMG processing
  initEMG();
  
  Serial.println("EMG Processing Initialized");
  Serial.print("Envelope Detection: ");
  Serial.println(useEnvelope ? "ENABLED" : "DISABLED");
  Serial.print("Threshold: "); Serial.println(THRESHOLD);
  Serial.print("Debounce Time: "); Serial.print(DEBOUNCE_TIME); Serial.println(" ms");
  Serial.print("Envelope Window: "); Serial.print(ENVELOPE_WINDOW_MS); Serial.println(" ms");
}

void loop() {
  // Process EMG signal
  bool motorTrigger = processEMG();
  
  // If threshold crossed (with debouncing), activate motor
  if (motorTrigger) {
    Serial.println("*** MOTOR TRIGGERED ***");
    
    // Your motor control code here
    // digitalWrite(MOTOR_PIN, HIGH);
    // delay(1000);  // Motor on for 1 second
    // digitalWrite(MOTOR_PIN, LOW);
  }
  
  // Rest of your code here...
}

// ===== HELPER FUNCTION: Toggle Envelope On/Off =====

/**
 * Enable or disable envelope detection
 * 
 * @param enable - true to use envelope, false for raw signal
 */
void setEnvelopeMode(bool enable) {
  useEnvelope = enable;
  
  // Reset buffer when changing modes
  for (int i = 0; i < BUFFER_SIZE; i++) {
    emgBuffer[i] = 0;
  }
  bufferSum = 0;
  bufferIndex = 0;
  
  Serial.print("Envelope Detection: ");
  Serial.println(useEnvelope ? "ENABLED" : "DISABLED");
}
