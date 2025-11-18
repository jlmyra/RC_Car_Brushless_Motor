//********************************************************************************
//*** Battery Monitoring Component ***
//*** Non-blocking battery voltage monitoring with LED indicators ***
//********************************************************************************

void computeBatteryVoltage() {
  
  unsigned long currentMillis = millis();
  
  // Sample battery voltage at fixed interval (50Hz)
  if (currentMillis - lastBatteryRead >= BATTERY_READ_INTERVAL) {
    lastBatteryRead = currentMillis;
    sampleBattery();
  }
  
  // Update battery status display at slower interval (1Hz)
  if (batteryBufferFull && (currentMillis - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL)) {
    lastBatteryUpdate = currentMillis;
    updateBatteryStatus();
  }
}

//------------------------------------------------------------------------------
// Sample Battery Voltage
//------------------------------------------------------------------------------
void sampleBattery() {
  
  // Read ADC value
  adcRead = analogRead(batPin);
  
  // Calibrate using ESP32's built-in calibration
  batteryVoltage = readADC_Cal(adcRead) * 0.001;  // Convert mV to V
  
  // Apply correction factor (if needed)
  batteryVoltage -= batCorrFactor;
  
  // Store in circular buffer for rolling average
  batteryReadings[batteryIndex] = batteryVoltage;
  batteryIndex++;
  
  // Wrap around when buffer is full
  if (batteryIndex >= BATTERY_SAMPLES) {
    batteryIndex = 0;
    batteryBufferFull = true;
  }
}

//------------------------------------------------------------------------------
// Update Battery Status
//------------------------------------------------------------------------------
void updateBatteryStatus() {
  
  // Calculate rolling average of last N samples
  float sum = 0;
  for (int i = 0; i < BATTERY_SAMPLES; i++) {
    sum += batteryReadings[i];
  }
  batteryVoltageAvg = sum / BATTERY_SAMPLES;
  
  // Apply voltage divider correction to get actual battery voltage
  batteryVoltageCorr = batteryVoltageAvg * mSlope;
  
  // Update LED indicators on PS3 controller
  updateBatteryLEDs(batteryVoltageCorr);
  
  // Print status to serial monitor (when voltage changes or every 5 seconds)
  static float lastPrintedVoltage = 0;
  static unsigned long lastPrint = 0;
  
  if (abs(batteryVoltageCorr - lastPrintedVoltage) > 0.1 || (millis() - lastPrint > 5000)) {
    printBatteryStatus(batteryVoltageCorr);
    lastPrintedVoltage = batteryVoltageCorr;
    lastPrint = millis();
  }
}

//------------------------------------------------------------------------------
// Update Battery LED Indicators
//------------------------------------------------------------------------------
void updateBatteryLEDs(float voltage) {

  if (voltage > 8.0) {
    // Full charge - Green LED
    if (PS4.isConnected()) {
      PS4.setLed(0, 255, 0);  // Green
      PS4.sendToController();
    }
    rumbleCounter = 0;
  }
  else if (voltage > 7.3) {
    // Good charge - Blue LED
    if (PS4.isConnected()) {
      PS4.setLed(0, 0, 255);  // Blue
      PS4.sendToController();
    }
    rumbleCounter = 0;
  }
  else if (voltage > 6.7) {
    // Low charge - Yellow LED
    if (PS4.isConnected()) {
      PS4.setLed(255, 255, 0);  // Yellow
      PS4.sendToController();
    }
    rumbleCounter = 0;
  }
  else if (voltage > 6.5) {
    // Very low - Red LED + rumble warning
    if (PS4.isConnected()) {
      PS4.setLed(255, 0, 0);  // Red
      PS4.sendToController();
    }
    handleLowBatteryRumble();
  }
  else {
    // Critical - Flashing Red (emergency stop will be triggered by main loop)
    if (PS4.isConnected()) {
      PS4.setLed(255, 0, 0);  // Red
      PS4.setFlashRate(500, 500);  // Flash every 500ms
      PS4.sendToController();
    }
  }
}

//------------------------------------------------------------------------------
// Handle Low Battery Rumble Warning
//------------------------------------------------------------------------------
void handleLowBatteryRumble() {

  const int RUMBLE_DELAY_SECONDS = 15;

  if (rumbleCounter >= RUMBLE_DELAY_SECONDS) {
    // Rumble continuously to warn user
    if (PS4.isConnected()) {
      PS4.setRumble(125, 125);  // Medium intensity on both motors
      PS4.sendToController();
      delay(100);  // 100ms pulse
      PS4.setRumble(0, 0);  // Turn off
      PS4.sendToController();
    }

    // Keep rumbling at this level
    rumbleCounter = RUMBLE_DELAY_SECONDS;
  } else {
    // Count up to delay threshold
    rumbleCounter++;
  }
}

//------------------------------------------------------------------------------
// Print Battery Status to Serial Monitor
//------------------------------------------------------------------------------
void printBatteryStatus(float voltage) {
  
  Serial.print("🔋 Battery: ");
  Serial.print(voltage, 2);
  Serial.print("V ");
  
  // Visual battery indicator
  if (voltage > 8.0) {
    Serial.println("████ (Full)");
  } else if (voltage > 7.3) {
    Serial.println("███░ (Good)");
  } else if (voltage > 6.7) {
    Serial.println("██░░ (Low)");
  } else if (voltage > 6.5) {
    Serial.println("█░░░ ⚠️  (Very Low - Charge Soon!)");
  } else {
    Serial.println("░░░░ ⛔ (CRITICAL - System will stop!)");
  }
}

//------------------------------------------------------------------------------
// ADC Calibration Function
//------------------------------------------------------------------------------
uint32_t readADC_Cal(int ADC_Raw) {
  
  esp_adc_cal_characteristics_t adc_chars;
  
  // Use ESP32's built-in calibration data
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  
  // Convert raw ADC value to calibrated voltage in mV
  return esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars);
}

//********************************************************************************
//*** Battery Monitoring Notes ***
//********************************************************************************
/*
 * Battery Voltage Monitoring:
 * 
 * Hardware Requirements:
 * - Voltage divider circuit to scale battery voltage to 0-3.3V range
 * - ESP32 ADC can only measure up to 3.3V (with 11dB attenuation)
 * - Typical 2S LiPo: 8.4V fully charged, 6.0V discharged
 * 
 * Voltage Divider Design:
 * - R1 = 100kΩ (high side, to battery +)
 * - R2 = 42kΩ (low side, to ground)
 * - Vout = Vin × (R2 / (R1 + R2))
 * - For 8.4V battery: Vout = 2.48V (safe for ESP32)
 * - Add 0.1μF capacitor across R2 to reduce noise
 * 
 * Voltage Thresholds (2S LiPo):
 * - 8.4V = Fully charged (4.2V per cell)
 * - 8.0V = Full indicator (4.0V per cell)
 * - 7.3V = Good indicator (3.65V per cell)
 * - 6.7V = Low warning (3.35V per cell)
 * - 6.5V = Critical stop (3.25V per cell)
 * - 6.0V = Damaged if discharged further (3.0V per cell)
 * 
 * Monitoring Method:
 * - Circular buffer of 50 samples
 * - Sampled at 50Hz (every 20ms)
 * - Rolling average to filter noise
 * - Non-blocking operation
 * - Updates display every 1 second
 * 
 * LED Indicators (PS3 Controller):
 * - 4 LEDs (pattern 10) = Full charge (>8.0V)
 * - 3 LEDs (pattern 9) = Good charge (>7.3V)
 * - 2 LEDs (pattern 7) = Low charge (>6.7V)
 * - 1 LED (pattern 4) = Very low (>6.5V) + rumble
 * - Below 6.5V = Emergency stop activated
 * 
 * Rumble Warning:
 * - Activates when voltage drops below 6.7V
 * - Delays 15 seconds before rumbling (avoid false alarms)
 * - Rumbles continuously every second
 * - Warns user to stop driving and recharge
 * 
 * Calibration:
 * - Measure actual battery voltage with multimeter
 * - Compare to displayed voltage
 * - Adjust batCorrFactor in Model_Variables.h
 * - Positive factor = measured voltage is higher than displayed
 * - Negative factor = measured voltage is lower than displayed
 * - Typical range: -0.2 to +0.2 volts
 * 
 * Safety Features:
 * - Main loop monitors batteryVoltageCorr
 * - Emergency stop triggered at 6.5V
 * - Prevents over-discharge damage to LiPo batteries
 * - Visual and haptic warnings before cutoff
 */
