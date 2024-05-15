#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeIn = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);  // Set the CE and CSN pins appropriately

// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
};

MyData data;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setAutoAck(true);  // Enable auto-acknowledgement
  radio.enableAckPayload();  // Enable acknowledgement payload
  radio.setDataRate(RF24_250KBPS);

  radio.openReadingPipe(1, pipeIn);
  radio.startListening();

  Serial.println("Receiver Initialized");
}

void loop() {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&data, sizeof(MyData));
    }

    // Print received data
    Serial.print("Throttle: ");
    Serial.print(data.throttle);
    Serial.print(", Yaw: ");
    Serial.println(data.yaw);
    //Serial.print(", Pitch: ");
    //Serial.print(data.pitch);
    //Serial.print(", Roll: ");
    //Serial.println(data.roll);
  }
}