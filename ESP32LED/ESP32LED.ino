#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string> // Include this if you haven't already


#define LED_PIN 26 // GPIO pin for the LED

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

unsigned long previousMillis = 0; // Timer for periodic notifications
const long interval = 5000; // Interval to send notification (in milliseconds)

// UUIDs for BLE service and characteristics
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      digitalWrite(LED_PIN, HIGH); // Optional: Turn LED on when connected
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(LED_PIN, LOW); // Optional: Turn LED off when disconnected
    }
};

class LEDControlCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        
        String value = pCharacteristic->getValue();
        std::string stdValue = value.c_str();

        if (stdValue == "ON") {
            digitalWrite(LED_PIN, HIGH); // Turn LED on
        } else if (value == "OFF") {
            digitalWrite(LED_PIN, LOW); // Turn LED off
        }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Create BLE Device and Server
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  BLEDevice::init("ESP32_LED_Controller");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  // Create BLE Service and Characteristic
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new LEDControlCallbacks());
  // Add a notification descriptor to enable notifications
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  // Start BLE advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection...");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);


}

void loop() {
  // Add any logic for continuous operation
    // Check if a device is connected
  if (deviceConnected) {
    unsigned long currentMillis = millis();
    
    // Send a notification every 'interval' milliseconds to keep connection alive
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
      // Send notification data (e.g., status message or any value)
      pCharacteristic->setValue("KEEP_ALIVE"); // Any arbitrary message
      pCharacteristic->notify(); // Sends a notification to the client
    }
  }
}
