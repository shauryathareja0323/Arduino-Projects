// X-axis pins
#define X_STEP_PIN 54
#define X_DIR_PIN  55
#define X_EN_PIN   38

// Y-axis pins
#define Y_STEP_PIN 60
#define Y_DIR_PIN  61
#define Y_EN_PIN   56

// Z-axis pins
#define Z_STEP_PIN 46
#define Z_DIR_PIN  48
#define Z_EN_PIN   62

void setup() {
  
  pinMode(X_STEP_PIN, OUTPUT); pinMode(X_DIR_PIN, OUTPUT); pinMode(X_EN_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT); pinMode(Y_DIR_PIN, OUTPUT); pinMode(Y_EN_PIN, OUTPUT);
  pinMode(Z_STEP_PIN, OUTPUT); pinMode(Z_DIR_PIN, OUTPUT); pinMode(Z_EN_PIN, OUTPUT);

  digitalWrite(X_EN_PIN, LOW);
  digitalWrite(Y_EN_PIN, LOW);
  digitalWrite(Z_EN_PIN, LOW);

  Serial.begin(9600);
  Serial.println("Enter commands: X <steps>, Y <steps>, Z <steps>");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() < 3) {
      Serial.println("Invalid command format.");
      return;
    }

    char axis = toupper(command.charAt(0));
    int steps = command.substring(2).toInt();

    switch (axis) {
      case 'X':
        moveAxis(X_STEP_PIN, X_DIR_PIN, steps);
        Serial.println("Moved X " + String(steps) + " steps");
        break;
      case 'Y':
        moveAxis(Y_STEP_PIN, Y_DIR_PIN, steps);
        Serial.println("Moved Y " + String(steps) + " steps");
        break;
      case 'Z':
        moveAxis(Z_STEP_PIN, Z_DIR_PIN, steps);
        Serial.println("Moved Z " + String(steps) + " steps");
        break;
      default:
        Serial.println("Invalid axis. Use X, Y, or Z.");
        break;
    }
  }
}

void moveAxis(int stepPin, int dirPin, int steps) {
  digitalWrite(dirPin, steps >= 0 ? HIGH : LOW);
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
}
