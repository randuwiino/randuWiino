#include <SPI.h>
#include <RF24.h>
#include <DHT.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const uint64_t address = 0xF0F0F0F0E1LL;  // Address for communication

DHT dht(3, DHT11);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  radio.begin();
  radio.openWritingPipe(address);  // Set the address for communication
  radio.setAutoAck(true);  // Enable auto-ACK

  // Check if the receiver is detected
  if (!radio.isChipConnected()) {
    Serial.println("Receiver not detected at initialization!");
  } else {
    Serial.println("Receiver detected.");
  }

  dht.begin();
}

void loop() {
  // Check if the receiver is not detected
  if (!radio.isChipConnected()) {
    Serial.println("Receiver not detected!");
  }

  // Read humidity
  int humidity = dht.readHumidity();

  if (humidity == 0) {
    Serial.println("Failed to read humidity data from DHT11 sensor!");
  } else {
    // Print humidity
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Transmit payload
    String payload = String(humidity);
    bool transmissionSuccess = radio.write(&payload, sizeof(payload));
    if (transmissionSuccess) {
      Serial.println("Payload transmitted successfully!");
    } else {
      Serial.println("Failed to transmit payload!");
    }
  }

  delay(5000); // Send data every 5 seconds
}