#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const char* ssid = "ATHENA";
const char* password = "Achille$3";
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

//RF24 radio(9, 10); // Arduino Uno, Pro Mini, Nano, LGT8F328P Boards
RF24 radio(4, 2); // NodeMCU 1.0 (ESP12E), ESP8266

// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
};

MyData data;

unsigned long previousOTACheck = 0;
const unsigned long otaCheckInterval = 1000; // 1 second

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setupOTA();

  radio.begin();
  radio.setAutoAck(true);  // Enable auto-acknowledgement
  radio.enableAckPayload();  // Enable acknowledgement payload
  radio.setDataRate(RF24_250KBPS);

  radio.openReadingPipe(0, pipeIn);
  radio.startListening();

  Serial.println("Receiver Initialized");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousOTACheck >= otaCheckInterval) {
    previousOTACheck = currentMillis;
    handleOTA();
  }

  if (radio.available()) {
    while (radio.available()) {
      radio.read(&data, sizeof(MyData));
    }

    // Print the received joystick values
    Serial.print("Throttle: ");
    Serial.print(data.throttle);
    Serial.print(", Yaw: ");
    Serial.print(data.yaw);
    Serial.print(", Pitch: ");
    Serial.print(data.pitch);
    Serial.print(", Roll: ");
    Serial.println(data.roll);

    // Process the received data as needed
    // For example, you can use the joystick values to control a servo or motor
  }

  delay(100);
}

void setupOTA() {
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
  Serial.println("OTA Initialized");
}

void handleOTA() {
  ArduinoOTA.handle();
}