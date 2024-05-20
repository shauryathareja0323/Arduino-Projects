//In this code I am using an IR Sensor with an L298n Motor Driver to create an obstacle avoider robot.

void setup() {
  // put your setup code here, to run once:
  pinMode(8,OUTPUT);  //MOTOR1 PIN1 IS CONNECTED TO PIN 8 OF ARDUINO
  pinMode(9,OUTPUT);  //MOTOR1 PIN1 IS CONNECTED TO PIN 9 OF ARDUINO
  pinMode(10,OUTPUT); //MOTOR2 PIN1 IS CONNECTED TO PIN 10 OF ARDUINO
  pinMode(11,OUTPUT); //MOTOR2 PIN2 IS CONNECTED TO PIN 11 OF ARDUINO
  pinMode(12,INPUT);  //IR SENSOR IS CONNECTED TO PIN 12 OF ARDUINO
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(12);
  // print out the state of the button:
  Serial.println(buttonState);
  delay(1);
  int s1 = digitalRead(12);
  if(s1 == LOW){
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);
    digitalWrite(10,HIGH);
    digitalWrite(11,LOW);
    }
  else{
    digitalWrite(8,LOW);
    digitalWrite(9,LOW);
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    delay(1000);
    digitalWrite(8,HIGH);
    digitalWrite(9,LOW);
    digitalWrite(10,HIGH);
    digitalWrite(11,LOW);
    delay(1000);
    digitalWrite(8,LOW);
    digitalWrite(9,LOW);
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    delay(1000);
  }
}