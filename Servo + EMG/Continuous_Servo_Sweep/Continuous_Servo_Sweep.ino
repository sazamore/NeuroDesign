/*
Basic Seep for Continuous Servo Motors

The 360 degree servo will rotate in one direction (clockwise) 
for 5 seconds; stay still for 5 seconds and rotate in the 
reverse direction for another 5 seconds. This cycle repeats 
as long as the power is given.

Servo Circuit Diagram:
Brown - Gnd
Red - 5V
Orange - PWK analog signal pin 9

Modified from https://www.makerguides.com/how-to-control-a-360-degree-servo-motor-with-arduino/

*/

#include <Servo.h>

// Create the servo object
Servo myservo;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  myservo.write(90); // stop the motor
}

// Loop to keep the motor turning!
void loop() {
  myservo.write(45); // rotate the motor counter-clockwise
  delay(5000); // keep rotating for 5 seconds (5000 milliseconds)

  myservo.write(90); // stop the motor
  delay(5000); // stay stopped

  myservo.write(135); // rotate the motor clockwise
  delay(5000); // keep rotating 
}

