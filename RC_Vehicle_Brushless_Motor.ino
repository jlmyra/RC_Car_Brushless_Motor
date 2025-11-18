//********************************************************************************
//*** RC Vehicle Control System - Main Sketch ***
//*** Version: 2.2 - Modular Design ***
//*** For: JEEP, LANDY, SAKURA- CARS WITH BRUSHLESS MOTORS ***
//********************************************************************************

#include <Ps3Controller.h>
#include "esp_adc_cal.h"
#include "esp_task_wdt.h"
#include "Model_Variables.h"
#include "esp32-hal-ledc.h"

//********************************************************************************
//*** CONFIGURATION ***
//********************************************************************************

#define PS3_MAC_ADDRESS "b8:27:eb:37:85:b9"  // ← Change to your controller MAC

//********************************************************************************
//*** PIN DEFINITIONS ***
//********************************************************************************

// Steering Servo
int steerPin = 4;
int steerChannel = 1;
int steerMin = MV_servoMin;
int steerMid = 4915;
int steerMax = MV_servoMax;
bool steerInverted = MV_servoInverted;
int steerFreq = 50;
int steerTimerWidth = 16;

// Motor ESC
int bldcPin = 25;
int bldcChannel = 2;
int bldcFreq = 50;
int bldcTimerWidth = 16;
int bldcRevMin = 3441;
int bldcFwdMax = 6554;
int bldcRevMinSlow = 4096;
int bldcFwdMaxSlow = 5898;
int bldcStopSpeed = 4915;
int bldcStartBoost = VEHICLE.motor.startBoost;  // From Model_Variables.h

// Winch Motors
uint8_t winchPWMChannel_1 = 4;
uint8_t winchPWMChannel_2 = 5;
int winchDirection_1 = 26;
int winchDirection_2 = 18;
uint32_t winchFreq = 30000;
uint8_t winchResolution = 8;

// Battery Monitor
const int batPin = 32;
int R1 = MV_R1;
int R2 = MV_R2;
float batCorrFactor = MV_batCorrFactor;

//********************************************************************************
//*** GLOBAL VARIABLES ***
//********************************************************************************

// Motor Control
float bldcTargetSpeed = 4915.0;
float bldcSmoothSpeed = 4915.0;
float ACCEL_SMOOTH_FACTOR = VEHICLE.motor.accelFactor;  // From Model_Variables.h
float DECEL_SMOOTH_FACTOR = VEHICLE.motor.decelFactor;  // From Model_Variables.h
int bldcCurrentSpeed = 4915;
int leftJoystickPos = 0;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 20;

// Steering Control
int steerJoystickPos = 0;

// Battery Monitoring
#define BATTERY_SAMPLES 50
float batteryReadings[BATTERY_SAMPLES];
int batteryIndex = 0;
bool batteryBufferFull = false;
unsigned long lastBatteryRead = 0;
const unsigned long BATTERY_READ_INTERVAL = 20;
unsigned long lastBatteryUpdate = 0;
const unsigned long BATTERY_UPDATE_INTERVAL = 1000;
float batteryVoltageCorr = 0;
float batteryVoltageAvg = 0;
float adcRead = 0;
float batteryVoltage = 0;
int rumbleCounter = 0;
float vOutMax = 8.4 * R2 / (R1 + R2);
float mSlope = 1 / (vOutMax / 8.4);

// Connection Status
bool ps3WasConnected = false;
unsigned long lastConnectionCheck = 0;

// Connection Animation
enum ConnectionAnimState {
  CONN_IDLE,
  CONN_LEFT,
  CONN_CENTER_1,
  CONN_RIGHT,
  CONN_CENTER_2,
  CONN_DONE
};
ConnectionAnimState connState = CONN_IDLE;
unsigned long connAnimStartTime = 0;

// LED Patterns
#define LED_PATTERN_FULL      10
#define LED_PATTERN_GOOD       9
#define LED_PATTERN_LOW        7
#define LED_PATTERN_CRITICAL   4

//********************************************************************************
//*** SETUP ***
//********************************************************************************

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n╔════════════════════════════════════════╗");
  Serial.println("║  RC Vehicle Control System v2.2        ║");
  Serial.println("║  Modular Design                        ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.printf("\nVehicle: %s\n", VEHICLE.name);
  Serial.printf("Motor Tuning - Accel: %.2f, Decel: %.2f, Boost: %d\n\n", 
                ACCEL_SMOOTH_FACTOR, DECEL_SMOOTH_FACTOR, bldcStartBoost);
  
  // Initialize Watchdog Timer
  Serial.println("Initializing watchdog timer...");
  esp_task_wdt_init(3, true);
  esp_task_wdt_add(NULL);
  
  // Initialize PS3 Controller
  Serial.print("Initializing PS3 controller...");
  if (!Ps3.begin(PS3_MAC_ADDRESS)) {
    Serial.println(" FAILED!");
    Serial.println("⚠️  Check MAC address and try again");
    while(1) { 
      delay(1000); 
      esp_task_wdt_reset();
    }
  }
  Serial.println(" OK");
  
  Ps3.attachOnConnect(onConnection);
  
  // Battery Setup
  Serial.println("Configuring battery monitor...");
  pinMode(batPin, INPUT);
  analogSetPinAttenuation(batPin, ADC_11db);
  
  // Winch PWM Setup
  Serial.println("Configuring winch...");
  pinMode(winchDirection_1, OUTPUT);
  pinMode(winchDirection_2, OUTPUT);
  digitalWrite(winchDirection_1, LOW);
  digitalWrite(winchDirection_2, LOW);
  delay(100);
  ledcSetup(winchPWMChannel_1, winchFreq, winchResolution);
  ledcAttachPin(winchDirection_1, winchPWMChannel_1);
  ledcSetup(winchPWMChannel_2, winchFreq, winchResolution);
  ledcAttachPin(winchDirection_2, winchPWMChannel_2);
  
  delay(100);
  
  // Steering Servo Setup
  Serial.println("Configuring steering...");
  ledcSetup(steerChannel, steerFreq, steerTimerWidth);
  ledcAttachPin(steerPin, steerChannel);
  
  delay(100);
  
  // ESC Motor Setup
  Serial.println("Configuring motor ESC...");
  ledcSetup(bldcChannel, bldcFreq, bldcTimerWidth);
  ledcAttachPin(bldcPin, bldcChannel);
  ledcWrite(bldcChannel, bldcStopSpeed);
  
  delay(500);
  
  // Center steering after power stabilizes
  Serial.println("Centering steering...");
  ledcWrite(steerChannel, steerMid);
  
  Serial.println("\n✓ Initialization complete");
  Serial.println("Waiting for PS3 controller...");
  Serial.println("Press PS button on controller to connect\n");
}

//********************************************************************************
//*** MAIN LOOP ***
//********************************************************************************

void loop() {
  esp_task_wdt_reset();
  
  // Check connection status every 100ms
  if (millis() - lastConnectionCheck > 100) {
    lastConnectionCheck = millis();
    
    bool currentlyConnected = Ps3.isConnected();
    
    // Detect new connection
    if (currentlyConnected && !ps3WasConnected) {
      ps3WasConnected = true;
      Serial.println("\n✓✓✓ PS3 Controller is CONNECTED and ACTIVE! ✓✓✓\n");
    }
    
    // Detect disconnection
    if (!currentlyConnected && ps3WasConnected) {
      ps3WasConnected = false;
      Serial.println("\n⚠️  Controller DISCONNECTED\n");
      emergencyStop();
      Serial.println("Waiting for PS3 controller...\n");
    }
  }
  
  // Main operation when connected
  if (ps3WasConnected) {
    // Check for critical battery
    if (batteryVoltageCorr < 6.5 && batteryVoltageCorr > 0) {
      Serial.printf("🔋 CRITICAL BATTERY: %.2fV - STOPPING\n", batteryVoltageCorr);
      emergencyStop();
      delay(5000);
      return;
    }
    
    // Normal operation - call component functions
    handleSteering();
    BLDC_move();
    computeBatteryVoltage();
    updateConnectionAnimation();
  } else {
    // Not connected - just wait
    delay(100);
  }
}
