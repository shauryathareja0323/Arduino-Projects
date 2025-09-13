#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Motor pins
#define IN1 4
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 32   // Motor A PWM
#define ENB 33   // Motor B PWM

#define SERVO_PIN 15   // Servo signal pin

Servo myServo;

// Wi-Fi credentials
const char* ssid = "ESP32-Car";
const char* password = "12345678";

// Web server
WebServer server(80);

// Motor speed (0–255)
int motorSpeed = 180;

// Track current state
String currentState = "stop";

// --- Motor speed control ---
void setSpeed(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// --- Motor functions ---
void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  setSpeed(motorSpeed);
  if (currentState != "forward") {
    currentState = "forward";
    Serial.println("State: Forward");
  }
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
  if (currentState != "backward") {
    currentState = "backward";
    Serial.println("State: Backward");
  }
}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  setSpeed(motorSpeed);
  if (currentState != "left") {
    currentState = "left";
    Serial.println("State: Left");
  }
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
  if (currentState != "right") {
    currentState = "right";
    Serial.println("State: Right");
  }
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  setSpeed(0);
  if (currentState != "stop") {
    currentState = "stop";
    Serial.println("State: Stop");
  }
}

// --- Servo functions ---
void handleServoLeft() {
  myServo.write(0);
  Serial.println("Servo at 0°");
  server.send(200, "text/plain", "Servo → 0°");
}

void handleServoRight() {
  myServo.write(70);
  Serial.println("Servo at 180°");
  server.send(200, "text/plain", "Servo → 180°");
}

// --- Webpage with joystick, speed slider, and servo buttons ---
void handleRoot() {
  String html = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>ESP32 RC Car + Servo</title>
<style>
  body { font-family: 'Segoe UI', sans-serif; margin: 0; padding: 20px; display: flex;
    flex-direction: column; align-items: center; gap: 25px;
    background: linear-gradient(135deg, #e0f7fa, #fce4ec); height: 100vh; }
  h2 { margin: 0; font-size: 1.6em; color: #333; }
  .joystick { width: 220px; height: 220px; position: relative; touch-action: none; user-select: none;
    border-radius: 50%; background: rgba(255,255,255,0.8);
    box-shadow: inset 4px 4px 12px #aaa, inset -4px -4px 12px #fff;
    display: flex; align-items: center; justify-content: center; }
  .knob { width: 90px; height: 90px; border-radius: 50%; background: #4cafef;
    box-shadow: 0 6px 15px rgba(0,0,0,0.2); position: absolute;
    left: 50%; top: 50%; transform: translate(-50%,-50%);
    transition: transform 0.1s ease-out; }
  #statusBox { background: rgba(255,255,255,0.85); border-radius: 12px; padding: 12px; width: 260px;
    text-align: center; font-family: monospace; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }
  #speedBox { text-align: center; width: 260px; }
  input[type=range] { width: 100%; height: 8px; border-radius: 5px; background: #ddd; outline: none; }
  input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 24px; height: 24px; border-radius: 50%;
    background: #4cafef; cursor: pointer; box-shadow: 0 3px 6px rgba(0,0,0,0.3); }
  #speedVal { font-weight: bold; font-size: 1.2em; margin-left: 8px; color: #4cafef; }
  .btn { padding: 12px 25px; font-size: 1em; border:none; border-radius:8px; cursor:pointer;
    background:#4cafef; color:white; box-shadow:0 4px 8px rgba(0,0,0,0.2); }
  .btn:active { background:#2196f3; }
</style>
</head>
<body>
  <h2>🚗 ESP32 RC Car + Servo</h2>

  <div id="joy" class="joystick">
    <div id="knob" class="knob"></div>
  </div>

  <div id="statusBox">X: 0.00, Y: 0.00 → stop</div>

  <div id="speedBox">
    <label for="speedSlider">Speed</label><br>
    <input type="range" id="speedSlider" min="0" max="255" step="5" value="180">
    <span id="speedVal">180</span>
  </div>

  <h3>Servo Control</h3>
  <div style="display:flex; gap:20px;">
    <button class="btn" onclick="fetch('/servoLeft')">◀ Left (0°)</button>
    <button class="btn" onclick="fetch('/servoRight')">Right (180°) ▶</button>
  </div>

<script>
  const joy=document.getElementById('joy');
  const knob=document.getElementById('knob');
  const statusBox=document.getElementById('statusBox');
  const slider=document.getElementById('speedSlider');
  const speedVal=document.getElementById('speedVal');

  let rect, radius, active=false, center={x:0,y:0};

  function recalc(){rect=joy.getBoundingClientRect();
    radius=Math.min(rect.width,rect.height)/2-(knob.offsetWidth/2);
    center={x:rect.left+rect.width/2,y:rect.top+rect.height/2};}
  window.addEventListener('resize',recalc);recalc();

  function setKnob(dx,dy){
    knob.style.transform=`translate(calc(-50% + ${dx}px), calc(-50% + ${dy}px))`;
  }

  function sendCommand(x,y){
    let cmd="stop";
    const dead=0.5;
    if(Math.abs(x)<dead && Math.abs(y)<dead) cmd="stop";
    else if(y>dead) cmd="forward";
    else if(y<-dead) cmd="backward";
    else if(x>dead) cmd="right";
    else if(x<-dead) cmd="left";
    fetch(`/${cmd}`).catch(()=>{});
    statusBox.textContent=`X: ${x.toFixed(2)}, Y: ${y.toFixed(2)} → ${cmd}`;
  }

  function pointerDown(e){active=true;joy.setPointerCapture(e.pointerId);pointerMove(e);}
  function pointerMove(e){
    if(!active) return;
    let dx=e.clientX-center.x, dy=-(e.clientY-center.y);
    const dist=Math.sqrt(dx*dx+dy*dy);
    if(dist>radius){const s=radius/dist;dx*=s;dy*=s;}
    setKnob(dx,-dy);
    sendCommand(dx/radius, dy/radius);
  }
  function pointerUp(e){active=false;setKnob(0,0);sendCommand(0,0);}
  joy.addEventListener('pointerdown',pointerDown);
  joy.addEventListener('pointermove',pointerMove);
  joy.addEventListener('pointerup',pointerUp);
  joy.addEventListener('pointercancel',pointerUp);

  // Speed control
  slider.addEventListener('input',()=>{
    speedVal.textContent=slider.value;
    fetch(`/speed?val=${slider.value}`).catch(()=>{});
  });
</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

// --- Handle speed update ---
void handleSpeed() {
  if (server.hasArg("val")) {
    motorSpeed = server.arg("val").toInt();
    motorSpeed = constrain(motorSpeed, 0, 255);
    Serial.print("Speed updated: ");
    Serial.println(motorSpeed);
    setSpeed(motorSpeed);
    server.send(200, "text/plain", "Speed set to " + String(motorSpeed));
  } else {
    server.send(400, "text/plain", "Missing speed value");
  }
}

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);


  myServo.attach(SERVO_PIN);

  // Motor direction pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar();

  // WiFi AP mode
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Access Point started");
  Serial.print("Connect to WiFi: ");
  Serial.println(ssid);

  // Routes
  server.on("/", handleRoot);
  server.on("/forward", []() { forward(); server.send(200, "text/plain", "Forward"); });
  server.on("/backward", []() { backward(); server.send(200, "text/plain", "Backward"); });
  server.on("/left", []() { left(); server.send(200, "text/plain", "Left"); });
  server.on("/right", []() { right(); server.send(200, "text/plain", "Right"); });
  server.on("/stop", []() { stopCar(); server.send(200, "text/plain", "Stop"); });
  server.on("/speed", handleSpeed);

  // Servo routes
  server.on("/servoLeft", handleServoLeft);
  server.on("/servoRight", handleServoRight);

  server.begin();
}

void loop() {
  server.handleClient();
}
