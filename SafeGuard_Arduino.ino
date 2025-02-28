#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DIYables_Keypad.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
int pirPin = 2; // PIR sensor
int buzzerPin = 3; // Buzzer

const int ROW_NUM = 4;     // four rows
const int COLUMN_NUM = 4;  // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pin_rows[ROW_NUM] = { 12, 11, 10, 9 };
byte pin_column[COLUMN_NUM] = { 8, 7, 6, 5 };

DIYables_Keypad keypad = DIYables_Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
String password = "1234";
String inputPassword = "";
bool alarmActive = false;
bool continuousAlarm = false;
unsigned long alarmStartTime = 0;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Motion Detector");
  Serial.begin(9600);
}

void loop() {
  int motion = digitalRead(pirPin);
  if (motion == HIGH && !alarmActive) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Intruder Alert!");
    alarmActive = true;
    alarmStartTime = millis();
  }

  if (alarmActive) {
    lcd.setCursor(0, 1);
    lcd.print("Enter Code:");
    unsigned long elapsedTime = millis() - alarmStartTime;

    if (elapsedTime < 10000) { // Първите 10 секунди кратки звуци
      if ((elapsedTime / 2000) % 2 == 0) {
        tone(buzzerPin, 1000);
      } else {
        noTone(buzzerPin);
      }
    } else { // След 10 секунди постоянен звук
      continuousAlarm = true;
      tone(buzzerPin, 1000);
    }

    char key = keypad.getKey();
    if (key) {
      Serial.print("Key pressed: ");
      Serial.println(key);
      
      if (key == '#') {
        Serial.print("Entered Password: ");
        Serial.println(inputPassword);
        if (inputPassword == password) {
          lcd.clear();
          lcd.print("Access Granted");
          Serial.println("Access Granted");
          noTone(buzzerPin);
          alarmActive = false;
          continuousAlarm = false;
        } else {
          lcd.clear();
          lcd.print("Wrong Code!");
          delay(1000);
        }
        inputPassword = ""; // Reset input
      } else {
        inputPassword += key;
        lcd.setCursor(11, 1);
        lcd.print("*");
      }
    }
  }
}