#include <DigiKeyboard.h>

void setup() {
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.sendKeyStroke(KEY_N, MOD_CONTROL_LEFT);
  delay(2000);
  DigiKeyboard.print("Notepad");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  delay(3000);
  DigiKeyboard.print("Digispark is Alive");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
}

void loop() {
 
}
