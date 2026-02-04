/*
  EMG Servo Control - Key Snippets
  
  This sketch contains functions for the EMG_Servo_Control_Base code. 
  
  This code assumes the original variable names. 

  This code will not run or compile on its own. You are intended to copy
  and paste blocks of code into your code.
  
  Hardware connections:
  - Raw EMG signal: A0
  - Rectified EMG signal: A1
  - Servo motor: Pin 9
*/

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

/*
  HOW TO USE:
  
  In your main loop, after filtering:
  
  EMGfilt = bandpassFilter(EMGrect);
  EMGsmth = smoothSignal(EMGrect);
  checkThreshold(EMGsmth);
  
  Don't forget to add a global variable at the top:
  int SMOOTHED_VALUE = 0;
*/

// ============= THRESHOLD DETECTION FUNCTION =============

void checkThreshold(int signalValue) {
  // Get current time
  unsigned long currentTime = millis();
  
  // Calculate time since last trigger
  unsigned long timeSinceLastTrigger = currentTime - LAST_TRIGGER_TIME;
  
  // Only check threshold if lockout period has passed
  if (timeSinceLastTrigger >= LOCKOUT_TIME) {
    
    // Check if signal exceeds threshold
    if (signalValue > THRESHOLD) {
      
      // Threshold crossed! Move servo to active position
      MYSERVO.write(SERVO_ACTIVE);
      
      // Update last trigger time
      LAST_TRIGGER_TIME = currentTime;
      
      // Print trigger message
      Serial.println("THRESHOLD CROSSED! Activate the servo .");
      
    } else {
      // Signal below threshold, add your servo actions
      Serial.println("Servo rest position or hold signal here.")
      //myservo.write(SOMEVALUEGOESHERE);
    }
    
  } else {
    // Still in lockout period - ignore signal
    // Servo stays in current position
    // myservo.write(SOMEVALUEGOESHERE);
  }
}
