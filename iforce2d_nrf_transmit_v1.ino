#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte throttle;
  byte yaw;
  //byte pitch;
  //byte roll;
};

MyData data;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setAutoAck(true);  // Enable auto-acknowledgement
  radio.enableAckPayload();  // Enable acknowledgement payload
  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(pipeOut);

  memset(&data, 0, sizeof(MyData));
  radio.write(&data, sizeof(MyData));

  Serial.println("Transmitter Initialized");
}

void loop() {
  data.throttle = map(analogRead(0), 0, 1023, 0, 255);
  data.yaw = map(analogRead(1), 0, 1023, 0, 255);
  //data.pitch = map(analogRead(2), 0, 1023, 0, 255);
  //data.roll = map(analogRead(3), 0, 1023, 0, 255);

  // Print joystick readings
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(", Yaw: ");
  Serial.println(data.yaw);
  //Serial.print(", Pitch: ");
  //Serial.print(data.pitch);
  //Serial.print(", Roll: ");
  //Serial.println(data.roll);

  // Send the payload wirelessly and wait for acknowledgement
  if (radio.write(&data, sizeof(MyData))) {
    Serial.println(" Data sent successfully");

    if (radio.isAckPayloadAvailable()) {
      byte ackPayload[1];
      radio.read(&ackPayload, sizeof(ackPayload));
      Serial.println("Acknowledgement received");
      // Process acknowledgement payload if needed
    }
  }

  delay(100);
}
