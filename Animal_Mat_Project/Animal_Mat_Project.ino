#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

const int sensorPins[] = {4, 5, 6};
const int numSensors = 3;
// Create the Player object
DFRobotDFPlayerMini player;

int s1=0;
int s2=0;
int s3=0;
//int s4=0;
//int s5=0;
//int s6=0;

void setup() {

  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);

  //pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT); // Set the sensor pins as inputs
  }
  Serial.begin(9600);
  softwareSerial.begin(9600); // Initialize the serial communication
}

void play_story(){
  if (player.begin(softwareSerial)) {
   Serial.println("4");
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(1);
    delay(2000);
    player.pause();
   }else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop(){
  //player.disableLoopAll();
  int sensorValues[numSensors];
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = digitalRead(sensorPins[i]);
  }

  if (sensorValues[0] == HIGH && s1==0) {
    if (player.begin(softwareSerial)) {
   Serial.println("1");
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(1);
    delay(2000);
    player.pause();
    s1=1;
    //player.disableLoop();
    } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
    }
  } else if (sensorValues[1] == HIGH && s2==0) {
    if (player.begin(softwareSerial)) {
   Serial.println("2");
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(2);
    delay(2000);
    player.pause();
    s2=1;
    //player.disableLoop();
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  } else if (sensorValues[2] == HIGH && s3==0) {
    if (player.begin(softwareSerial)) {
   Serial.println("3");
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(3);
    delay(2000);
    player.pause();
    s3=1;
    //player.disableLoop();
  }
  }
 
  if(sensorValues[0]==HIGH){
    digitalWrite(8,LOW);
  }
  else{
    s1=0;
    digitalWrite(8,HIGH);
  }
  if(sensorValues[1]==HIGH){
    digitalWrite(9,LOW);
  }
  else{
    s2=0;
    digitalWrite(9,HIGH);
  }
  if(sensorValues[2]==HIGH){
    digitalWrite(10,LOW);
  }
  else{
    s3=0;
    digitalWrite(10,HIGH);
  }
  while(s1==1 && s2==1 && s3==1){
    if (player.begin(softwareSerial)) {
   Serial.println("4");
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(4);
    delay(33000);
    player.pause();
   }else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  }
  
}  

