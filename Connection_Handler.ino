//********************************************************************************
//*** Connection Handler Component ***
//*** PS3 controller connection/disconnection and welcome animation ***
//********************************************************************************

//------------------------------------------------------------------------------
// Connection Event Handler
//------------------------------------------------------------------------------
void onConnection() {
  
  // This function is called automatically when PS3 controller connects
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  PS3 CONTROLLER CONNECTED!         ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // Start connection welcome animation
  connState = CONN_LEFT;
  connAnimStartTime = millis();
}

//------------------------------------------------------------------------------
// Connection Animation (Steering Wiggle)
//------------------------------------------------------------------------------
void updateConnectionAnimation() {
  
  // Skip if animation is not active
  if (connState == CONN_IDLE || connState == CONN_DONE) return;
  
  unsigned long elapsed = millis() - connAnimStartTime;
  
  switch(connState) {
    
    //--------------------------------------------------------------------------
    case CONN_LEFT:
      // Turn steering full left
      if (elapsed == 0) {
        ledcWrite(steerChannel, steerMax);
      }
      // Wait 500ms, then move to next state
      if (elapsed > 500) {
        connState = CONN_CENTER_1;
        connAnimStartTime = millis();
      }
      break;
    
    //--------------------------------------------------------------------------
    case CONN_CENTER_1:
      // Return to center
      if (elapsed == 0) {
        ledcWrite(steerChannel, steerMid);
      }
      // Wait 500ms, then move to next state
      if (elapsed > 500) {
        connState = CONN_RIGHT;
        connAnimStartTime = millis();
      }
      break;
    
    //--------------------------------------------------------------------------
    case CONN_RIGHT:
      // Turn steering full right
      if (elapsed == 0) {
        ledcWrite(steerChannel, steerMin);
      }
      // Wait 500ms, then move to next state
      if (elapsed > 500) {
        connState = CONN_CENTER_2;
        connAnimStartTime = millis();
      }
      break;
    
    //--------------------------------------------------------------------------
    case CONN_CENTER_2:
      // Return to center and finish
      if (elapsed == 0) {
        ledcWrite(steerChannel, steerMid);
      }
      // Wait 500ms, then complete animation
      if (elapsed > 500) {
        connState = CONN_DONE;
        Serial.println("✓ Connection animation complete - Ready to drive!\n");
      }
      break;
    
    //--------------------------------------------------------------------------
    default:
      break;
  }
}

//********************************************************************************
//*** Connection Handler Notes ***
//********************************************************************************
/*
 * PS3 Controller Connection:
 * 
 * Pairing Process:
 * 1. Controller must be paired with ESP32's Bluetooth MAC address
 * 2. Use SixAxis Pair Tool (Windows) or sixpair (Linux/Mac)
 * 3. Connect controller to computer via USB
 * 4. Use tool to set controller's master device to ESP32's MAC
 * 5. Disconnect USB, press PS button on controller
 * 6. Controller should connect to ESP32 via Bluetooth
 * 
 * Connection Flow:
 * 1. User presses PS button on controller
 * 2. Controller attempts Bluetooth connection to paired device
 * 3. ESP32 receives connection request
 * 4. onConnection() callback is triggered
 * 5. Welcome animation starts (steering wiggle)
 * 6. Main loop detects connection via ps3WasConnected flag
 * 7. System becomes active and responds to controller input
 * 
 * Connection Animation:
 * - Visual confirmation that controller is connected
 * - Steering servo wiggles: Left → Center → Right → Center
 * - Total duration: 2 seconds
 * - Non-blocking (doesn't delay other operations)
 * - Can be disabled by commenting out in main loop
 * 
 * MAC Address Configuration:
 * - Set in main sketch: PS3_MAC_ADDRESS
 * - Must match your PS3 controller's paired MAC
 * - Format: "XX:XX:XX:XX:XX:XX" (colon-separated hex)
 * - Can find ESP32's MAC by adding to setup():
 *   Serial.println(WiFi.macAddress());
 * 
 * Disconnection Handling:
 * - Detected by main loop (ps3WasConnected flag changes)
 * - Emergency stop automatically triggered
 * - All motors stopped immediately
 * - System returns to waiting state
 * - Safe to power off or reconnect
 * 
 * Troubleshooting Connection Issues:
 * 
 * Controller won't connect:
 * - Verify MAC address matches in code and controller
 * - Check controller is charged
 * - Try re-pairing with SixAxis tool
 * - Power cycle both controller and ESP32
 * 
 * Connects but doesn't respond:
 * - Check serial monitor for "CONNECTED and ACTIVE" message
 * - Verify ps3WasConnected flag is set
 * - Test with different controller if available
 * - Check for brownout (power issue)
 * 
 * Animation doesn't run:
 * - Check servo is connected and powered
 * - Verify steerMin/Max values are correct
 * - May indicate servo power or connection issue
 * 
 * Multiple controllers:
 * - Only one controller can be connected at a time
 * - All paired controllers will attempt to connect
 * - First to connect wins
 * - Others will fail to connect
 * 
 * Connection State Machine:
 * - CONN_IDLE = No animation active
 * - CONN_LEFT = Moving to left position
 * - CONN_CENTER_1 = First center position
 * - CONN_RIGHT = Moving to right position
 * - CONN_CENTER_2 = Final center position
 * - CONN_DONE = Animation complete
 * 
 * The animation is non-blocking - it runs in the background
 * while the main loop continues to execute. This ensures
 * the system remains responsive during the welcome sequence.
 */
