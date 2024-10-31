#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <String.h>

#define LED_PIN 26 // GPIO pin for the LED

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUIDs for BLE service and characteristics
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      digitalWrite(LED_PIN, HIGH); // Optional: Turn LED on when connected
      
      // Send initial notification to indicate connection
      if (pCharacteristic) {
          pCharacteristic->setValue("CONNECTED");
          pCharacteristic->notify();
      }
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(LED_PIN, LOW); // Optional: Turn LED off when disconnected
      pServer->startAdvertising(); // Restart advertising for new connections
    }
};

class LEDControlCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        // Use Arduino String type for compatibility
        String value = pCharacteristic->getValue().c_str();
        
        if (value == "ON") {
            digitalWrite(LED_PIN, HIGH); // Turn LED on
            pCharacteristic->setValue("LED ON"); // Update characteristic value
            pCharacteristic->notify(); // Notify client of the new LED state
        } else if (value == "OFF") {
            digitalWrite(LED_PIN, LOW); // Turn LED off
            pCharacteristic->setValue("LED OFF"); // Update characteristic value
            pCharacteristic->notify(); // Notify client of the new LED state
        }
    }
};


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Create BLE Device and Server
  BLEDevice::init("ESP32_LED_Controller");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service and Characteristic with Notify Property
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setCallbacks(new LEDControlCallbacks());

  // Add a notification descriptor to enable notifications
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  // Start advertising for connections
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection...");
}

void loop() {
  // No need to add code in loop() for notifications, handled in callbacks
}
