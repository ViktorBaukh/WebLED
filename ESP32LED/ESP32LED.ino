
#include <FastLED.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 5
#define NUM_LEDS 100
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

BLECharacteristic *ledControlCharacteristic;
bool deviceConnected = false;  // Track connection status

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
        for (int i=0; i<10; i++)
        {
          leds[0] = CRGB(0,0,0);
          FastLED.show();
          delay(200);
          leds[0] = CRGB(100,0,0);
          FastLED.show();
          delay(200);
        }
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
    }
};


class LEDControlCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = std::string(pCharacteristic->getValue().c_str());


        for (int i=0; i<10; i++)
        {
        leds[0] = CRGB(0,0,100);
        FastLED.show();
        delay(200);
        leds[0] = CRGB(0,0,0);
        FastLED.show();
        delay(200);
        }
        // Debugging: Check the received data length
        Serial.print("Received data length: ");
        Serial.println(value.length());
          Serial.println(value[0]);
          Serial.println(value[1]);
          Serial.println(value[2]);
          Serial.println(value[3]);
        // Check if we received the expected data size (400 bytes)
        if (value.length() == NUM_LEDS * 4) {
            //for (int i = 0; i < NUM_LEDS; i++) {
            for (int i = 0; i < 5; i++) {
                // Read RGB and on/off status for each LED
                int r = static_cast<uint8_t>(value[i * 4]);
                int g = static_cast<uint8_t>(value[i * 4 + 1]);
                int b = static_cast<uint8_t>(value[i * 4 + 2]);
                bool on = static_cast<uint8_t>(value[i * 4 + 3]) == 1;
                
                // Debugging: Print the color and status for each LED
                Serial.print("LED ");
                Serial.print(i);
                Serial.print(" - R: ");
                Serial.print(r);
                Serial.print(", G: ");
                Serial.print(g);
                Serial.print(", B: ");
                Serial.print(b);
                Serial.print(", On: ");
                Serial.println(on);

                // Set the LED color or turn it off
                leds[i] = on ? CRGB(r, g, b) : CRGB::Black;
            }
            Serial.println("Showing");
            FastLED.show();  // Update the LED matrix with the new colors
        } else {
            Serial.println("Unexpected data length received!");
        }
    }
};

void setup() {
    Serial.begin(115200);  // Initialize serial communication for debugging
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,0,0);

    }
    FastLED.show();
    Serial.println("Black");
    delay(1000);
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(i,0,0);
      delay(10);
      FastLED.show();
    }
  
    Serial.println("Red");
    delay(1000);

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,i,0);
      delay(10);
      FastLED.show();
    }
  
    Serial.println("Green");
    delay(1000);


    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,0,i);
      delay(10);
      FastLED.show();
    }
  
    Serial.println("Blue");
    delay(1000);



    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,0,0);

    }
    FastLED.show();
    delay(1000);
    leds[0] = CRGB(100,0,0);
    leds[1] = CRGB(0,100,0);
    leds[2] = CRGB(0,0,100);
    leds[3] = CRGB(100,100,0);
    leds[4] = CRGB(100,0,100);
    leds[5] = CRGB(0,100,100);
    leds[6] = CRGB(50,50,50);

    FastLED.show();


  BLEDevice::init("ESP32_LED_Controller");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());  // Set connection status callbacks

    BLEService *pService = pServer->createService("12345678-1234-1234-1234-123456789abc");
    ledControlCharacteristic = pService->createCharacteristic(
                               "abcd1234-1234-1234-1234-123456789abc",
                               BLECharacteristic::PROPERTY_WRITE);
    ledControlCharacteristic->setCallbacks(new LEDControlCallback());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();
}

void loop() {
    // Continuously update the LED matrix if any changes occur
  
    // FastLED.show();
    // delay(500);

    // Keep the FastLED display updated if connected
    if (deviceConnected) {
      //  FastLED.show();
    }
    else
    {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,0,0);

    }
      FastLED.show();
      delay(500);
      leds[0] = CRGB(0,100,0);
      FastLED.show();
      delay(500);
    } 
}


