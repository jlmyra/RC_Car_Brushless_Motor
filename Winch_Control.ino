//********************************************************************************
//*** Winch Control Component ***
//*** Handles winch motor control via D-pad buttons ***
//********************************************************************************

void handleWinch() {
  
  //------------------------------------------------------------------------------
  // UP Button - Unwind Winch
  //------------------------------------------------------------------------------
  if (Ps3.event.analog_changed.button.up) {
    
    if (Ps3.data.analog.button.up > 0) {
      // Button is pressed - activate winch in unwind direction
      
      // Set direction (channel 1 off, channel 2 active)
      digitalWrite(winchPWMChannel_1, LOW);
      
      // Apply PWM speed control (analog button pressure 0-255)
      // Multiplied by 0.8 to limit maximum speed for control
      ledcWrite(winchPWMChannel_2, Ps3.data.analog.button.up * 0.8);
      
      Serial.print("Winch UNWIND: ");
      Serial.println(Ps3.data.analog.button.up);
      
    } else {
      // Button released - stop winch
      digitalWrite(winchPWMChannel_1, LOW);
      digitalWrite(winchPWMChannel_2, LOW);
    }
  }
  
  //------------------------------------------------------------------------------
  // DOWN Button - Rewind Winch
  //------------------------------------------------------------------------------
  if (Ps3.event.analog_changed.button.down) {
    
    if (Ps3.data.analog.button.down > 0) {
      // Button is pressed - activate winch in rewind direction
      
      // Set direction (channel 2 off, channel 1 active)
      digitalWrite(winchPWMChannel_2, LOW);
      
      // Apply PWM speed control (analog button pressure 0-255)
      // Multiplied by 0.8 to limit maximum speed for control
      ledcWrite(winchPWMChannel_1, Ps3.data.analog.button.down * 0.8);
      
      Serial.print("Winch REWIND: ");
      Serial.println(Ps3.data.analog.button.down);
      
    } else {
      // Button released - stop winch
      digitalWrite(winchPWMChannel_1, LOW);
      digitalWrite(winchPWMChannel_2, LOW);
    }
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
