//********************************************************************************
//*** Connection Handler Component ***
//*** Bluepad32 controller connection/disconnection and welcome animation ***
//********************************************************************************

//------------------------------------------------------------------------------
// Connection Event Handler
//------------------------------------------------------------------------------
void onConnectedController(ControllerPtr ctl) {

  // This function is called automatically when a controller connects
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  CONTROLLER CONNECTED!             ║");
  Serial.println("╚════════════════════════════════════╝\n");

  // Store controller reference
  myController = ctl;

  // Print controller info
  Serial.printf("Controller model: %s\n", ctl->getModelName().c_str());

  // Start connection welcome animation
  connState = CONN_LEFT;
  connAnimStartTime = millis();
}

//------------------------------------------------------------------------------
// Disconnection Event Handler
//------------------------------------------------------------------------------
void onDisconnectedController(ControllerPtr ctl) {

  // This function is called automatically when controller disconnects
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  CONTROLLER DISCONNECTED!          ║");
  Serial.println("╚════════════════════════════════════╝\n");

  // Clear controller reference if it matches
  if (myController == ctl) {
    myController = nullptr;
  }
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
 * Bluepad32 Controller Connection:
 *
 * Pairing Process:
 * 1. Power on ESP32 and wait for "Waiting for Bluetooth controller" message
 * 2. Put your controller in pairing mode:
 *    - PS3/PS4/PS5: Press and hold PS + Share buttons until light flashes
 *    - Xbox: Press and hold pairing button on top of controller
 *    - Switch Pro: Press and hold sync button until lights flash
 *    - 8BitDo: Refer to controller manual for pairing mode
 * 3. Controller will automatically connect to ESP32
 * 4. No MAC address configuration needed!
 *
 * Connection Flow:
 * 1. User puts controller in pairing mode
 * 2. ESP32 detects and accepts Bluetooth connection
 * 3. onConnectedController() callback is triggered
 * 4. Controller reference is stored in myController variable
 * 5. Welcome animation starts (steering wiggle)
 * 6. Main loop detects connection via myController->isConnected()
 * 7. System becomes active and responds to controller input
 *
 * Connection Animation:
 * - Visual confirmation that controller is connected
 * - Steering servo wiggles: Left → Center → Right → Center
 * - Total duration: 2 seconds
 * - Non-blocking (doesn't delay other operations)
 * - Can be disabled by commenting out in main loop
 *
 * Supported Controllers:
 * - Sony: DualSense (PS5), DualShock 4 (PS4), DualShock 3 (PS3)
 * - Microsoft: Xbox Wireless (Series X/S, One S)
 * - Nintendo: Switch Pro Controller, Joy-Cons, Wii U Pro
 * - 8BitDo: Most models
 * - Generic: Most Bluetooth HID gamepads
 *
 * Disconnection Handling:
 * - Detected by main loop (myController->isConnected() check)
 * - onDisconnectedController() callback is triggered
 * - Emergency stop automatically activated
 * - All motors stopped immediately
 * - System returns to waiting state
 * - Safe to power off or reconnect
 *
 * Troubleshooting Connection Issues:
 *
 * Controller won't pair:
 * - Ensure controller is in pairing mode (lights flashing)
 * - Check controller battery is charged
 * - Try forgetting Bluetooth keys: BP32.forgetBluetoothKeys()
 * - Power cycle both controller and ESP32
 * - Some controllers need to be unpaired from other devices first
 *
 * Connects but doesn't respond:
 * - Check serial monitor for "CONNECTED and ACTIVE" message
 * - Verify myController is not null
 * - Check controller model is supported
 * - Check for brownout (power issue)
 *
 * Animation doesn't run:
 * - Check servo is connected and powered
 * - Verify steerMin/Max values are correct
 * - May indicate servo power or connection issue
 *
 * Multiple Controllers:
 * - Bluepad32 supports up to 4 controllers simultaneously
 * - This code uses only the first connected controller
 * - To use multiple controllers, modify myController to an array
 *
 * Forgetting Paired Devices:
 * - Call BP32.forgetBluetoothKeys() in setup to clear all pairings
 * - Useful when switching to a new controller
 * - Current code calls this on startup for easy pairing
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
