#include <WiFi.h>
#include <WebServer.h>

// Variables for setting up the web server for remote visualization of the amount of cycles done
const char* ssid = "Ciclo";
const char* password = "itt";
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

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

  // Setting up the web server
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
}

void loop() {
  // Code for the manual control mode when the "swAutoManual" is set to HIGH
  while (digitalRead(swAutoManual) == HIGH) {
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
    // Code for the automatic mode when the swAutoManual reads LOW
    do{
    int aux = 0;
    while (count < maxPerCycle) {
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
      cyclesCompleted += 1;
      Serial.println(cyclesCompleted);
    }
  }while (digitalRead(swAutoManual) == LOW);
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Número de ciclos: </title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  ptr +="<p>" + String(count) + "</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  
  ptr +="<p>" + String(cyclesCompleted) + "</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

  return ptr;
}
