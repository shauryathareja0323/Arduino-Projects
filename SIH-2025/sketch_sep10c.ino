#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>   // Library for servo

// Motor pins
#define IN1 4
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 32   // Motor A PWM
#define ENB 33   // Motor B PWM

// Servo pin
#define SERVO_PIN 15
Servo myServo;

// Wi-Fi credentials
const char* ssid = "ESP32-Car";
const char* password = "12345678";

// Web server
WebServer server(80);

// Motor speed
int motorSpeed = 180;

// Track state
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
}
void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
}
void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  setSpeed(motorSpeed);
}
void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(motorSpeed);
}
void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  setSpeed(0);
}

// --- Handle servo movement ---
void handleServo() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    angle = constrain(angle, 0, 180);
    myServo.write(angle);
    Serial.print("Servo angle: ");
    Serial.println(angle);
    server.send(200, "text/plain", "Servo set to " + String(angle));
  } else {
    server.send(400, "text/plain", "Missing angle value");
  }
}

// --- Webpage with 2 joysticks ---
void handleRoot() {
  String html = R"rawliteral(
<!doctype html>
<html>
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>ESP32 RC Car + Servo</title>
<style>
  body { font-family:sans-serif; display:flex; flex-direction:column; align-items:center; gap:20px; }
  .joystick {
    width:200px; height:200px; border-radius:50%; background:#eee; position:relative;
    touch-action:none; user-select:none; display:flex; justify-content:center; align-items:center;
  }
  .knob { width:80px; height:80px; border-radius:50%; background:#4cafef; position:absolute;
    left:50%; top:50%; transform:translate(-50%,-50%); }
</style>
</head>
<body>
  <h2>🚗 ESP32 RC Car + Servo</h2>

  <div>Car Joystick</div>
  <div id="joyCar" class="joystick"><div id="knobCar" class="knob"></div></div>
  
  <div>Servo Joystick</div>
  <div id="joyServo" class="joystick"><div id="knobServo" class="knob"></div></div>

<script>
function makeJoystick(joyId, knobId, callback){
  const joy=document.getElementById(joyId), knob=document.getElementById(knobId);
  let rect, radius, active=false, center={x:0,y:0};
  function recalc(){rect=joy.getBoundingClientRect();
    radius=Math.min(rect.width,rect.height)/2-(knob.offsetWidth/2);
    center={x:rect.left+rect.width/2,y:rect.top+rect.height/2};}
  window.addEventListener('resize',recalc);recalc();
  function setKnob(dx,dy){knob.style.transform=`translate(calc(-50% + ${dx}px),calc(-50% + ${dy}px))`;}
  function pointerDown(e){active=true;joy.setPointerCapture(e.pointerId);pointerMove(e);}
  function pointerMove(e){
    if(!active) return;
    let dx=e.clientX-center.x, dy=e.clientY-center.y;
    const dist=Math.sqrt(dx*dx+dy*dy);
    if(dist>radius){let s=radius/dist;dx*=s;dy*=s;}
    setKnob(dx,dy); callback(dx/radius, -dy/radius);
  }
  function pointerUp(e){active=false;setKnob(0,0);callback(0,0);}
  joy.addEventListener('pointerdown',pointerDown);
  joy.addEventListener('pointermove',pointerMove);
  joy.addEventListener('pointerup',pointerUp);
  joy.addEventListener('pointercancel',pointerUp);
}

makeJoystick("joyCar","knobCar",(x,y)=>{
  let cmd="stop", dead=0.5;
  if(Math.abs(x)<dead && Math.abs(y)<dead) cmd="stop";
  else if(y>dead) cmd="forward";
  else if(y<-dead) cmd="backward";
  else if(x>dead) cmd="right";
  else if(x<-dead) cmd="left";
  fetch(`/${cmd}`).catch(()=>{});
});

makeJoystick("joyServo","knobServo",(x,y)=>{
  let angle=Math.round((x+1)*90); // x -1→1 maps to 0–180
  fetch(`/servo?angle=${angle}`).catch(()=>{});
});
</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopCar();

  myServo.attach(SERVO_PIN);

  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");

  server.on("/", handleRoot);
  server.on("/forward", [](){ forward(); server.send(200,"text/plain","Forward"); });
  server.on("/backward", [](){ backward(); server.send(200,"text/plain","Backward"); });
  server.on("/left", [](){ left(); server.send(200,"text/plain","Left"); });
  server.on("/right", [](){ right(); server.send(200,"text/plain","Right"); });
  server.on("/stop", [](){ stopCar(); server.send(200,"text/plain","Stop"); });
  server.on("/servo", handleServo);

  server.begin();
}

void loop() {
  server.handleClient();
}
