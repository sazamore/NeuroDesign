/*
--------------------------------------------------------------
Basic Microphone Plotter
--------------------------------------------------------------
Wiring:
•	VCC → 3.3V
•	GND → GND
•	AUD → A0 (or any analog pin)

Open Serial plotter to see voltage. Try whistling or clapping!
*/

const int MIC_PIN = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  float volts = analogRead(MIC_PIN) * (3.3 / 1023.0);
  Serial.println(volts);
}