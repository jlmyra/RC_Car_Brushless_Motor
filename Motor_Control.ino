//********************************************************************************
//*** Motor Control Component ***
//*** Handles BLDC motor acceleration, deceleration, and turbo mode ***
//********************************************************************************

void BLDC_move() {

  // Fixed time-step updates for consistent behavior
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime < UPDATE_INTERVAL) {
    return; // Not time to update yet
  }
  lastUpdateTime = currentTime;

  // Read joystick input from PS4 controller
  leftJoystickPos = PS4.data.analog.stick.ly;
  bool turboMode = PS4.data.button.l1;
  
  // Determine target speed based on joystick position and mode
  if (leftJoystickPos <= -6) {
    // FORWARD MOTION
    if (turboMode) {
      // Turbo mode - full speed range
      bldcTargetSpeed = map(leftJoystickPos, -128, 0, bldcFwdMax, bldcStopSpeed);
    } else {
      // Normal mode - limited speed for better control
      bldcTargetSpeed = map(leftJoystickPos, -128, 0, bldcFwdMaxSlow, bldcStopSpeed);
    }
  } 
  else if (leftJoystickPos >= 6) {
    // REVERSE MOTION
    bldcTargetSpeed = map(leftJoystickPos, 0, 128, bldcStopSpeed, bldcRevMinSlow);
  } 
  else {
    // STOP ZONE (dead zone: -5 to +5)
    bldcTargetSpeed = bldcStopSpeed;
  }
  
  // Apply exponential smoothing to gradually reach target speed
  // This creates smooth acceleration and deceleration curves
  float smoothFactor;
  
  if (abs(bldcTargetSpeed - bldcSmoothSpeed) < 50) {
    // Close to target - slow down the approach to avoid oscillation
    smoothFactor = 0.05;
  } else if (bldcTargetSpeed < bldcSmoothSpeed) {
    // Decelerating - use vehicle-specific decel factor
    smoothFactor = DECEL_SMOOTH_FACTOR;
  } else {
    // Accelerating - use vehicle-specific accel factor
    smoothFactor = ACCEL_SMOOTH_FACTOR;
  }
  
  // Exponential smoothing formula: new = old + factor * (target - old)
  bldcSmoothSpeed += smoothFactor * (bldcTargetSpeed - bldcSmoothSpeed);
  
  // Convert to integer for PWM output
  int outputSpeed = (int)bldcSmoothSpeed;
  
  // Apply start boost for low speeds in forward direction
  // Helps overcome initial motor friction and get moving smoothly
  if ((leftJoystickPos <= -6) && (leftJoystickPos >= -15) && (outputSpeed > bldcStopSpeed)) {
    outputSpeed += bldcStartBoost;
  }
  
  // Safety limits - constrain to valid PWM range
  outputSpeed = constrain(outputSpeed, bldcRevMin, bldcFwdMax);
  
  // Write PWM signal to ESC
  ledcWrite(bldcChannel, outputSpeed);
  
  // Update current speed for next iteration
  bldcCurrentSpeed = outputSpeed;
  
  // Debug output - print motor status every second when moving
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 1000 && leftJoystickPos != 0) {
    lastDebug = millis();
    Serial.print("Motor: Joy=");
    Serial.print(leftJoystickPos);
    Serial.print(" Target=");
    Serial.print((int)bldcTargetSpeed);
    Serial.print(" Speed=");
    Serial.print(outputSpeed);
    if (turboMode) Serial.print(" [TURBO]");
    Serial.println();
  }
}

//********************************************************************************
//*** Motor Tuning Notes ***
//********************************************************************************
/*
 * The motor control uses exponential smoothing for natural feeling acceleration.
 * 
 * Key Parameters (set in Model_Variables.h per vehicle):
 * 
 * 1. ACCEL_SMOOTH_FACTOR (0.05 to 0.30)
 *    - Controls how quickly speed increases
 *    - Higher = faster acceleration, more aggressive
 *    - Lower = slower acceleration, smoother
 *    - Adjust based on vehicle weight and gearing
 * 
 * 2. DECEL_SMOOTH_FACTOR (0.05 to 0.20)
 *    - Controls how quickly speed decreases
 *    - Higher = faster stopping, more responsive
 *    - Lower = coasting deceleration, momentum-preserving
 *    - Adjust based on desired stopping behavior
 * 
 * 3. bldcStartBoost (0 to 100)
 *    - Extra power at low speeds to overcome friction
 *    - Higher = easier to get moving from stop
 *    - Lower = smoother start but may not move easily
 *    - Adjust based on motor/gear friction
 * 
 * Speed Ranges:
 * - bldcStopSpeed = 4915 (1500μs, neutral)
 * - bldcRevMin = 3441 (1000μs, full reverse)
 * - bldcFwdMax = 6554 (2000μs, full forward)
 * - bldcRevMinSlow = 4096 (1250μs, limited reverse)
 * - bldcFwdMaxSlow = 5898 (1800μs, limited forward for normal mode)
 * 
 * Turbo Mode:
 * - Activated by holding L1 button
 * - Uses full speed range (bldcFwdMax instead of bldcFwdMaxSlow)
 * - Same acceleration parameters apply
 */
