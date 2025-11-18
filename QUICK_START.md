# QUICK START - PS4 Controller Version

## 📥 Download All Files

All modular files should be in the same folder:

1. RC_Vehicle_Brushless_Motor.ino
2. Motor_Control.ino
3. Steering_Control.ino
4. Winch_Control.ino
5. Battery_Monitor.ino
6. Connection_Handler.ino
7. Emergency_Stop.ino
8. Model_Variables.h

---

## ⚡ 5-Minute Setup

### 1. Install PS4Controller Library
- Open Arduino IDE → Sketch → Include Library → Manage Libraries
- Search for "PS4_Controller_Host" by pablomarquez76
- Click Install
- Or download from: https://github.com/pablomarquez76/PS4_Controller_Host

### 2. Configure Your Vehicle
**Model_Variables.h** - Line 14:
```cpp
#define VEHICLE_JEEP    // ← Your vehicle
```

**Model_Variables.h** - Motor tuning:
```cpp
.motor = {
  .accelFactor = 0.15,    // ← Start with these
  .decelFactor = 0.10,    //    Tune later
  .startBoost = 33
}
```

### 3. Set PS4 Controller MAC (Optional)
**RC_Vehicle_Brushless_Motor.ino** - Line 17:
```cpp
#define PS4_MAC_ADDRESS "01:02:03:04:05:06"  // ← Your MAC
```
**Or** use `PS4.begin()` without MAC to connect to any PS4 controller

### 4. Upload
- Open `RC_Vehicle_Brushless_Motor.ino` in Arduino IDE
- Select ESP32 board and port
- Click Upload
- Open Serial Monitor (115200)

### 5. Connect PS4 Controller
- Press **PS button** on your PS4 controller
- Watch Serial Monitor for "PS4 CONTROLLER CONNECTED!"
- Green lightbar = ready to drive!

---

## 🎛️ Tuning for Each Vehicle

Each vehicle can have different settings in **Model_Variables.h**:

### Heavy Truck (Jeep):
```cpp
.accelFactor = 0.20,    // Needs more power
.decelFactor = 0.15,    // Faster stop
.startBoost = 50        // Heavy = more boost
```

### Light Buggy:
```cpp
.accelFactor = 0.25,    // Quick response
.decelFactor = 0.08,    // Keep momentum
.startBoost = 25        // Light = less boost
```

### Crawler (Landy):
```cpp
.accelFactor = 0.12,    // Very smooth
.decelFactor = 0.08,    // Gentle
.startBoost = 55        // Lots of torque
```

---

## 🔄 Switching Between Vehicles

Just change ONE line in Model_Variables.h:

```cpp
// For Sakura:
#define VEHICLE_SAKURA
// #define VEHICLE_JEEP
// #define VEHICLE_LANDY
// #define VEHICLE_BUGGY

// For Jeep:
// #define VEHICLE_SAKURA
#define VEHICLE_JEEP
// #define VEHICLE_LANDY
// #define VEHICLE_BUGGY
```

Upload, and all the right settings load automatically!

---

## 📊 What You Get

### ✅ Vehicle-Specific Tuning
- Each vehicle has perfect acceleration
- Different weights/gearing handled
- Store settings for all your vehicles

### ✅ Clean Modular Code
- Easy to understand
- Each component separate
- Well documented
- Easy to modify

### ✅ All Your Features
- Smooth motor control
- Turbo mode (L1 button)
- Steering (right stick)
- Winch (D-pad up/down)
- Battery monitoring with color-coded lightbar
- Emergency stop
- PS4 wireless Bluetooth connection

---

## 🎯 Quick Tuning Workflow

1. **Test Current Settings**
   - Drive on workbench
   - Note how it feels

2. **Adjust One Parameter**
   - Too slow? Increase accelFactor by 0.03
   - Too jerky? Decrease accelFactor by 0.03
   - Won't start? Increase startBoost by 10

3. **Upload and Test**
   - Try the change
   - Compare to before

4. **Repeat Until Perfect**
   - Fine-tune in small steps
   - Each vehicle is different

---

## 💡 Pro Tips

### Record Your Settings
Keep notes for each vehicle:
```
Sakura:
- accelFactor: 0.15 (perfect)
- decelFactor: 0.10 (maybe try 0.12?)
- startBoost: 33 (good)

Jeep:
- accelFactor: 0.18 (needs 0.20)
- decelFactor: 0.12 (perfect)
- startBoost: 45 (try 50)
```

### Test in Same Conditions
- Same surface (carpet, asphalt, dirt)
- Same battery level
- Same temperature
- Makes comparisons valid

### Start Conservative
- Begin with low values
- Increase gradually
- Safer than starting too aggressive

---

## 📁 Folder Structure

```
RC_Car_Brushless_Motor/
├── RC_Vehicle_Brushless_Motor.ino  ← Open this in Arduino
├── Motor_Control.ino
├── Steering_Control.ino
├── Winch_Control.ino
├── Battery_Monitor.ino
├── Connection_Handler.ino
├── Emergency_Stop.ino
└── Model_Variables.h               ← Configure this
```

**Important:** Folder name MUST match main .ino filename!

---

## 🆘 Problems?

### Won't Compile
- Check all 8 files are in same folder
- Folder name matches RC_Vehicle_Main.ino
- Only ONE vehicle uncommented

### Motor Issues
- See Motor_Control.ino notes
- See tuning guide in Model_Variables.h
- Check serial monitor for debug output

### Full Help
- See **README.md** for complete guide
- Each .ino file has detailed notes at bottom
- Model_Variables.h has extensive tuning guide

---

You're all set! Each vehicle will now have perfectly tuned acceleration and deceleration! 🚗💨
