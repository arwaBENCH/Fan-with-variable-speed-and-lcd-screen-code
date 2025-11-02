#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C address and size (0x27 for most)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
const int fanPin = 9;
const int buttonPin = 2;
const int potPin = A0;
const int ledGreenPin = 8;
const int ledRedPin = 7;

bool fanOn = false;
int lastSpeedPercent = -1;

void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);

  digitalWrite(fanPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledRedPin, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ventilateur OFF");
  lcd.setCursor(0, 1);
  lcd.print("Fan is OFF     ");

  Serial.begin(9600);
  Serial.println("System started. Fan is OFF.");
}

void loop() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(buttonPin);

  // Toggle fan state on button press
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    fanOn = !fanOn;
    Serial.println(fanOn ? "Fan turned ON" : "Fan turned OFF");
    delay(200);  // debounce
    lastSpeedPercent = -1; // force LCD update
  }
  lastButtonState = currentButtonState;

  if (fanOn) {
    int potValue = analogRead(potPin);
    int pwmValue = map(potValue, 0, 1023, 0, 255);
    int speedPercent = map(pwmValue, 0, 255, 0, 100);

    analogWrite(fanPin, pwmValue);
    digitalWrite(ledGreenPin, HIGH);
    digitalWrite(ledRedPin, LOW);

    if (speedPercent != lastSpeedPercent) {
      lcd.setCursor(0, 0);
      lcd.print("Ventilateur ON ");
      lcd.setCursor(0, 1);
      lcd.print("Vitesse: ");
      lcd.print(speedPercent);
      lcd.print("%   "); // clear leftovers

      Serial.print("Fan speed: ");
      Serial.print(speedPercent);
      Serial.println("%");

      lastSpeedPercent = speedPercent;
    }

  } else {
    analogWrite(fanPin, 0);
    digitalWrite(ledGreenPin, LOW);
    digitalWrite(ledRedPin, HIGH);

    // Update LCD only once
    if (lastSpeedPercent != -2) {
      lcd.setCursor(0, 0);
      lcd.print("Ventilateur OFF");
      lcd.setCursor(0, 1);
      lcd.print("Fan is OFF     ");

      Serial.println("Fan is OFF");
      lastSpeedPercent = -2; // mark as off
    }

    delay(300); // prevent flooding
  }

  delay(100); // short delay for stability
}
