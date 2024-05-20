#include <subs.h>
#include <yt1.h>
#include <yt2.h>

#include <ESP32WiFi.h>
 
const char* ssid = "GROWLER";
const char* password = "GROWLER03";
 
int ledPin = D3; // GPIO13---D7 of NodeMCU
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(D5,OUTPUT);  //MOTOR1 PIN1 IS CONNECTED TO PIN 8 OF ARDUINO
  pinMode(D6,OUTPUT);  //MOTOR1 PIN1 IS CONNECTED TO PIN 9 OF ARDUINO
  pinMode(D7,OUTPUT); //MOTOR2 PIN1 IS CONNECTED TO PIN 10 OF ARDUINO
  pinMode(D8,OUTPUT); //MOTOR2 PIN2 IS CONNECTED TO PIN 11 OF ARDUINO
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(ledPin,HIGH);
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
 


  int value = LOW;
  if (request.indexOf("/FORWARD") != -1)  {
    digitalWrite(D5,LOW);
    digitalWrite(D6,HIGH);
    digitalWrite(D7,HIGH);
    digitalWrite(D8,LOW);
    value = HIGH;
  }
  if (request.indexOf("/BACKWARD") != -1)  {
    digitalWrite(D5,HIGH);
    digitalWrite(D6,LOW);
    digitalWrite(D7,LOW);
    digitalWrite(D8,HIGH);
    value = LOW;
  }
  if (request.indexOf("/RIGHT") != -1)  {
    digitalWrite(D5,LOW);
    digitalWrite(D6,HIGH);
    digitalWrite(D7,LOW);
    digitalWrite(D8,LOW);
    value = LOW;
  }
  if (request.indexOf("/LEFT") != -1)  {
    digitalWrite(D5,LOW);
    digitalWrite(D6,LOW);
    digitalWrite(D7,HIGH);
    digitalWrite(D8,LOW);
    value = LOW;
  }
  if (request.indexOf("/STOP") != -1)  {
    digitalWrite(D5,LOW);
    digitalWrite(D6,LOW);
    digitalWrite(D7,LOW);
    digitalWrite(D8,LOW);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led is now: ");
 
  if(value == HIGH) {
    client.print("Forward");
  } 
  else if(value == LOW) {
    client.print("Stop");
  }

  
  client.println("<br><br>");
  client.println("<a href=\"/FORWARD\"\"><img forward_data ><button>Forward</button></a><br />");
  client.println("<br><br>");
  client.println("<a href=\"/BACKWARD\"\"><button>Backward</button></a><br />");
  client.println("<br><br>");
  client.println("<a href=\"/LEFT\"\"><button>Right</button></a><br />");
  client.println("<br><br>");
  client.println("<a href=\"/RIGHT\"\"><button>Left</button></a><br />");
  client.println("<br><br>");
  client.println("<a href=\"/STOP\"\"><button>Stop</button></a><br />");

  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
