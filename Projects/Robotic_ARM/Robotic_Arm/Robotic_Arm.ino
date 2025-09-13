#define EN_PIN   38
#define STEP_PIN 54
#define DIR_PIN  55

String input = "";
bool moveUp = true;
int steps = 0;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(EN_PIN, LOW);  // Enable motor driver

  Serial.begin(9600);
  Serial.println("Enter command: UP <steps> or DOWN <steps>");
}

void loop() {
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();  // Remove whitespace

    // Parse command
    if (input.startsWith("UP")) {
      moveUp = true;
      steps = input.substring(2).toInt();
      moveMotor(steps, moveUp);
    } else if (input.startsWith("DOWN")) {
      moveUp = false;
      steps = input.substring(4).toInt();
      moveMotor(steps, moveUp);
    } else {
      Serial.println("Invalid command. Use: UP <steps> or DOWN <steps>");
    }
  }
}

void moveMotor(int steps, bool direction) {
  digitalWrite(DIR_PIN, direction ? HIGH : LOW);

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(800);  // Speed control
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800);
  }

  Serial.print("Moved ");
  Serial.print(direction ? "UP" : "DOWN");
  Serial.print(" ");
  Serial.print(steps);
  Serial.println(" steps");
}
