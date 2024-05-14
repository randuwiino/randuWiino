#include <SPI.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN); // Create an instance of the RF24 object
const uint64_t address = 0xF0F0F0F0E1LL; // Address for communication

void setup() {
  Serial.begin(115200); // Initialize the serial communication
  radio.begin(); // Initialize the RF24 module
  radio.openReadingPipe(1, address); // Set the address for communication
  radio.startListening(); // Put the radio in RX mode
  radio.setAutoAck(true); // Enable auto-ACK

  // Check if a transmitter is detected
  if (!radio.isChipConnected()) {
    Serial.println("Transmitter NOT detected at initialization!");
  } else {
    Serial.println("Transmitter DETECTED at initilaization.");
  }
}

void loop() {
  if (!radio.isChipConnected()) { // Check if the transmitter is not detected
    Serial.println("Transmitter not detected!");
  }

  if (!radio.available()) { // Check if there is no data available to receive
    Serial.println("No data received.");
    delay(1000); // Delay before checking again
    return;
  }

  float humidity = 0.0; // Variable to store the received humidity value
  radio.read(&humidity, sizeof(humidity)); // Receive the humidity value

  Serial.print("Received humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(100); // Delay before checking for available data again
}