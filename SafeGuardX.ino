#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
int pirPin = 2; // PIR sensor
int buzzerPin = 7; // Buzzer

const byte ROWS = 4; // 4 rows
const byte COLS = 4; // 4 columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8, 7, 6, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String password = "1234";
String inputPassword = "";
bool alarmActive = false;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Motion Detector");
  delay(2000);
  Serial.begin(9600);
}

void loop() {
  int motion = digitalRead(pirPin);
  if (motion == HIGH && !alarmActive) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Intruder Alert!");
    tone(buzzerPin, 1000);
    alarmActive = true;
  }
  
  if (alarmActive) {
    lcd.setCursor(0, 1);
    lcd.print("Enter Code:");
    char key = keypad.getKey();
    
    if (key) {
      if (key == '#') {
        if (inputPassword == password) {
          lcd.clear();
          lcd.print("Access Granted");
          noTone(buzzerPin);
          alarmActive = false;
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
