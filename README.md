# RC Vehicle Control System - Modular Version 2.2

## 📦 File Structure

Your sketch folder should contain these files:

```
RC_Vehicle_Main/
├── RC_Vehicle_Main.ino          ← Main sketch (setup & loop)
├── Motor_Control.ino             ← BLDC motor with acceleration tuning
├── Steering_Control.ino          ← Servo steering control
├── Winch_Control.ino             ← Winch motor control
├── Battery_Monitor.ino           ← Non-blocking battery monitoring
├── Connection_Handler.ino        ← Controller connection & animation
├── Emergency_Stop.ino            ← Safety stop function
└── Model_Variables.h             ← Vehicle-specific configuration
```

**IMPORTANT:** The main sketch file MUST have the same name as the folder!
Example: If folder is `RC_Vehicle_Main`, the main file must be `RC_Vehicle_Main.ino`

---

## 🎯 Key Features

### Vehicle-Specific Tuning
All tuning parameters are now in **Model_Variables.h**:
- ✅ Acceleration rate (accelFactor)
- ✅ Deceleration rate (decelFactor)  
- ✅ Start boost power (startBoost)
- ✅ Steering calibration
- ✅ Battery voltage divider values

### Modular Design
Each component is in its own file:
- ✅ Easy to understand and modify
- ✅ Well-documented with detailed notes
- ✅ Independent testing possible
- ✅ Reusable across projects

---

## 🚀 Installation

### Step 1: Create Sketch Folder
1. Open Arduino IDE
2. File → New
3. Save as: `RC_Vehicle_Main` (or your preferred name)

### Step 2: Add All Files
1. Close Arduino IDE
2. Navigate to sketch folder
3. Delete the default .ino file
4. Copy ALL 8 files into the folder:
   - RC_Vehicle_Main.ino
   - Motor_Control.ino
   - Steering_Control.ino
   - Winch_Control.ino
   - Battery_Monitor.ino
   - Connection_Handler.ino
   - Emergency_Stop.ino
   - Model_Variables.h

### Step 3: Configure Your Vehicle
Open **Model_Variables.h**:

1. **Select your vehicle** (uncomment ONE):
   ```cpp
   #define VEHICLE_SAKURA    // ← Active
   // #define VEHICLE_JEEP
   // #define VEHICLE_LANDY
   // #define VEHICLE_BUGGY
   ```

2. **Adjust motor tuning for your vehicle**:
   ```cpp
   .motor = {
     .accelFactor = 0.15,    // 0.05-0.30 (higher = faster accel)
     .decelFactor = 0.10,    // 0.05-0.20 (higher = faster stop)
     .startBoost = 33        // 0-100 (power at low speeds)
   }
   ```

### Step 4: Install Bluepad32 Library
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "Bluepad32"
4. Install the latest version
5. Restart Arduino IDE

### Step 5: Upload
1. Connect ESP32
2. Select correct board and port
3. Click Upload
4. Open Serial Monitor (115200 baud)

---

## ⚙️ Tuning Your Vehicle

### Finding the Right Values

**Start Conservative:**
```cpp
.accelFactor = 0.15    // Moderate
.decelFactor = 0.10    // Smooth
.startBoost = 33       // Balanced
```

**Then Adjust Based on Testing:**

#### If acceleration is too slow/sluggish:
- **Increase accelFactor** by 0.03
- Example: 0.15 → 0.18

#### If acceleration is too jerky/wheelspin:
- **Decrease accelFactor** by 0.03
- Example: 0.15 → 0.12

#### If stopping is too abrupt:
- **Decrease decelFactor** by 0.02
- Example: 0.10 → 0.08

#### If takes too long to stop:
- **Increase decelFactor** by 0.02
- Example: 0.10 → 0.12

#### If vehicle doesn't start smoothly:
- **Increase startBoost** by 5-10
- Example: 33 → 40

#### If vehicle jerks when starting:
- **Decrease startBoost** by 5-10
- Example: 33 → 25

### Tuning by Vehicle Type

**Light Racing Buggy:**
```cpp
.accelFactor = 0.25,    // Fast response
.decelFactor = 0.15,    // Quick stops
.startBoost = 25        // Light vehicle
```

**Heavy Scale Truck:**
```cpp
.accelFactor = 0.15,    // Smooth, realistic
.decelFactor = 0.10,    // Gentle
.startBoost = 45        // Overcome weight
```

**Rock Crawler:**
```cpp
.accelFactor = 0.12,    // Very controlled
.decelFactor = 0.08,    // Maintain momentum
.startBoost = 55        // High torque
```

**Drift Car:**
```cpp
.accelFactor = 0.22,    // Responsive
.decelFactor = 0.18,    // Control slides
.startBoost = 30        // Easy wheelspin
```

---

## 🎮 Control Layout

### Left Stick (Motor Control)
- **Forward** = Push up (negative values)
- **Reverse** = Push down (positive values)
- **Stop** = Center (dead zone ±5)

### L1 Button (Turbo Mode)
- **Hold L1 + Left Stick** = Full speed range
- **Without L1** = Limited speed for control

### Right Stick (Steering)
- **Left** = Turn left
- **Right** = Turn right
- **Center** = Straight

### D-Pad (Winch)
- **Up** = Unwind cable
- **Down** = Rewind cable
- Digital on/off control (80% speed)

### Controller LEDs (Battery Level)
- **4 LEDs** = Full (>8.0V)
- **3 LEDs** = Good (>7.3V)
- **2 LEDs** = Low (>6.7V)
- **1 LED** = Critical (>6.5V) + rumble warning

---

## 🔧 Component Details

### Motor_Control.ino
- Exponential smoothing for natural acceleration
- Turbo mode support
- Start boost for low-speed control
- Vehicle-specific tuning from Model_Variables.h

### Steering_Control.ino
- Direct joystick to servo mapping
- Dead zone to prevent jitter
- Calibration via Model_Variables.h

### Winch_Control.ino
- Dual direction control
- Analog speed control
- Safety interlocks

### Battery_Monitor.ino
- Non-blocking circular buffer sampling
- 50Hz ADC sampling, 1Hz display
- LED indicators on controller
- Rumble warning at low voltage
- Emergency stop at critical voltage

### Connection_Handler.ino
- Auto-pairing with Bluetooth controllers
- Welcome animation (steering wiggle)
- Supports PS3/PS4/PS5/Xbox/Switch/8BitDo controllers
- Non-blocking animation

### Emergency_Stop.ino
- Stops all motors immediately
- Centers steering
- Visual/haptic feedback
- Triggered by: disconnect, low battery, manual

---

## 📊 Serial Monitor Output

Normal operation:
```
╔════════════════════════════════════════╗
║  RC Vehicle Control System v2.2        ║
║  Modular Design                        ║
╚════════════════════════════════════════╝

Vehicle: Sakura
Motor Tuning - Accel: 0.15, Decel: 0.10, Boost: 33

Initializing watchdog timer...
Initializing Bluepad32...
✓ Bluepad32 initialized
Ready to pair with any Bluetooth controller
Configuring battery monitor...
Configuring winch...
Configuring steering...
Configuring motor ESC...
Centering steering...

✓ Initialization complete
Waiting for Bluetooth controller...
Put your controller in pairing mode to connect

╔════════════════════════════════════╗
║  CONTROLLER CONNECTED!             ║
╚════════════════════════════════════╝

Controller model: Sony DualShock 4

✓✓✓ Controller is CONNECTED and ACTIVE! ✓✓✓

Motor: Joy=-80 Target=5500 Speed=5450
🔋 Battery: 7.85V ███░ (Good)
Steering: 60 → PWM: 5800
```

---

## 🐛 Troubleshooting

### Compilation Errors

**"File must be in sketch folder"**
- All .ino and .h files must be in same folder
- Folder name must match main .ino filename

**"VEHICLE not defined"**
- Uncomment one vehicle in Model_Variables.h
- Only one vehicle can be active at a time

**"Multiple definition errors"**
- Make sure you don't have duplicate function names
- Check that you only have ONE copy of each component file

### Runtime Issues

**Motor too aggressive**
- Reduce accelFactor in Model_Variables.h
- Try 0.10 or 0.12 for very smooth

**Motor too sluggish**
- Increase accelFactor in Model_Variables.h
- Try 0.20 or 0.25 for sporty response

**Doesn't start from stop**
- Increase startBoost value
- Try 40, 45, or 50

**Jerky at low speeds**
- Decrease startBoost value
- Try 20 or 25

**Steering not centered**
- Adjust steerMin/Max in Model_Variables.h
- See steering calibration notes in Steering_Control.ino

**Battery reads 0V**
- Check wiring to GPIO 32
- Verify voltage divider resistors
- Check batCorrFactor in Model_Variables.h

---

## 📝 Adding New Vehicles

### Step 1: Add to Model_Variables.h

```cpp
#ifdef VEHICLE_MYNEWCAR
  const VehicleConfig VEHICLE = {
    .name = "My New Car",
    .steering = {
      .min = 3600,
      .max = 6800
    },
    .motor = {
      .accelFactor = 0.15,
      .decelFactor = 0.10,
      .startBoost = 35
    },
    .battery = {
      .R1 = 100000,
      .R2 = 42000,
      .correctionFactor = 0.0
    }
  };
  
  #define MV_servoMin 3600
  #define MV_servoMax 6800
  #define MV_R1 100000
  #define MV_R2 42000
  #define MV_batCorrFactor 0.0
#endif
```

### Step 2: Update Validation Check

Add your vehicle to the #if statement:
```cpp
#if !defined(VEHICLE_SAKURA) && !defined(VEHICLE_JEEP) && \
    !defined(VEHICLE_LANDY) && !defined(VEHICLE_BUGGY) && \
    !defined(VEHICLE_MYNEWCAR)  // ← Add this
  #error "No vehicle selected!"
#endif
```

### Step 3: Select and Test

```cpp
// #define VEHICLE_SAKURA
// #define VEHICLE_JEEP
#define VEHICLE_MYNEWCAR    // ← Activate your vehicle
```

---

## 🎓 Learning Resources

Each component file has extensive notes at the bottom explaining:
- How it works
- Tuning parameters
- Calibration procedures
- Modification examples
- Safety considerations

Start with:
1. **Model_Variables.h** - Tuning guide
2. **Motor_Control.ino** - Acceleration tuning
3. **Battery_Monitor.ino** - Voltage monitoring

---

## 🚗 Ready to Drive!

1. Power on ESP32 with battery
2. Wait for "Waiting for Bluetooth controller"
3. Put your controller in pairing mode:
   - **PS3/PS4/PS5**: Hold PS + Share until light flashes
   - **Xbox**: Press pairing button on top of controller
   - **Switch Pro**: Press sync button until lights flash
   - **8BitDo**: Refer to controller manual
4. Watch for steering wiggle animation
5. See "Ready to drive!" message
6. Start driving!

**Supported Controllers:**
- Sony DualSense (PS5), DualShock 4 (PS4), DualShock 3 (PS3)
- Microsoft Xbox Wireless (Series X/S, One S)
- Nintendo Switch Pro, Joy-Cons
- 8BitDo and most generic Bluetooth gamepads

Enjoy your perfectly tuned RC vehicle! 🎉
