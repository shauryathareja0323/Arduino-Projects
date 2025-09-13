#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2        // DHT sensor connected to pin 2
#define DHTTYPE DHT11   // or DHT22

#define SOIL_PIN A0     // Soil Moisture sensor to A0
#define PH_PIN A1       // pH Sensor to A1
#define RELAY_PIN 8     // Relay module to D8

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27 for 16x2 LCD

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Pump off initially
  lcd.setCursor(0,0);
  lcd.print("Hello, world!");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil = analogRead(SOIL_PIN);
  float pHValue = analogRead(PH_PIN) * (5.0 / 1023.0) * 3.5; // Adjust based on calibration

  // Map soil moisture to %
  int moisturePercent = map(soil, 1023, 300, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(moisturePercent);
  lcd.print("% T:");
  lcd.print((int)temperature);

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print((int)humidity);
  lcd.print("% pH:");
  lcd.print(pHValue, 1);

  // Irrigation logic
  if (moisturePercent < 40) {
    digitalWrite(RELAY_PIN, LOW); // Pump ON
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Pump OFF
  }

  delay(2000);
}
