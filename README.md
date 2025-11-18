# RC Vehicle Control System - Version 3.0 with PS4 Controller Support

## 📦 File Structure

Your sketch folder should contain these files:

```
RC_Vehicle_Main/
├── RC_Vehicle_Brushless_Motor.ino  ← Main sketch (setup & loop)
├── Motor_Control.ino               ← BLDC motor with acceleration tuning
├── Steering_Control.ino            ← Servo steering control
├── Winch_Control.ino               ← Winch motor control
├── Battery_Monitor.ino             ← Non-blocking battery monitoring
├── Connection_Handler.ino          ← PS4 connection & animation
├── Emergency_Stop.ino              ← Safety stop function
└── Model_Variables.h               ← Vehicle-specific configuration
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

### Step 4: Install PS4Controller Library
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for "PS4Controller" or "PS4_Controller_Host"
4. Install the library by **pablomarquez76**
5. Alternatively, download from: https://github.com/pablomarquez76/PS4_Controller_Host

### Step 5: Set PS4 MAC Address
Open **RC_Vehicle_Brushless_Motor.ino**, find line ~17:
```cpp
#define PS4_MAC_ADDRESS "01:02:03:04:05:06"  // ← Change to your controller MAC
```

**Note:** You can also use `PS4.begin()` without a MAC address to connect to any PS4 controller.

### Step 6: Pair PS4 Controller
1. **Method 1 (No MAC needed):** Simply use `PS4.begin()` in code and press PS button
2. **Method 2 (Specific MAC):** Use SixaxisPairTool to pair controller with ESP32's MAC
3. Find ESP32 MAC: Add `Serial.println(WiFi.macAddress());` to setup()

### Step 7: Upload
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

## 🎮 PS4 Controller Layout

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
- **Up** = Unwind cable (fixed speed)
- **Down** = Rewind cable (fixed speed)
- **Note:** PS4 D-pad buttons are digital (on/off), unlike PS3's analog pressure

### Lightbar (Battery Level)
- **Green** = Full charge (>8.0V)
- **Blue** = Good charge (>7.3V)
- **Yellow** = Low charge (>6.7V)
- **Red** = Very low (>6.5V) + rumble warning
- **Flashing Red** = Critical (<6.5V) - emergency stop

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
- RGB lightbar indicators on PS4 controller
- Color-coded battery status (Green/Blue/Yellow/Red)
- Rumble warning at low voltage
- Emergency stop at critical voltage

### Connection_Handler.ino
- Auto-connect on PS button press (PS4 controller)
- Welcome animation (steering wiggle)
- Non-blocking animation
- Works with PS4 wireless Bluetooth connection

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
║  RC Vehicle Control System v3.0        ║
║  PS4 Controller Support                ║
╚════════════════════════════════════════╝

Vehicle: Sakura
Motor Tuning - Accel: 0.15, Decel: 0.10, Boost: 33

Initializing watchdog timer...
Initializing PS4 controller... OK
Configuring battery monitor...
Configuring winch...
Configuring steering...
Configuring motor ESC...
Centering steering...

✓ Initialization complete
Waiting for PS4 controller...

╔════════════════════════════════════╗
║  PS4 CONTROLLER CONNECTED!         ║
╚════════════════════════════════════╝

✓✓✓ PS4 Controller is CONNECTED and ACTIVE! ✓✓✓

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
2. Wait for "Waiting for PS3 controller"
3. Press PS button on controller
4. Watch for steering wiggle animation
5. See "Ready to drive!" message
6. Start driving!

Enjoy your perfectly tuned RC vehicle! 🎉
