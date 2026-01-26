//*
//gripper 0 is closed
//gripper 180 is open
//wrist 180 is highest point
//wrist 50 is lowest point (lower will damage over time)
//

#include <Servo.h> 
Servo gripper, gripper_rot, wrist, shoulder, elbow;// elbow_rot;  // create servo object to control a servo 

int pos = 180; // initial motorposition

int sweep(Servo& theServo, int startAngle=0, int limitAngle=180){
  //THIS iS DIFFERENT - IT ONLY SWEEPS IN ONE DIRECTION
  if (startAngle < limitAngle){
    //small to large
    for (pos = startAngle; pos <= limitAngle; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      theServo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);  
      Serial.println(pos);                     // waits 15 ms for the servo to reach the position
    }
  }
  else{
    //large to small
    for (pos = startAngle; pos <= limitAngle; pos -= 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      theServo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);  
      Serial.println(pos);                     // waits 15 ms for the servo to reach the position
    }
  }
  delay(10);
  return pos;
}

void wave(){
  
}

void setup() {
  Serial.begin(9600);

  gripper.attach(3);  //the pin for the servoa control
  gripper_rot.attach(5);  //the pin for the servob control
  wrist.attach(6);  //the pin for the servoc control
  elbow.attach(9);  //the pin for the servod control 
  
  Serial.println("robot arm motor test starting..."); // so I can keep track of what is loaded

}

void loop() {

  //open the gripper
  sweep(gripper,180,0);
  gripper.write(0);
  delay(500);

  //drop the gripper
  // sweep(elbow,180,0);
  sweep(wrist,120,50);
  delay(50); 

// close the gripper
  sweep(gripper, 0, 180); 
  // gripper.write(180); //holds in place with tension (will overheat)

  //raise the gripper
  // sweep(elbow,0,180);
  sweep(wrist,60,180);
  sweep(wrist,180,60);

  wrist.write(180);

  delay(2000);

}


