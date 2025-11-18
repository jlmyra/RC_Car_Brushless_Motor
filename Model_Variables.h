#ifndef MODEL_VARIABLES_H
#define MODEL_VARIABLES_H

//********************************************************************************
// VEHICLE CONFIGURATION SYSTEM
// Each vehicle can have different calibration and tuning
//********************************************************************************

// ┌─────────────────────────────────────────────────────────────────┐
// │  SELECT YOUR VEHICLE: Uncomment ONE vehicle below               │
// └─────────────────────────────────────────────────────────────────┘

// #define VEHICLE_SAKURA    
 #define VEHICLE_JEEP
// #define VEHICLE_LANDY   // <-- Currently active vehicle
// #define VEHICLE_BUGGY

//********************************************************************************
// VEHICLE CONFIGURATION STRUCTURES
//********************************************************************************

struct ServoCalibration {
  int min;    // Minimum PWM value (full left/right)
  int max;    // Maximum PWM value (full right/left)
};

struct MotorTuning {
  float accelFactor;      // Acceleration smoothness (0.05 = slow, 0.30 = fast)
  float decelFactor;      // Deceleration smoothness (0.05 = slow, 0.20 = fast)
  int startBoost;         // Extra power at low speeds (0-100)
};

struct VoltageDivider {
  int R1;                      // Voltage divider resistor 1 (Ohms)
  int R2;                      // Voltage divider resistor 2 (Ohms)
  float correctionFactor;      // Voltage correction offset
};

struct VehicleConfig {
  const char* name;            // Vehicle name for display
  ServoCalibration steering;   // Steering servo calibration
  MotorTuning motor;           // Motor acceleration tuning
  VoltageDivider battery;      // Battery monitoring config
};

//********************************************************************************
// VEHICLE DEFINITIONS
//********************************************************************************

#ifdef VEHICLE_SAKURA
  const VehicleConfig VEHICLE = {
    .name = "Sakura",
    .steering = {
      .min = 6799,  // 2075μs (swapped to invert steering direction)
      .max = 3604   // 1100μs (swapped to invert steering direction)
    },
    .motor = {
      .accelFactor = 0.15,      // Moderate acceleration
      .decelFactor = 0.10,      // Smooth deceleration
      .startBoost = 33          // Small boost to get moving
    },
    .battery = {
      .R1 = 99500,
      .R2 = 41600,
      .correctionFactor = 0.0
    }
  };

  // Legacy defines for compatibility
  #define MV_servoMin 6799
  #define MV_servoMax 3604
  #define MV_R1 99500
  #define MV_R2 41600
  #define MV_batCorrFactor 0.0
#endif

#ifdef VEHICLE_JEEP
  const VehicleConfig VEHICLE = {
    .name = "Jeep",
    .steering = {
      .min = 3604,  // 1100μs
      .max = 6799   // 2075μs
    },
    .motor = {
      .accelFactor = 0.20,      // Faster acceleration (heavier vehicle)
      .decelFactor = 0.15,      // Faster stopping
      .startBoost = 50          // More boost for heavy vehicle
    },
    .battery = {
      .R1 = 101600,
      .R2 = 41600,
      .correctionFactor = 0.0
    }
  };

  // Legacy defines for compatibility
  #define MV_servoMin 3604
  #define MV_servoMax 6799
  #define MV_R1 101600
  #define MV_R2 41600
  #define MV_batCorrFactor 0.0
#endif

#ifdef VEHICLE_LANDY
  const VehicleConfig VEHICLE = {
    .name = "Landy",
    .steering = {
      .min = 3586,  // 1125μs
      .max = 6708   // 1925μs
    },
    .motor = {
      .accelFactor = 0.18,      // Medium-fast acceleration
      .decelFactor = 0.12,      // Medium deceleration
      .startBoost = 40          // Medium boost
    },
    .battery = {
      .R1 = 102800,
      .R2 = 41700,
      .correctionFactor = 0.0
    }
  };

  // Legacy defines for compatibility
  #define MV_servoMin 3586
  #define MV_servoMax 6708
  #define MV_R1 102800
  #define MV_R2 41700
  #define MV_batCorrFactor 0.0
#endif

#ifdef VEHICLE_BUGGY
  const VehicleConfig VEHICLE = {
    .name = "Buggy",
    .steering = {
      .min = 4014,  // 1225μs
      .max = 6635   // 2025μs
    },
    .motor = {
      .accelFactor = 0.25,      // Fast acceleration (light, sporty)
      .decelFactor = 0.08,      // Gentle deceleration (keep momentum)
      .startBoost = 25          // Less boost needed (light vehicle)
    },
    .battery = {
      .R1 = 101600,
      .R2 = 41600,
      .correctionFactor = 0.0
    }
  };

  // Legacy defines for compatibility
  #define MV_servoMin 4014
  #define MV_servoMax 6635
  #define MV_R1 101600
  #define MV_R2 41600
  #define MV_batCorrFactor 0.0
#endif

//********************************************************************************
// VALIDATION
//********************************************************************************

#if !defined(VEHICLE_SAKURA) && !defined(VEHICLE_JEEP) && \
    !defined(VEHICLE_LANDY) && !defined(VEHICLE_BUGGY)
  #error "No vehicle selected! Uncomment one vehicle definition in Model_Variables.h"
#endif

//********************************************************************************
// TUNING GUIDE
//********************************************************************************
/*
 * MOTOR TUNING PARAMETERS:
 * 
 * accelFactor (Acceleration Smoothness):
 * - Range: 0.05 to 0.30
 * - Lower = Smoother, slower acceleration
 * - Higher = More aggressive, faster acceleration
 * - Recommended starting points:
 *   - Light vehicles: 0.20 - 0.25
 *   - Medium vehicles: 0.15 - 0.20
 *   - Heavy vehicles: 0.15 - 0.18
 * 
 * decelFactor (Deceleration/Braking Smoothness):
 * - Range: 0.05 to 0.20
 * - Lower = Smoother, coasting deceleration
 * - Higher = More aggressive braking
 * - Recommended starting points:
 *   - Sporty/racing: 0.15 - 0.20 (quick stops)
 *   - Crawler/scale: 0.08 - 0.12 (smooth)
 *   - General use: 0.10 - 0.15
 * 
 * startBoost (Low-Speed Power Boost):
 * - Range: 0 to 100
 * - Helps overcome initial motor friction
 * - Higher values for:
 *   - Heavy vehicles
 *   - High gear ratios
 *   - Sticky surfaces
 * - Lower values for:
 *   - Light vehicles
 *   - Low gear ratios (fast)
 *   - Smooth surfaces
 * - Recommended starting points:
 *   - Light/fast: 20 - 30
 *   - Medium: 30 - 40
 *   - Heavy/crawler: 40 - 60
 * 
 * TUNING PROCESS:
 * 
 * 1. Start with conservative values (accel=0.15, decel=0.10, boost=33)
 * 2. Test acceleration:
 *    - Too slow/sluggish? Increase accelFactor by 0.03
 *    - Too jerky/wheelspin? Decrease accelFactor by 0.03
 * 3. Test deceleration:
 *    - Too abrupt? Decrease decelFactor by 0.02
 *    - Takes too long to stop? Increase decelFactor by 0.02
 * 4. Test low-speed movement:
 *    - Doesn't start smoothly? Increase startBoost by 5-10
 *    - Jerks forward? Decrease startBoost by 5-10
 * 5. Fine-tune in small increments (0.01-0.02) until perfect
 * 
 * EXAMPLES BY VEHICLE TYPE:
 * 
 * Racing Buggy (light, fast):
 *   accelFactor = 0.25 (aggressive)
 *   decelFactor = 0.15 (quick stops)
 *   startBoost = 25 (light vehicle)
 * 
 * Rock Crawler (heavy, torquey):
 *   accelFactor = 0.12 (smooth, controlled)
 *   decelFactor = 0.08 (gentle)
 *   startBoost = 50 (overcome weight/friction)
 * 
 * Scale Truck (moderate):
 *   accelFactor = 0.15 (realistic)
 *   decelFactor = 0.10 (smooth)
 *   startBoost = 35 (balanced)
 * 
 * Drift Car (light, responsive):
 *   accelFactor = 0.22 (quick response)
 *   decelFactor = 0.18 (control slides)
 *   startBoost = 30 (easy wheelspin)
 */

#endif // MODEL_VARIABLES_H
