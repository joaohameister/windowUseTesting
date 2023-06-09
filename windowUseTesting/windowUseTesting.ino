int count = 0;
int maxCount = 5;

const int swManualForward = 2;
const int swManualBackward = 3;

const int relayFw = 6;
const int relayBw = 7;

const int swAutoManual = 4;
const int swAutoStart = 5;

const int sensorFront = 8;
const int sensorBack = 9;

const int alarm = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(swManualForward, INPUT);
  pinMode(swManualBackward, INPUT);
  pinMode(swAutoManual, INPUT);
  pinMode(swAutoStart, INPUT);

  pinMode(sensorFront, INPUT);
  pinMode(sensorBack, INPUT);

  pinMode(relayFw, OUTPUT);
  pinMode(relayBw, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (digitalRead(swAutoManual) == HIGH) {
    digitalWrite(relayBw, HIGH);
    digitalWrite(relayFw, HIGH);
    if (digitalRead(sensorBack) == HIGH || digitalRead(sensorFront) == HIGH) {
      digitalWrite(relayBw, HIGH);
      digitalWrite(relayFw, HIGH);
    } else {
      if (digitalRead(swManualForward) == HIGH) {
        digitalWrite(relayBw, HIGH);
        digitalWrite(relayFw, LOW);
      }

      else if (digitalRead(swManualBackward) == HIGH) {
        digitalWrite(relayBw, LOW);
        digitalWrite(relayFw, HIGH);
      }

      else {
        digitalWrite(relayBw, HIGH);
        digitalWrite(relayFw, HIGH);
      }
    }
  }

    do{
    int aux = 0;
    while (count < maxCount) {
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
    if(digitalRead(swManualForward) == HIGH){
      count = 0;
    }
  }while (digitalRead(swAutoManual) == LOW);
}
