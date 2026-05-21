#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IN1 6
#define IN2 9
#define ENA 5
#define encoderPin 3
#define POT A0

LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile unsigned int pulseCount = 0;
unsigned long lastMeasurementTime = 0;
const int measurementInterval = 1000;  // 1 second
const float PPR = 440.33;  // After test

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(POT, INPUT);
  pinMode(encoderPin, INPUT_PULLUP);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  attachInterrupt(digitalPinToInterrupt(encoderPin), countPulse, CHANGE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Motor Speed control
  int potValue = analogRead(POT);   // 0 - 1023
  int motorPWM;
  int inputRPM = map(potValue, 0, 1023, 0, 20); // Estimated Input RPM

  if (potValue == 0) {
    motorPWM = 0; // Motor off
  } else {
    motorPWM = map(potValue, 1, 1023, 60, 255); // Spins at 1 RPM or more
  }

  analogWrite(ENA, motorPWM);

  // Display estimated input RPM (first row)
  lcd.setCursor(0, 0);
  lcd.print("Input: ");
  lcd.print(inputRPM);
  lcd.print(" RPM   ");

  // Every 1 second, calculate actual output RPM
  if (millis() - lastMeasurementTime >= measurementInterval) {
    noInterrupts();
    unsigned int pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    float rotations = pulses / PPR;
    float outputRPM = rotations * 60.0;  // per minute

    if (outputRPM > 20.0) {
      outputRPM = 20.0;
    }

    // Display on LCD (second row)
    lcd.setCursor(0, 1);
    lcd.print("Output: ");
    lcd.print(outputRPM, 1);
    lcd.print(" RPM  ");

    lastMeasurementTime = millis();
  }
}

void countPulse() {
  pulseCount++;
}
