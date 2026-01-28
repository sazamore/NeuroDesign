/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int signal = 0; // define signal variable (we will replace this with EMG signals)

// EMG setup
int EMGrect = A0;   // input pin for the potentiometer
int EMGvoltage = 0; // stores the EMG signal that is coming in.

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  Serial.begin(9600);  // initiate serial communication for plotting
}

void loop() {
  // uncomment to read EMG sensor
  // EMGvoltage = analogRead(EMGpin);

  // randomly set the value of signal to something that will run the servo or not
  if (random(2)>0){
    signal = 60;
  }
  else{
    signal = 0;
  }

  // to use EMG input. REPLACE line 30 with the line below (uncommented)
  // if (EMGvoltage >= threshold){

  Serial.println(signal); //print signal value

  // The servo movement that should happen with muscle contraction
  // check the signal value and operate ONLY when conditions are met
  if (signal >= 50){
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }
  }
  else{
    // for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(0);              // tell servo to go to position in variable 'pos'
      // delay(15);                       // waits 15 ms for the servo to reach the position
    // }
  }
}
