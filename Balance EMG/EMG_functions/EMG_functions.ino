/*
  Unit 2 - EMG Pre-processing - Key Snippets
  
  This sketch contains functions for pre-processing RECTIFIED EMG signals. 
  
  This code assumes the original variable names. 

  This code will not run or compile on its own. You are intended to copy
  and paste blocks of code into your code.
  
  Hardware connections:
  - Rectified EMG signal - channel 1: A0
  - Rectified EMG signal - channel 2: A2
*/

/*
  HOW TO USE:
  
  Add a tab to your sketch. Title it EMG functions.
  Add these functions there and then call them in your main sketch (first tab).
  
  Add global variables above setup:
  int EMGsmth = 0;

  // Threshold settings
  const int THRESHOLD = 200;        // Threshold value (adjust based on your EMG)
  const int DEBOUNCE_TIME = 50;      // Debounce lockout time, in milliseconds

  // Timing variables
  unsigned long LAST_TRIGGER_TIME = 0;  // Last time threshold was crossed

  In the loop function:
  
  EMGfilt = bandpassFilter(EMGrect);
  EMGsmth = smoothSignal(EMGrect);
  checkThreshold(EMGsmth); or checkThreshold(EMGfilt);
  
*/

// ============= BANDPASS FILTER FUNCTION =============

int bandpassFilter(int inputValue) {
  // Local variables for filter (lowercase)
  static int previousValue1 = 0;  // create 
  static int previousValue2 = 0;
  static int filteredOutput = 0;
  
  // Simple moving average bandpass filter
  // This averages the current value with two previous values
  filteredOutput = (inputValue + previousValue1 + previousValue2) / 3;
  
  // Update previous values for next iteration
  previousValue2 = previousValue1;
  previousValue1 = inputValue;
  
  return filteredOutput;
}

// ============= ENVELOPE VIA SMOOTHING FUNCTION =============

int smoothSignal(int inputValue) {
  static int readings[400];              // Array to store last 400 readings (40ms window)
  static int readIndex = 0;              // Current position in array
  static long total = 0;                 // Running total of readings (using long for large sums)
  static int average = 0;                // Smoothed output value
  
  // Subtract the oldest reading from total
  total = total - readings[readIndex];
  
  // Store new reading in array
  readings[readIndex] = inputValue;
  
  // Add new reading to total
  total = total + readings[readIndex];
  
  // Move to next position in array
  readIndex = readIndex + 1;
  
  // If we reach the end, wrap around to beginning
  if (readIndex >= 400) {
    readIndex = 0;
  }
  
  // Calculate average
  average = total / 400;
  
  return average;
}


// ============= THRESHOLD DETECTION FUNCTION =============

void checkThreshold(int signalValue) {
  // Get current time
  unsigned long currentTime = millis();
  
  // Calculate time since last trigger
  unsigned long timeSinceLastTrigger = currentTime - LAST_TRIGGER_TIME;
  
  // Only check threshold if lockout period has passed
  if (timeSinceLastTrigger >= DEBOUNCE_TIME) {
    
    // Check if signal exceeds threshold
    if (signalValue > THRESHOLD) {
      
      // Threshold crossed! Move servo to active position
      // Place your servo command here. It can be: myservo.write(somePosition);
      // myservo.write(90); //demo code. change as needed.

      // Update last trigger time
      LAST_TRIGGER_TIME = currentTime;
      
      // Print trigger message
      Serial.println("THRESHOLD CROSSED!");
    } 
    // else {
    //   // Signal below threshold, add your servo actions
    // }
    
  } 
  else {
    // Still in lockout period - ignore signal
    Serial.println("Lockout period. No detection.");
  }
}
