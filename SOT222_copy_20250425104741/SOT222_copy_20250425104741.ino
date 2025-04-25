#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
// LCD настройки (I2C адрес може да е 0x27 или 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
// PIR сензори
const int pirPins[3] = {22, 24, 26};
 
// Buzzer
const int buzzerPin = 46;
 
// ПИН код за достъп
String correctCode = "1234";
String enteredCode = "";
 
// Състояние на системата
bool systemArmed = false;
bool alarmTriggered = false;
 
// Настройки за Keypad 4x4
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {39, 41, 43, 45};
byte colPins[COLS] = {47, 49, 51, 53};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
 
// Таймер за PIR проверка (non-blocking)
unsigned long lastPIRCheck = 0;
const unsigned long pirInterval = 500; // 500 ms
 
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN:");
 
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
 
  for (int i = 0; i < 3; i++) {
    pinMode(pirPins[i], INPUT);
  }
}
 
void loop() {
  char key = keypad.getKey();
 
  // Работа с клавиатурата
  if (key) {
    if (key == '#') {
      if (enteredCode == correctCode) {
        systemArmed = !systemArmed;
        alarmTriggered = false;
 
        lcd.clear();
        lcd.setCursor(0, 0);
        if (systemArmed) {
          lcd.print("System ARMED");
        } else {
          lcd.print("System DISARMED");
          digitalWrite(buzzerPin, LOW);  // Спиране на бузера, ако е деактивирана системата
        }
 
        delay(2000);
        lcd.clear();
        lcd.print("Enter PIN:");
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong PIN");
        delay(2000);
        lcd.clear();
        lcd.print("Enter PIN:");
      }
      enteredCode = "";
    } else if (key == '*') {
      enteredCode = "";
      lcd.clear();
      lcd.print("Enter PIN:");
    } else {
      enteredCode += key;
      lcd.setCursor(0, 1);
      lcd.print(enteredCode);
    }
  }
 
  // Проверка на PIR сензори при активна система
  if (systemArmed && millis() - lastPIRCheck >= pirInterval) {
    lastPIRCheck = millis();
 
    for (int i = 0; i < 3; i++) {
      int val = digitalRead(pirPins[i]);
      Serial.print("PIR ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(val);
 
      if (val == HIGH) {
        triggerAlarm();
        break;
      }
    }
  }
 
  // Ако системата е деактивирана, спираме бузера
  if (!systemArmed && alarmTriggered) {
    digitalWrite(buzzerPin, LOW);  // Спиране на бузера
    alarmTriggered = false;  // Спиране на алармата
  }
}
 
void triggerAlarm() {
  if (!alarmTriggered) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INTRUDER ALERT!");
 
    // Стартиране на алармата
    while (systemArmed) {  // Алармата свири, докато системата е активирана
      tone(buzzerPin, 1000);
      delay(200);
      tone(buzzerPin, 1000);
      delay(200);
 
      // Проверка за въведен PIN
      char key = keypad.getKey();
      if (key) {
        if (key == '#') {
          if (enteredCode == correctCode) {
            systemArmed = false;  // Деактивиране на системата
            alarmTriggered = false;  // Спиране на алармата
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("System DISARMED");
            digitalWrite(buzzerPin, LOW);  // Спиране на бузера
            break;  // Прекратяване на алармата
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Wrong PIN");
            delay(2000);
            lcd.clear();
            lcd.print("Enter PIN:");
            enteredCode = "";
          }
        } else if (key == '*') {
          enteredCode = "";
          lcd.clear();
          lcd.print("Enter PIN:");
        } else {
          enteredCode += key;
          lcd.setCursor(0, 1);
          lcd.print(enteredCode);
        }
      }
    }
 
    alarmTriggered = true;
    lcd.clear();
    lcd.print("Enter PIN:");
  }
}