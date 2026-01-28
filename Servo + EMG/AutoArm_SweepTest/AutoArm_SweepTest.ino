#include <Servo.h> 
Servo gripper, gripper_rot, wrist, shoulder;//, elbow, elbow_rot;  // create servo object to control a servo 

int pos = 180; // initial motorposition

void sweep(Servo& theServo, int limitAngle=180){
  for (pos = 0; pos <= limitAngle; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    theServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);  
    Serial.println(pos);                     // waits 15 ms for the servo to reach the position
  }
  delay(10);

  for (pos = limitAngle; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    theServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);  
    Serial.println(pos);                      // waits 15 ms for the servo to reach the position
  }
}

void wave(){
  
}

void setup() {
  Serial.begin(9600);

  gripper.attach(3);  //the pin for the servoa control
  gripper_rot.attach(5);  //the pin for the servob control
  wrist.attach(6);  //the pin for the servoc control
  shoulder.attach(9);
  // elbow.attach(9);  //the pin for the servod control 
  // elbow_rot.attach(9);  //the pin for the servod control 
  
  Serial.println("robot arm motor test starting..."); // so I can keep track of what is loaded

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Gripper");
  sweep(gripper, 180);
  gripper.write(0);  
  
  // tell servo to go to position in variable 'pos'
  sweep(gripper_rot, 180);
  delay(20);

  delay(20);
  Serial.println("Wrist");
  sweep(wrist, 180);

  // Serial.println("Wrist");
  sweep(shoulder, 180);

}


