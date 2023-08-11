#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4);

byte custom[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
// Auxiliar variables to keep track of how many times it completed a cycle
int count = 0;
int maxPerCycle = 1000;
int cyclesCompleted = 0;

// Digital ports for the switches
const int swManualForward = 32;
const int swManualBackward = 33;
const int swAutoManual = 34;
const int swAutoStart = 35;

// Digital ports for the magnetic sensors
const int sensorFront = 26;
const int sensorBack = 27;

// Digital ports for the relays
const int relayFw = 18;
const int relayBw = 19;
// Digital port for the 12v buzzer used as an alarm
const int buzzerAlarm = 12;
// Safety pin
const int safety = 13;

void setup() {
  // Initiating all digital ports
  Serial.begin(9600);
  pinMode(swManualForward, INPUT);
  pinMode(swManualBackward, INPUT);
  pinMode(swAutoManual, INPUT);
  pinMode(swAutoStart, INPUT);

  pinMode(sensorFront, INPUT);
  pinMode(sensorBack, INPUT);

  pinMode(relayFw, OUTPUT);
  pinMode(relayBw, OUTPUT);

  pinMode(safety, INPUT);

    // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.createChar(5, custom);
}

void loop() {
  // Code for the manual control mode when the "swAutoManual" is set to HIGH
  while (digitalRead(swAutoManual) == HIGH) {
    lcd.print("Modo manual");
    digitalWrite(relayBw, HIGH);
    digitalWrite(relayFw, HIGH);

    // Read the manual control switches and checks if the magnetic sensors are reading HIGH before doing so
    if (digitalRead(sensorBack) == HIGH || digitalRead(sensorFront) == HIGH) {
      digitalWrite(relayBw, HIGH);
      digitalWrite(relayFw, HIGH);
    } else {
      if (digitalRead(swManualForward) == HIGH) {
        goForward();
        lcd.clear();
        lcd.home();
        lcd.print("Modo manual  <-----");
      }

      else if (digitalRead(swManualBackward) == HIGH) {
        goBackwards();
        lcd.clear();
        lcd.home();
        lcd.print("Modo manual  ----->");
      }

      else {
        digitalWrite(relayBw, HIGH);
        digitalWrite(relayFw, HIGH);
      }
    }
  }
  // Code for the automatic mode when the swAutoManual reads LOW
  do {
    int aux = 0;
    while (count < maxPerCycle) {
      lcd.clear();
      lcd.home();
      lcd.print("Modo de ensaio");
      lcd.setCursor(0, 2);
      lcd.print("Número de ciclos: " + count);
      lcd.setCursor(0, 3);
      lcd.print("Etapas concluídas: " + cyclesCompleted);

      if (aux == 0) {
        goForward();
        if (digitalRead(sensorFront) == HIGH) {
          aux = 1;
        }
      }
      if (aux == 1) {
        goBackwards();
        if (digitalRead(sensorBack) == HIGH) {
          count += 1;
          aux = 0;
          Serial.println(count);
        }
      }
      if (digitalRead(swAutoManual) == HIGH) {

        break;
      }

      while (digitalRead(safety) == LOW) {
        lcd.clear();
        lcd.home();
        lcd.print("SISTEMA DE ");
        lcd.setCursor(0, 2);
        lcd.print("SEGURANÇA");
        lcd.setCursor(0, 3);
        lcd.print("ACIONADO");
        ringTheAlarm();

      }
    }

    ringTheAlarm();
    if (digitalRead(swAutoStart) == HIGH) {
      count = 0;
      cyclesCompleted += 1;
      Serial.println(cyclesCompleted);
    }
  } while (digitalRead(swAutoManual) == LOW);
}

// Rings the alarm
void ringTheAlarm() {
  digitalWrite(relayBw, HIGH);
  digitalWrite(relayFw, HIGH);
  tone(10, 2000);
  Serial.println("Tone");
  delay(100);
  noTone(10);
  delay(100);
}

// Goes forward
void goForward() {
  digitalWrite(relayBw, HIGH);
  digitalWrite(relayFw, LOW);
}

// Goes backwards
void goBackwards() {
  digitalWrite(relayBw, LOW);
  digitalWrite(relayFw, HIGH);
}