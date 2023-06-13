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
int maxPerCycle = 5;
int cyclesCompleted = 0;

// Digital ports for the switches
const int swManualForward = 2;
const int swManualBackward = 3;
const int swAutoManual = 4;
const int swAutoStart = 5;

// Digital ports for the relays
const int relayFw = 6;
const int relayBw = 7;

// Digital ports for the magnetic sensors
const int sensorFront = 8;
const int sensorBack = 9;

// Digital port for the 12v buzzer used as an alarm
const int buzzerAlarm = 10;

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

  // LCD setup
  analogReference(INTERNAL);
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
        digitalWrite(relayBw, HIGH);
        digitalWrite(relayFw, LOW);
        lcd.clear();
        lcd.home();
        lcd.print("Modo manual  <-----");
      }

      else if (digitalRead(swManualBackward) == HIGH) {
        digitalWrite(relayBw, LOW);
        digitalWrite(relayFw, HIGH);
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
    do{
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
        digitalWrite(relayFw, LOW);
        digitalWrite(relayBw, HIGH);
        if (digitalRead(sensorFront) == HIGH) {
          aux = 1;
        }
      }
      if (aux == 1) {
        digitalWrite(relayFw, HIGH);
        digitalWrite(relayBw, LOW);
        if (digitalRead(sensorBack) == HIGH) {
          count += 1;
          aux = 0;
          Serial.println(count);
        }
      }
      if (digitalRead(swAutoManual) == HIGH) {
        break;
      }
    }
    
    digitalWrite(relayBw, HIGH);
    digitalWrite(relayFw, HIGH);
    tone(10, 2000);
    Serial.println("Tone");
    delay(100);
    noTone(10);
    Serial.println("noTone");
    delay(100);
    if(digitalRead(swManualForward) == HIGH){
      count = 0;
      cyclesCompleted += 1;
      Serial.println(cyclesCompleted);
    }
  }while (digitalRead(swAutoManual) == LOW);
}