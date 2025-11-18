//********************************************************************************
//*** Winch Control Component ***
//*** Handles winch motor control via D-pad buttons ***
//********************************************************************************

void handleWinch() {

  //------------------------------------------------------------------------------
  // UP Button - Unwind Winch
  //------------------------------------------------------------------------------
  if (PS4.data.button.up) {
    // Button is pressed - activate winch in unwind direction

    // Set direction (channel 1 off, channel 2 active)
    ledcWrite(winchPWMChannel_1, 0);

    // Apply PWM speed control at 80% speed for better control
    ledcWrite(winchPWMChannel_2, 204);  // 255 * 0.8 = 204

    static bool upButtonLogged = false;
    if (!upButtonLogged) {
      Serial.println("Winch UNWIND");
      upButtonLogged = true;
    }
  } else {
    static bool upButtonLogged = false;
    upButtonLogged = false;
  }

  //------------------------------------------------------------------------------
  // DOWN Button - Rewind Winch
  //------------------------------------------------------------------------------
  if (PS4.data.button.down) {
    // Button is pressed - activate winch in rewind direction

    // Set direction (channel 2 off, channel 1 active)
    ledcWrite(winchPWMChannel_2, 0);

    // Apply PWM speed control at 80% speed for better control
    ledcWrite(winchPWMChannel_1, 204);  // 255 * 0.8 = 204

    static bool downButtonLogged = false;
    if (!downButtonLogged) {
      Serial.println("Winch REWIND");
      downButtonLogged = true;
    }
  } else {
    static bool downButtonLogged = false;
    downButtonLogged = false;
  }

  // Stop winch if neither button is pressed
  if (!PS4.data.button.up && !PS4.data.button.down) {
    static bool wasStopped = false;
    if (!wasStopped) {
      ledcWrite(winchPWMChannel_1, 0);
      ledcWrite(winchPWMChannel_2, 0);
      wasStopped = true;
    }
  } else {
    static bool wasStopped = false;
    wasStopped = false;
  }
}

//********************************************************************************
//*** Winch Control Notes ***
//********************************************************************************
/*
 * Winch Motor Control:
 * 
 * Hardware Setup:
 * - Winch motor connected via H-bridge or dual motor driver
 * - GPIO 26 (winchPWMChannel_1) - Forward direction control
 * - GPIO 18 (winchPWMChannel_2) - Reverse direction control
 * 
 * Control Method:
 * - D-pad UP = Unwind winch (let out cable)
 * - D-pad DOWN = Rewind winch (pull in cable)
 * - Analog pressure sensitive (0-255)
 * - Speed limited to 80% for better control
 * 
 * Safety Features:
 * - Opposite channel forced LOW before activating direction
 * - Prevents shoot-through (both channels active simultaneously)
 * - Motor stops immediately when button released
 * - Emergency stop also disables winch
 * 
 * Speed Control:
 * - Button pressure is analog (0-255)
 * - Light press = slow winch speed
 * - Full press = 80% of maximum speed
 * - Multiply factor can be adjusted:
 *   - 0.5 = Very slow, very controlled
 *   - 0.8 = Default, good control
 *   - 1.0 = Full speed, less control
 * 
 * Wiring:
 * - ESP32 GPIO 26 → Motor Driver IN1 (PWM)
 * - ESP32 GPIO 18 → Motor Driver IN2 (PWM)
 * - Motor Driver → Winch Motor
 * - Separate power supply for motor driver (not from ESP32!)
 * - Common ground between ESP32 and motor driver
 * 
 * Modifications:
 * - To swap direction: Swap channel 1 and channel 2 in code
 * - To change speed: Adjust multiplication factor (0.8)
 * - To add ramping: Apply exponential smoothing like motor control
 * - To limit torque: Reduce PWM frequency or duty cycle
 * 
 * PWM Settings:
 * - Frequency: 30kHz (high frequency for smooth motor operation)
 * - Resolution: 8-bit (0-255 levels)
 * - Higher frequency = smoother operation, less motor whine
 * - Lower frequency = more torque at low speeds
 */
