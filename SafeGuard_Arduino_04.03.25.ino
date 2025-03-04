#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DIYables_Keypad.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int pirPin = 2;     // PIR сензор
int buzzerPin = 3;  // Зумер

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

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
bool systemArmed = false;
bool alarmActive = false;
unsigned long alarmStartTime = 0;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Security System");
  lcd.setCursor(0, 1);
  lcd.print("Enter Code...");

  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    if (key == '#') {
      Serial.print("Entered Password: ");
      Serial.println(inputPassword);

      if (inputPassword == password) {
        if (systemArmed) {
          deactivateSystem();
        } else {
          activateSystem();
        }
      } else {
        lcd.clear();
        lcd.print("Wrong Code!");
        tone(buzzerPin, 1500, 500);
        delay(1000);
      }
      inputPassword = ""; // Нулиране на въведената парола
      lcd.clear();
      lcd.print("Enter Code...");
    } else {
      inputPassword += key;
      lcd.setCursor(11, 1);
      lcd.print("*");
    }
  }

  // АКТИВИРАНА СИСТЕМА -> ПРОВЕРКА ЗА ДВИЖЕНИЕ
  if (systemArmed) {
    int motion = digitalRead(pirPin);
    if (motion == HIGH && !alarmActive) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Intruder Alert!");
      alarmActive = true;
      alarmStartTime = millis();
    }
  }

  // УПРАВЛЕНИЕ НА АЛАРМАТА
  if (alarmActive) {
    lcd.setCursor(0, 1);
    lcd.print("Enter Code:");
    unsigned long elapsedTime = millis() - alarmStartTime;

    if (elapsedTime < 10000) {
      if ((elapsedTime / 2000) % 2 == 0) {
        tone(buzzerPin, 1000);
      } else {
        noTone(buzzerPin);
      }
    } else {
      tone(buzzerPin, 1000);
    }
  }
}

// ФУНКЦИЯ ЗА АКТИВИРАНЕ НА СИСТЕМАТА
void activateSystem() {
  lcd.clear();
  lcd.print("System Arming...");
  tone(buzzerPin, 1000, 500);
  delay(5000); // 5 секунди за излизане
  systemArmed = true;
  lcd.clear();
  lcd.print("System Armed!");
  delay(1000);
  lcd.clear();
}

// ФУНКЦИЯ ЗА ДЕАКТИВИРАНЕ НА СИСТЕМАТА
void deactivateSystem() {
  lcd.clear();
  lcd.print("System Disarmed");
  Serial.println("System Disarmed");
  noTone(buzzerPin);
  systemArmed = false;
  alarmActive = false;
  delay(2000);
  lcd.clear();
}
