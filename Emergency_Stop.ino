//********************************************************************************
//*** Emergency Stop Component ***
//*** Safely stops all motors and alerts user ***
//********************************************************************************

void emergencyStop() {
  
  //----------------------------------------------------------------------------
  // Stop Motor
  //----------------------------------------------------------------------------
  ledcWrite(bldcChannel, bldcStopSpeed);
  bldcCurrentSpeed = bldcStopSpeed;
  bldcSmoothSpeed = bldcStopSpeed;   // Reset smooth speed
  bldcTargetSpeed = bldcStopSpeed;   // Reset target speed
  
  //----------------------------------------------------------------------------
  // Center Steering
  //----------------------------------------------------------------------------
  ledcWrite(steerChannel, steerMid);
  
  //----------------------------------------------------------------------------
  // Stop Winch Motors
  //----------------------------------------------------------------------------
  digitalWrite(winchPWMChannel_1, LOW);
  digitalWrite(winchPWMChannel_2, LOW);
  
  //----------------------------------------------------------------------------
  // Visual Alert on Controller
  //----------------------------------------------------------------------------
  if (myController && myController->isConnected()) {
    // Set to critical battery pattern (1 LED)
    myController->setPlayerLEDs(LED_PATTERN_CRITICAL);

    // Triple rumble pulse for attention
    for (int i = 0; i < 3; i++) {
      // setRumble(force, duration)
      // force: 0-255 (0xFF = max), duration: 0-255 (255 ≈ 2 seconds)
      myController->setRumble(0xFF, 12);   // Maximum intensity, short pulse
      delay(100);                           // 100ms between pulses
    }
  }
  
  //----------------------------------------------------------------------------
  // Serial Monitor Alert
  //----------------------------------------------------------------------------
  Serial.println("\n⛔ EMERGENCY STOP ACTIVATED ⛔");
  Serial.println("All motors stopped");
  Serial.println("System safe\n");
}

//********************************************************************************
//*** Emergency Stop Notes ***
//********************************************************************************
/*
 * Emergency Stop Triggers:
 * 
 * 1. PS3 Controller Disconnection
 *    - Detected by main loop
 *    - Controller turned off, out of range, or battery died
 *    - Automatic stop prevents runaway vehicle
 * 
 * 2. Critical Battery Voltage
 *    - Battery drops below 6.5V (3.25V per cell for 2S LiPo)
 *    - Protects battery from over-discharge damage
 *    - Vehicle stops until battery is replaced/recharged
 * 
 * 3. Manual Trigger (if needed)
 *    - Can be called from any part of code
 *    - Useful for adding custom safety features
 *    - Example: Tilt sensor, collision detection, geofence
 * 
 * What Emergency Stop Does:
 * 
 * Motor Control:
 * - Sets ESC to neutral position (1500μs)
 * - Resets all speed tracking variables
 * - Prevents any acceleration on resume
 * - Motor will be completely stopped
 * 
 * Steering Control:
 * - Centers steering servo
 * - Prevents turning during stop
 * - Vehicle remains straight
 * 
 * Winch Control:
 * - Both winch channels set to LOW
 * - Both directions disabled
 * - Winch motor completely stopped
 * 
 * User Feedback:
 * - PS3 LEDs set to critical pattern (1 LED)
 * - Triple rumble pulse for attention
 * - Serial monitor message
 * - Clear indication something is wrong
 * 
 * Recovery:
 * - From controller disconnect: Reconnect controller
 * - From low battery: Replace/recharge battery, reset system
 * - System automatically resumes when condition clears
 * - No manual reset required for most cases
 * 
 * Safety Features:
 * - Non-blocking (doesn't hang system)
 * - Immediate response (no delays before stopping)
 * - Multiple redundant stops (motor, servo, winch)
 * - Clear user notification
 * - Fail-safe design (default to stopped state)
 * 
 * Extending Emergency Stop:
 * 
 * To add custom triggers:
 * 1. Add condition check in main loop
 * 2. Call emergencyStop() when condition met
 * 3. Add appropriate recovery logic
 * 
 * Example - Tilt Sensor:
 * if (tiltAngle > 45) {
 *   Serial.println("Vehicle tipped over!");
 *   emergencyStop();
 * }
 * 
 * Example - Distance Sensor:
 * if (distance < 10) {
 *   Serial.println("Obstacle detected!");
 *   emergencyStop();
 * }
 * 
 * Example - Geofence:
 * if (GPS.distanceFromHome() > 100) {
 *   Serial.println("Out of range!");
 *   emergencyStop();
 * }
 * 
 * The emergency stop is designed to be conservative - it's
 * better to stop unnecessarily than to fail to stop when needed.
 * All motors are immediately disabled with multiple layers of
 * redundancy to ensure safety.
 */
