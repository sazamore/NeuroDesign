/*
 * Texture_Playback.ino
 * Haptic Mimicry Course — Stimulus Playback
 *
 * Enter amplitude (40–255) and granularity (2–30) via Serial Monitor.
 * Motor runs the stimulus while finger is on the device.
 * Press any key to stop.
 *
 * Wiring:
 *   DRV2605L GND  -> Arduino GND
 *   DRV2605L VCC  -> Arduino 5V
 *   DRV2605L SDA  -> Arduino A4
 *   DRV2605L SCL  -> Arduino A5
 *   DRV2605L EN   -> Arduino 5V
 *   DRV2605L IN   -> Arduino pin 9
 *   DRV2605L O+   -> motor red wire
 *   DRV2605L O-   -> motor blue wire
 */

#include <Wire.h>
#include <Sparkfun_DRV2605L.h>

SFE_HMD_DRV2605L HMD;

// ── pin ───────────────────────────────────────────────────────────────────────
const int PWM_PIN = 9;

// ── motor drive ───────────────────────────────────────────────────────────────
const int MOTOR_OFF = 127;
const int MIN_OUT   = 138;
const int MAX_OUT   = 255;

// ── parameter bounds ──────────────────────────────────────────────────────────
const int MIN_AMPLITUDE   = 40;
const int MAX_AMPLITUDE   = 255;
const int MIN_GRANULARITY = 2;
const int MAX_GRANULARITY = 30;

// ── motor helpers ─────────────────────────────────────────────────────────────
void motorOff() {
  analogWrite(PWM_PIN, MOTOR_OFF);
  delay(50);
  HMD.Mode(0x43);
  delay(10);
  HMD.Mode(0x03);
}

// ── prompt helper ─────────────────────────────────────────────────────────────
int promptInt(const __FlashStringHelper* label, int minVal, int maxVal) {
  while (true) {
    Serial.print(label);
    Serial.print(F(" ("));
    Serial.print(minVal);
    Serial.print(F("–"));
    Serial.print(maxVal);
    Serial.print(F("): "));

    while (!Serial.available());
    delay(50);

    // read all available chars as a string
    char buf[8];
    int  i = 0;
    while (Serial.available() && i < 7) {
      char c = Serial.read();
      if (c >= '0' && c <= '9') buf[i++] = c;
    }
    while (Serial.available()) Serial.read();
    buf[i] = '\0';

    if (i == 0) {
      Serial.println(F("No input received — try again."));
      continue;
    }

    int val = atoi(buf);
    if (val < minVal || val > maxVal) {
      Serial.print(F("Out of range — enter a value between "));
      Serial.print(minVal);
      Serial.print(F(" and "));
      Serial.println(maxVal);
      continue;
    }

    return val;
  }
}

// ── setup ─────────────────────────────────────────────────────────────────────
void setup() {
  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  pinMode(PWM_PIN, OUTPUT);
  digitalWrite(PWM_PIN, LOW);

  Serial.begin(9600);
  Wire.begin();

  HMD.begin();
  delay(100);
  HMD.Mode(0x03);
  HMD.MotorSelect(0x0A);
  HMD.Library(7);
  delay(100);

  analogWrite(PWM_PIN, MOTOR_OFF);

  Serial.println(F("Texture Playback ready."));
  Serial.println(F("---"));
}

// ── main loop ─────────────────────────────────────────────────────────────────
void loop() {

  int amplitude   = promptInt(F("Amplitude"),   MIN_AMPLITUDE,   MAX_AMPLITUDE);
  int granularity = promptInt(F("Granularity"), MIN_GRANULARITY, MAX_GRANULARITY);

  int pwmVal       = map(amplitude, MIN_AMPLITUDE, MAX_AMPLITUDE, MIN_OUT, MAX_OUT);
  int halfPeriodMs = 1000 / (granularity * 2);

  Serial.print(F("Running — Amp: "));
  Serial.print(amplitude);
  Serial.print(F("  Gran: "));
  Serial.print(granularity);
  Serial.println(F("  |  Press any key to stop."));

  bool    motorOn    = false;
  unsigned long lastToggle = millis();

  while (true) {
    // stop on any keypress
    if (Serial.available()) {
      while (Serial.available()) Serial.read();
      break;
    }

    if (millis() - lastToggle >= (unsigned long)halfPeriodMs) {
      motorOn = !motorOn;
      analogWrite(PWM_PIN, motorOn ? pwmVal : MOTOR_OFF);
      lastToggle = millis();
    }
  }

  motorOff();
  Serial.println(F("Stopped."));
  Serial.println(F("---"));
}