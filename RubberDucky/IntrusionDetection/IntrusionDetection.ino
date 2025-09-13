#include "DigiKeyboard.h"

void setup() {
  DigiKeyboard.sendKeyStroke(0); // Init

  DigiKeyboard.delay(500);

  // Open Run dialog
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.delay(800);

  // Launch PowerShell silently
  DigiKeyboard.print("powershell");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(1500);

  // Create log folder
  DigiKeyboard.print("mkdir C:\\Temp");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // Dump system event log
  DigiKeyboard.print("Get-EventLog System -Newest 5000 | Out-File C:\\Temp\\eventlog.txt");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(3000);

  // Optional: dump application lpowershellog
  // DigiKeyboard.print("Get-EventLog Application -Newest 50 | Out-File -Append C:\\Temp\\eventlog.txt");
  // DigiKeyboard.sendKeyStroke(KEY_ENTER);

  // Done
  DigiKeyboard.delay(1000);
}

void loop() {
  // Do nothing
}
