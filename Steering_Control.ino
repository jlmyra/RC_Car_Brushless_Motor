//********************************************************************************
//*** Steering Control Component ***
//*** Handles servo steering from right joystick ***
//********************************************************************************

void handleSteering() {

  // Skip if no controller connected
  if (!myController || !myController->isConnected()) {
    return;
  }

  // Read right stick X position from Bluepad32
  // Note: Bluepad32 axis range is -511 to 512, we map to PS3 range -128 to 128
  int x_position = map(myController->axisRX(), -512, 512, -128, 128);

  // Track last position to detect changes
  static int lastXPosition = 0;
  int stickChange = abs(x_position - lastXPosition);

  if (stickChange > 2) {  // Dead zone of 2 to avoid jitter
    lastXPosition = x_position;

    // Invert direction if servo is mounted backwards
    if (steerInverted) {
      x_position = -x_position;
    }

    // Map joystick range (-128 to +128) to servo PWM range
    // Left stick (-128) → steerMin
    // Center (0) → centered position
    // Right stick (+128) → steerMax
    steerJoystickPos = map(x_position, -128, 128, steerMin, steerMax);
    
    // Write PWM signal to steering servo
    ledcWrite(steerChannel, steerJoystickPos);
    
    // Debug output
    Serial.print("Steering: ");
    Serial.print(x_position);
    Serial.print(" → PWM: ");
    Serial.println(steerJoystickPos);
  }
}

//********************************************************************************
//*** Steering Calibration Notes ***
//********************************************************************************
/*
 * Steering Calibration:
 * 
 * The steering servo needs calibration so that:
 * - Joystick centered (0) = Wheels straight ahead
 * - Joystick full left (-128) = Maximum left turn
 * - Joystick full right (+128) = Maximum right turn
 * 
 * How to Calibrate:
 * 
 * 1. Power on vehicle
 * 2. Center the joystick
 * 3. Check if wheels point straight ahead
 * 4. If NOT straight:
 *    a. Move joystick full left - note wheel position
 *    b. Move joystick full right - note wheel position
 *    c. Adjust steerMin and steerMax in Model_Variables.h
 * 
 * Typical PWM Values (16-bit, 50Hz):
 * - 1000μs = 3277 ticks
 * - 1500μs = 4915 ticks (usually center)
 * - 2000μs = 6554 ticks
 * 
 * Example Adjustments:
 * 
 * If wheels turn left when centered:
 * - Increase steerMin slightly
 * - Or decrease steerMax slightly
 * 
 * If wheels turn right when centered:
 * - Decrease steerMin slightly
 * - Or increase steerMax slightly
 *
 * If steering is inverted (left goes right):
 * - Set .inverted = true in the vehicle's steering configuration
 * - This negates the joystick input before mapping to PWM
 * - Much cleaner than swapping min/max values
 *
 * Current Settings (from Model_Variables.h):
 * - steerMin = Vehicle-specific (left turn)
 * - steerMid = 4915 (center)
 * - steerMax = Vehicle-specific (right turn)
 * - steerInverted = Vehicle-specific (true for backwards-mounted servos)
 *
 * The dead zone of 2 prevents jitter from small joystick movements
 * when the stick is near center. Increase this value if you experience
 * twitchy steering when the joystick is barely touched.
 */
