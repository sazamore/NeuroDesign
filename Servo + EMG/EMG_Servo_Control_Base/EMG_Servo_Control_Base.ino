/*
  EMG Servo Control - Example Sketch
  
  This sketch reads EMG signals from two analog inputs (raw and rectified),
  applies a bandpass filter, detects threshold crossings, and controls a servo motor.
  
  Hardware connections:
  - Raw EMG signal: A0
  - Rectified EMG signal: A1
  - Servo motor: Pin 9
*/

#include <Servo.h>

// ============= GLOBAL VARIABLES (CAPITALS) =============

// Pin definitions
const int EMGpin = A0;           // Raw EMG input pin
const int RECTpin = A1;     // Rectified EMG input pin
const int SERVOpin = 9;          // Servo control pin

// Threshold settings
const int THRESHOLD = 200;        // Threshold value (adjust based on your EMG)
const int DEBOUNCE_TIME = 50;      // Debounce lockout time, in milliseconds
// Lockout can be the amount of time to execute the servo command.

// Servo positions
const int SERVOrest = 0;         // Servo rest position (degrees)
// Add more position variables here, if desired

// Timing variables
unsigned long LAST_TRIGGER_TIME = 0;  // Last time threshold was crossed

// Sensor readings
int EMGraw = 0;                // Raw EMG reading
int EMGrect = 0;          // Rectified EMG reading
int EMGfilt = 0;           // Filtered signal value

// Servo object
// Servo whichMotor; // select and assign the motor arm motor(s) here

// ============= SETUP FUNCTION =============

void setup() {
  // Initialize serial communication for monitoring
  Serial.begin(112500); // be sure to change your Serial plotter baud rate too!
  
  // Attach servo to pin
 // MYSERVO.attach(SERVO_PIN);
  
  // Set servo to rest position
 // MYSERVO.write(SERVO_REST);
  
  // Print startup message
 // Serial.println("EMG Servo Control Started");
  Serial.println("Waiting for EMG signals...");
}

// ============= MAIN LOOP =============

void loop() {
  // Read analog inputs
  EMGraw = analogRead(EMGpin);
  EMGrect = analogRead(RECTpin);
  
  // Apply bandpass filter to rectified signal
  EMGfilt = bandpassFilter(EMGrect);

  // If needed, place smoothing commands here

  // Place thresholding commands here
    
  // Print values to serial monitor (for debugging)
  Serial.print("Raw:");
  Serial.print(EMGraw);
  Serial.print(", ");
  Serial.print("Rectified:");
  Serial.print(EMGrect);
  Serial.print(", ");
  Serial.print("Filtered: ");
  Serial.println(EMGfilt);
  
}

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