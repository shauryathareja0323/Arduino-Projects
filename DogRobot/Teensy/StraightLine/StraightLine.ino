// includes
#include <math.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ODriveArduino.h>
// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }


////////////////////////////////
// Set up serial pins to the ODrive
////////////////////////////////

// Below are some sample configurations.
// You can comment out the default Teensy one and uncomment the one you wish to use.
// You can of course use something different if you like
// Don't forget to also connect ODrive GND to Arduino GND.

// Teensy 3 and 4 (all versions) - Serial1
// pin 0: RX - connect to ODrive TX
// pin 1: TX - connect to ODrive RX
// See https://www.pjrc.com/teensy/td_uart.html for other options on Teensy
HardwareSerial& odrive_serial = Serial2;
// ODrive object
ODriveArduino odrive(odrive_serial);

const float L1 = 20.0;  // Hip to knee length (cm)
const float L2 = 20.0;  // Knee to foot length (cm)
const float ENC_STEP_DEG = 36.0; // 1 encoder step = 36°
const float GEAR_RATIO = 10.0;   // Gear ratio
const float HIP_NEUTRAL = -90.0; // Neutral angle

// Joint limits
const float HIP_MIN = -61.0;
const float HIP_MAX = 61.0;
const float KNEE_MIN = -91.0;
const float KNEE_MAX = 135.0;

// ----------------- Motion Parameters -----------------
float hip_target_turns = 0.0f;
float knee_target_turns = 0.0f;

float hip_curr_turns = 0.0f;
float knee_curr_turns = 0.0f;

// float max_speed = 0.002;   // max turns per update (safety)
unsigned long lastUpdate = 0;
// const int updateInterval = 0; // 100 Hz update

// Oscillation state
static float y = -20.0;       // starting Y position
static bool goingDown = true; // direction flag

void setup() {
  // ODrive uses 115200 baud
  odrive_serial.begin(115200);

  // Serial to PC
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor to open

  Serial.println("ODriveArduino");
  Serial.println("Setting parameters...");

  // In this example we set the same parameters to both motors.
  // You can of course set them different if you want.
  // See the documentation or play around in odrivetool to see the available parameters
  for (int axis = 0; axis < 2; ++axis) {
    odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 6000.0f << '\n';
    odrive_serial << "w axis" << axis << ".motor.config.current_lim " << 20.0f << '\n';
    // This ends up writing something like "w axis0.motor.config.current_lim 10.0\n"
  }

  int requested_state;

  requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  //Serial << "Axis" << 0 << ": Requesting state " << requested_state << '\n';
  if(!odrive.run_state(0, requested_state, false /*don't wait*/)) return;
  delay(50);
  //Serial << "Axis" << 1 << ": Requesting state " << requested_state << '\n';
  if(!odrive.run_state(1, requested_state, false /*don't wait*/)) return;
  delay(50);

  float posGainKnee = 60.0;
  float posGainHips = 100.0;  
  float velGain = 1;      
  float integrator = 2;  

  odrive_serial << "w axis" << 0 << ".controller.config.pos_gain " << posGainHips << '\n';
  odrive_serial << "w axis" << 1 << ".controller.config.pos_gain " << posGainKnee << '\n';

  odrive_serial << "w axis" << 0 << ".controller.config.vel_gain " << velGain << '\n';
  odrive_serial << "w axis" << 1 << ".controller.config.vel_gain " << velGain << '\n';

  odrive_serial << "w axis" << 0 << ".controller.config.vel_integrator_gain " << integrator << '\n';
  odrive_serial << "w axis" << 1 << ".controller.config.vel_integrator_gain " << integrator << '\n';


}

void loop(){
  unsigned long now = millis();

  // 1. Smooth linear oscillation of Y
  
    if (goingDown) {
      y -= 0.16f;  // small step for smooth motion
      if (y <= -39.0f) goingDown = false;
    } 
    else {
      y += 0.16f;
      if (y >= -29.0f) goingDown = true;
    }
  

  float x = 0.0;  // fixed X

  // 2. Calculate IK for (x, y)
  float hip_angle_rel, knee_angle_rel;
  if (inverseKinematics(x, y, hip_angle_rel, knee_angle_rel)) {
    hip_target_turns = (hip_angle_rel / ENC_STEP_DEG);
    knee_target_turns = (knee_angle_rel / ENC_STEP_DEG);
  }

  // 3. Update motors at fixed rate

  // Smooth speed-limited motion
  // hip_curr_turns = moveTowards(hip_curr_turns, hip_target_turns, max_speed);
  // knee_curr_turns = moveTowards(knee_curr_turns, -knee_target_turns, max_speed);

  odrive.SetPosition(0, hip_target_turns);
  odrive.SetPosition(1, -knee_target_turns);
  delay(5);

  //Debug output
  Serial.print("y=");
  Serial.print(y, 3);
  Serial.print("  hip=");
  Serial.print(hip_target_turns);
  Serial.print("  knee=");
  Serial.println(knee_target_turns);
  Serial.print("Time =");
  Serial.println(millis()-now);
  
}

// float moveTowards(float current, float target, float maxStep) {
//   if (fabs(target - current) <= maxStep) return target;
//   return (target > current) ? current + maxStep : current - maxStep;
// }

// ----------------- IK Function -----------------
bool inverseKinematics(float x, float y, float &hip_angle_rel, float &knee_angle_rel) {
  float d = sqrt(x * x + y * y);
  if (d > (L1 + L2) || d < fabs(L1 - L2)) return false;

  float cos_knee = (L1 * L1 + L2 * L2 - d * d) / (2 * L1 * L2);
  cos_knee = constrain(cos_knee, -1.0, 1.0);
  float knee_angle_rad = PI - acos(cos_knee);
  knee_angle_rel = knee_angle_rad * 180.0 / PI;

  float cos_hip_offset = (d * d + L1 * L1 - L2 * L2) / (2 * L1 * d);
  cos_hip_offset = constrain(cos_hip_offset, -1.0, 1.0);
  float hip_offset = acos(cos_hip_offset);
  float base_angle = atan2(y, x);
  float hip_angle_abs = base_angle - hip_offset;
  float hip_angle_deg = hip_angle_abs * 180.0 / PI;

  hip_angle_rel = hip_angle_deg - HIP_NEUTRAL;

  if (hip_angle_rel < HIP_MIN || hip_angle_rel > HIP_MAX) return false;
  if (knee_angle_rel < KNEE_MIN || knee_angle_rel > KNEE_MAX) return false;

  return true;
}
