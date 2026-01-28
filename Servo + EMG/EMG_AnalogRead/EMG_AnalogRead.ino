// Myoware 2.0 Muscle Sensor Example Code for Arduino UNO
int muscleSensorPin = A0; // Connect the sensor's SIG pin to A0
int muscleSensorValue = 0; // Variable to store the sensor value

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud
  pinMode(A0, INPUT);

}

void loop() {
  muscleSensorValue = analogRead(muscleSensorPin); // Read the sensor value
  Serial.println(muscleSensorValue); // Print the sensor value to the serial monitor
  // Serial.print(", "); // Print the sensor value to the serial monitor
  // Serial.println(600); // Print the sensor value to the serial monitor

  delay(20); // Short delay for stability
}