//********************************************************************************
//*** Winch Control Component ***
//*** Handles winch motor control via D-pad buttons ***
//********************************************************************************

void handleWinch() {

  // Skip if no controller connected
  if (!myController || !myController->isConnected()) {
    return;
  }

  // Read D-pad state from Bluepad32
  // Bluepad32 D-pad values: 0x01=up, 0x02=down, 0x04=right, 0x08=left
  uint8_t dpad = myController->dpad();

  //------------------------------------------------------------------------------
  // UP Button - Unwind Winch
  //------------------------------------------------------------------------------
  if (dpad & 0x01) {  // D-pad UP pressed
    // Button is pressed - activate winch in unwind direction

    // Set direction (channel 1 off, channel 2 active)
    digitalWrite(winchPWMChannel_1, LOW);

    // Apply PWM speed control at 80% maximum speed
    // Note: Bluepad32 D-pad is digital (on/off), not analog pressure
    ledcWrite(winchPWMChannel_2, 255 * 0.8);

    Serial.println("Winch UNWIND");

  }
  //------------------------------------------------------------------------------
  // DOWN Button - Rewind Winch
  //------------------------------------------------------------------------------
  else if (dpad & 0x02) {  // D-pad DOWN pressed
    // Button is pressed - activate winch in rewind direction

    // Set direction (channel 2 off, channel 1 active)
    digitalWrite(winchPWMChannel_2, LOW);

    // Apply PWM speed control at 80% maximum speed
    ledcWrite(winchPWMChannel_1, 255 * 0.8);

    Serial.println("Winch REWIND");

  }
  else {
    // No D-pad button pressed - stop winch
    digitalWrite(winchPWMChannel_1, LOW);
    digitalWrite(winchPWMChannel_2, LOW);
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
 * - Digital on/off control (Bluepad32 D-pad is not pressure sensitive)
 * - Speed limited to 80% of maximum for better control
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
