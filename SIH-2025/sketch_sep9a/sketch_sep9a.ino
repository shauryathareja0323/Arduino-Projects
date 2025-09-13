#include <WiFi.h>
#include <WebServer.h>

// Motor pins
#define IN1 4
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 32   // Enable pin for Motor A
#define ENB 33   // Enable pin for Motor B

// Wi-Fi credentials
const char* ssid = "ESP32-Car";
const char* password = "12345678";

// Web server
WebServer server(80);

// Default motor speed (0–255)
int motorSpeed = 180;

// --- Helper function to print states ---
void printPinStates(const char* action) {
  Serial.print(action);
  Serial.print(" | IN1: "); Serial.print(digitalRead(IN1));
  Serial.print(" IN2: ");   Serial.print(digitalRead(IN2));
  Serial.print(" IN3: ");   Serial.print(digitalRead(IN3));
  Serial.print(" IN4: ");   Serial.print(digitalRead(IN4));
  Serial.print(" Speed: "); Serial.println(motorSpeed);
}

// --- Motor speed control ---
void setSpeed(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// --- Motor functions ---
void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
  printPinStates("Forward");
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  setSpeed(motorSpeed);
  printPinStates("Backward");
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  setSpeed(motorSpeed);
  printPinStates("Left");
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
  printPinStates("Right");
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  setSpeed(0);
  printPinStates("Stop");
}

// --- Webpage ---
void handleRoot() {
  String html = "<html><head><title>ESP32 RC Car</title></head><body>";
  html += "<h1>ESP32 RC Car</h1>";
  html += "<p>Speed: " + String(motorSpeed) + "</p>";
  html += "<a href='/forward'><button>Forward</button></a><br>";
  html += "<a href='/backward'><button>Backward</button></a><br>";
  html += "<a href='/left'><button>Left</button></a><br>";
  html += "<a href='/right'><button>Right</button></a><br>";
  html += "<a href='/stop'><button>Stop</button></a><br><br>";
  html += "<a href='/faster'><button>Speed +</button></a>";
  html += "<a href='/slower'><button>Speed -</button></a><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Motor direction pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Motor enable pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopCar();

  // WiFi AP mode
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Access Point started");
  Serial.print("Connect to WiFi: ");
  Serial.println(ssid);

  // Routes
  server.on("/", handleRoot);
  server.on("/forward", []() { forward(); handleRoot(); });
  server.on("/backward", []() { backward(); handleRoot(); });
  server.on("/left", []() { left(); handleRoot(); });
  server.on("/right", []() { right(); handleRoot(); });
  server.on("/stop", []() { stopCar(); handleRoot(); });
  server.on("/faster", []() { motorSpeed = min(255, motorSpeed + 5); handleRoot(); });
  server.on("/slower", []() { motorSpeed = max(0, motorSpeed - 5); handleRoot(); });

  server.begin();
}

void loop() {
  server.handleClient();
}
