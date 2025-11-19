// Minimal Bluepad32 Test - Low Power
// This tests Bluetooth without initializing servos/motors

#include <Bluepad32.h>

ControllerPtr myController = nullptr;

void onConnectedController(ControllerPtr ctl) {
  Serial.println("\n=== CONTROLLER CONNECTED! ===");
  Serial.printf("Model: %s\n", ctl->getModelName().c_str());
  myController = ctl;
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("\n=== CONTROLLER DISCONNECTED ===");
  myController = nullptr;
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial monitor

  Serial.println("\n========================================");
  Serial.println("Bluepad32 Low-Power Test");
  Serial.println("========================================");

  Serial.println("Initializing Bluepad32...");
  BP32.setup(&onConnectedController, &onDisconnectedController);

  Serial.println("✓ Ready to pair!");
  Serial.println("Put your controller in pairing mode now.");
  Serial.println("========================================\n");
}

void loop() {
  BP32.update();

  // Print controller status every 2 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();

    if (myController && myController->isConnected()) {
      Serial.printf("✓ Connected | X:%d Y:%d | Buttons:0x%04X\n",
                    myController->axisX(),
                    myController->axisY(),
                    myController->buttons());
    } else {
      Serial.println("Waiting for controller...");
    }
  }

  delay(100);
}
