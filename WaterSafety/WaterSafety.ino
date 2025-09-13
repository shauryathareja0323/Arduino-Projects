#include "RGBmatrixPanel.h"
#include "bit_bmp.h"
#include "fonts.h"
#include <string.h>
#include <stdlib.h>
#include <Fonts/TomThumb.h>




// --- Matrix wiring (adjust if needed) ---
#define CLK 11 // Use this on Arduino Mega
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define E   A4

RGBmatrixPanel matrix(A, B, C, D, E, CLK, LAT, OE, false, 64);

// --- UART and data variables ---
String inputString = "";
float phValue   = 0.0;
float tdsValue  = 0.0;
int turbidity   = 0;
bool newData    = false;

void setup() {
  Serial.begin(9600);    // Debug to PC
  Serial1.begin(9600);   // Receiving from UNO
  matrix.begin();
  delay(500);
  screen_clear();
}

void loop() {
  // --- Receive data from UNO ---
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {           // End of message
      parseData(inputString);  // Parse values
      newData = true;
      inputString = "";
    } else {
      inputString += c;
    }
  }

  // --- Update LED panel when new data arrives ---
  if (newData) {
    screen_clear();
    showData();
    newData = false;
  }
}

void parseData(String data) {
  // Example: "ph=6.85,tds=312,turb=42"
  int phIndex   = data.indexOf("ph=") + 3;
  int tdsIndex  = data.indexOf("tds=") + 4;
  int turbIndex = data.indexOf("turb=") + 5;

  String phStr   = data.substring(phIndex, data.indexOf(",", phIndex));
  String tdsStr  = data.substring(tdsIndex, data.indexOf(",", tdsIndex));
  String turbStr = data.substring(turbIndex);

  phValue   = phStr.toFloat();
  tdsValue  = tdsStr.toFloat();
  turbidity = turbStr.toInt();

  // Debug print to Serial Monitor
  Serial.print("ph = "); Serial.print(phValue, 2);
  Serial.print(" | tds = "); Serial.print(tdsValue, 0);
  Serial.print(" | turb = "); Serial.println(turbidity);
}

void screen_clear() {
  matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                  matrix.Color333(0, 0, 0));
}

void showData() {
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  //matrix.setFont();
  //matrix.setFont(&TomThumb);

  // matrix.setCursor(9, 2);    
  // char *str = "UPES";
  // for (uint8_t w=0; w<4; w++) {
  //     matrix.setTextColor(Wheel(w));
  //     matrix.print(str[w]);
  // }

  //matrix.setTextSize(2);

  // --- Show TDS ---
  matrix.setCursor(2, 23);
  matrix.setTextColor(matrix.Color333(7, 7, 7));
  matrix.print("TDS:");
  matrix.print(tdsValue, 0);

  // --- Show pH ---
  matrix.setCursor(2, 13);
  matrix.print("PH:");
  matrix.print(phValue, 2);

  // --- Show Turbidity ---
  matrix.setCursor(2, 33);
  matrix.print("Turb:");
  matrix.print(turbidity);
  matrix.print("%");

  // --- Status ---
  matrix.setCursor(12, 47);
  if (phValue >= 6.5 && phValue <= 8.5 && tdsValue < 500 && turbidity < 15) {
    matrix.setCursor(6, 47);
    matrix.setTextColor(Wheel(4));
    matrix.setTextSize(2);
    matrix.print("safe");
  } else {
    matrix.setCursor(15, 47);
    matrix.setTextColor(Wheel(0));
    matrix.setTextSize(1);
    //matrix.setFont(&TomThumb);
    matrix.print("UNSAFE");
  }
}

// Helper for colors
uint16_t Wheel(byte WheelPos) {
  if (WheelPos < 8) {
    return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if (WheelPos < 16) {
    WheelPos -= 8;
    return matrix.Color333(0, 7 - WheelPos, WheelPos);
  } else {
    WheelPos -= 16;
    return matrix.Color333(WheelPos, 0, 7 - WheelPos);
  }
}
