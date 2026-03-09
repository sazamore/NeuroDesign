/*
 * Adaptive_Roughness_Interleaved.ino
 * Haptic Mimicry Course — Adaptive Texture Tuning
 *
 * Two interleaved adaptive tracks.
 * Track A starts ROUGH  (amplitude high, descending)
 * Track B starts SMOOTH (amplitude low,  ascending)
 *
 * Tracks alternate randomly. PSE = mean of both convergence points.
 * Each stage ends when BOTH tracks reach REVERSALS_TO_LOCK.
 *
 * Stage 1 adapts amplitude.   Stage 2 adapts granularity.
 *
 * RESPONSES:
 *   1 = smoother  (correct when active track stepped DOWN)
 *   2 = rougher   (correct when active track stepped UP)
 *   3 = same      (correct only on catch trials)
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

// ── stimulus timing ───────────────────────────────────────────────────────────
const int TRIAL_DURATION = 3000;
const int REST_DURATION  = 1500;

// ── parameter bounds ──────────────────────────────────────────────────────────
const int MIN_AMPLITUDE   = 100;
const int MAX_AMPLITUDE   = 255;
const int INIT_STEP_AMP   = 32;
const int MIN_STEP_AMP    = 8;

const int MIN_GRANULARITY = 2;
const int MAX_GRANULARITY = 30;
const int INIT_STEP_GRAN  = 4;
const int MIN_STEP_GRAN   = 1;

const int REVERSALS_TO_LOCK = 4;

// ── track struct ──────────────────────────────────────────────────────────────
struct Track {
  const char* label;       // "A" or "B" — internal use only
  int  amplitude;
  int  granularity;
  int  stepAmp;
  int  stepGran;
  int  lastDirAmp;
  int  lastDirGran;
  int  reversalsAmp;
  int  reversalsGran;
  int  lastDirection;      // +1 = last step was UP, -1 = DOWN
  bool done;
};

Track tA = { "A", MAX_AMPLITUDE, MIN_GRANULARITY,
             INIT_STEP_AMP, INIT_STEP_GRAN,
             0, 0, 0, 0, -1, false };   // starts ROUGH, steps DOWN

Track tB = { "B", MIN_AMPLITUDE, MIN_GRANULARITY,
             INIT_STEP_AMP, INIT_STEP_GRAN,
             0, 0, 0, 0, +1, false };   // starts SMOOTH, steps UP

// ── global state ──────────────────────────────────────────────────────────────
int  stage       = 1;
int  trialCount  = 0;
int  catchCount  = 0;
int  catchErrors = 0;

// ── motor helpers ─────────────────────────────────────────────────────────────
void motorOff() {
  analogWrite(PWM_PIN, MOTOR_OFF);
  delay(50);
  HMD.Mode(0x43);
  delay(10);
  HMD.Mode(0x03);
}

void runStimulus(int amplitude, int granularity) {
  int pwmVal       = map(amplitude, MIN_AMPLITUDE, MAX_AMPLITUDE, MIN_OUT, MAX_OUT);
  int halfPeriodMs = 1000 / (granularity * 2);
  bool motorOn     = false;
  unsigned long lastToggle = millis();
  unsigned long start      = millis();

  while (millis() - start < TRIAL_DURATION) {
    if (millis() - lastToggle >= (unsigned long)halfPeriodMs) {
      motorOn = !motorOn;
      analogWrite(PWM_PIN, motorOn ? pwmVal : MOTOR_OFF);
      lastToggle = millis();
    }
  }
  motorOff();
}

// ── adapt: update one track ───────────────────────────────────────────────────
void adapt(Track &t, int response) {
  int correctResponse = (t.lastDirection == 1) ? 2 : 1;
  bool correct = (response == correctResponse);

  Serial.print(F("  ["));
  Serial.print(t.label);
  Serial.print(F("] Correct: "));
  Serial.print(correctResponse == 2 ? F("2 (rougher)") : F("1 (smoother)"));
  Serial.print(F("  Got: "));
  Serial.print(response == 1 ? F("1 (smoother)") : F("2 (rougher)"));
  Serial.println(correct ? F("  CORRECT") : F("  WRONG"));

  int newDir = correct ? t.lastDirection : -t.lastDirection;

  if (stage == 1) {
    t.amplitude = constrain(t.amplitude + (newDir * t.stepAmp),
                            MIN_AMPLITUDE, MAX_AMPLITUDE);
    if (t.lastDirAmp != 0 && newDir != t.lastDirAmp) {
      t.reversalsAmp++;
      t.stepAmp = max(t.stepAmp / 2, MIN_STEP_AMP);
      Serial.print(F("  ** Reversal ["));
      Serial.print(t.label);
      Serial.print(F("] total: "));
      Serial.println(t.reversalsAmp);
      if (t.reversalsAmp >= REVERSALS_TO_LOCK) t.done = true;
    }
    t.lastDirAmp = newDir;
  } else {
    t.granularity = constrain(t.granularity + (newDir * t.stepGran),
                              MIN_GRANULARITY, MAX_GRANULARITY);
    if (t.lastDirGran != 0 && newDir != t.lastDirGran) {
      t.reversalsGran++;
      t.stepGran = max(t.stepGran / 2, MIN_STEP_GRAN);
      Serial.print(F("  ** Reversal ["));
      Serial.print(t.label);
      Serial.print(F("] total: "));
      Serial.println(t.reversalsGran);
      if (t.reversalsGran >= REVERSALS_TO_LOCK) t.done = true;
    }
    t.lastDirGran = newDir;
  }

  t.lastDirection = newDir;
}

// ── stage completion ──────────────────────────────────────────────────────────
void checkStageComplete() {
  if (!tA.done || !tB.done) return;

  if (stage == 1) {
    int psAmp = (tA.amplitude + tB.amplitude) / 2;
    Serial.println(F("---"));
    Serial.println(F("Stage 1 complete."));
    Serial.println(F("Adaptive testing returned two amplitude thresholds:"));
    Serial.print(F("  Upper threshold: ")); Serial.println(tA.amplitude);
    Serial.print(F("  Lower threshold: ")); Serial.println(tB.amplitude);
    Serial.print(F("  Locked amplitude (mean): ")); Serial.println(psAmp);

    tA.amplitude     = psAmp;
    tB.amplitude     = psAmp;
    tA.granularity   = MAX_GRANULARITY;
    tB.granularity   = MIN_GRANULARITY;
    tA.stepGran      = INIT_STEP_GRAN;
    tB.stepGran      = INIT_STEP_GRAN;
    tA.lastDirGran   = 0;
    tB.lastDirGran   = 0;
    tA.reversalsGran = 0;
    tB.reversalsGran = 0;
    tA.lastDirection = -1;
    tB.lastDirection = +1;
    tA.done          = false;
    tB.done          = false;
    stage            = 2;

    Serial.println(F("Starting Stage 2 — granularity adaptation."));
    Serial.println(F("---"));
    return;
  }

  if (stage == 2) {
    int psGran = (tA.granularity + tB.granularity) / 2;
    Serial.println(F("---"));
    Serial.println(F("Experiment complete."));
    Serial.println(F("Adaptive testing returned two granularity thresholds:"));
    Serial.print(F("  Upper threshold: ")); Serial.println(tA.granularity);
    Serial.print(F("  Lower threshold: ")); Serial.println(tB.granularity);
    Serial.print(F("  Locked granularity (mean): ")); Serial.println(psGran);
    Serial.print(F("  Locked amplitude:          ")); Serial.println(tA.amplitude);
    Serial.print(F("  Catch errors: "));
    Serial.print(catchErrors);
    Serial.print(F(" / "));
    Serial.println(catchCount);
    Serial.println(F("Upload Serial Monitor log to save data."));
    while (1);
  }
}

// ── logging ───────────────────────────────────────────────────────────────────
void logTrial(Track &t, int response, bool isCatch) {
  Serial.print(F("Trial ")); Serial.print(trialCount);
  Serial.print(F(" | Stage ")); Serial.print(stage);
  Serial.print(F(" | SC ")); Serial.print(t.label);
  Serial.print(isCatch ? F(" | CATCH") : F(" | REAL "));
  Serial.print(F(" | Amp ")); Serial.print(t.amplitude);
  Serial.print(F(" | Gran ")); Serial.print(t.granularity);
  Serial.print(F(" | Response ")); Serial.println(response);
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
  randomSeed(analogRead(A0));

  Serial.println(F("Adaptive Roughness Tuning."));
  Serial.println(F("The test will adapt the stimulus based on your responses."));
  Serial.println(F("Responses:  1 = smoother   2 = rougher   3 = same (catch only)"));
  Serial.println(F("---"));
}

// ── main loop ─────────────────────────────────────────────────────────────────
void loop() {

  bool catchTrial = (random(0, 5) == 0);

  Track* t = nullptr;
  if (!tA.done && !tB.done) {
    t = (random(0, 2) == 0) ? &tA : &tB;
  } else if (!tA.done) {
    t = &tA;
  } else if (!tB.done) {
    t = &tB;
  } else {
    checkStageComplete();
    return;
  }

  Serial.println(catchTrial
    ? F("CATCH TRIAL — Press Enter when ready.")
    : F("Press Enter when ready."));

  while (!Serial.available());
  while (Serial.available()) Serial.read();

  Serial.println(F("Running stimulus..."));
  runStimulus(t->amplitude, t->granularity);
  delay(REST_DURATION);

  Serial.println(catchTrial
    ? F("Respond:  1 = smoother   2 = rougher   3 = same")
    : F("Respond:  1 = smoother   2 = rougher"));

  while (!Serial.available());
  delay(50);
  char responseChar = Serial.read();
  while (Serial.available()) Serial.read();
  int response = responseChar - '0';

  if (response < 1 || response > 3) {
    Serial.println(F("Invalid — trial not counted."));
    return;
  }

  trialCount++;

  if (catchTrial) {
    catchCount++;
    if (response != 3) {
      catchErrors++;
      Serial.println(F("  CATCH ERROR — adapting."));
      int wrongResponse = (t->lastDirection == 1) ? 1 : 2;
      adapt(*t, wrongResponse);
    } else {
      Serial.println(F("  CATCH CORRECT."));
    }
    Serial.print(F("  Catch false alarm rate: "));
    Serial.print(catchErrors);
    Serial.print(F(" / "));
    Serial.println(catchCount);
    logTrial(*t, response, true);
    checkStageComplete();
    return;
  }

  logTrial(*t, response, false);
  adapt(*t, response);
  checkStageComplete();
}
