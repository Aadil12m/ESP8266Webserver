#include <ESP8266WiFi.h>
#include <Servo.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

WiFiServer server(80);

Servo myServo;

// Motor Pins
const int ENA = D3;
const int IN1 = D1;
const int IN2 = D2;

const int ENB = D4;
const int IN3 = D5;
const int IN4 = D6;

// Servo pin
const int SERVO_PIN = D7;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Parse query string like /?motorA=200&motorB=150&servo=90
    int mA = getValue(request, "motorA", 0);
    int mB = getValue(request, "motorB", 0);
    int sAngle = getValue(request, "servo", 90);

    // Motor A forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, mA);

    // Motor B forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, mB);

    // Servo control
    myServo.write(sAngle);

    // Send HTML Page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println(buildHTML(mA, mB, sAngle));
    client.stop();
  }
}

int getValue(String req, String key, int defaultVal) {
  int i = req.indexOf(key + "=");
  if (i == -1) return defaultVal;
  int j = req.indexOf("&", i);
  String val = req.substring(i + key.length() + 1, j == -1 ? req.length() : j);
  return val.toInt();
}

String buildHTML(int mA, int mB, int s) {
  return "<!DOCTYPE html><html><body><h2>Motor & Servo Control</h2>"
         "<form action='/' method='get'>"
         "Motor A Speed (0-1023): <input type='range' name='motorA' min='0' max='1023' value='" + String(mA) + "' onchange='this.form.submit()'><br>"
         "Motor B Speed (0-1023): <input type='range' name='motorB' min='0' max='1023' value='" + String(mB) + "' onchange='this.form.submit()'><br>"
         "Servo Angle (0-180): <input type='range' name='servo' min='0' max='180' value='" + String(s) + "' onchange='this.form.submit()'><br>"
         "</form></body></html>";
}
