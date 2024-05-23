#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char* apSsid = "esp8266Lordi";
const char* apPassword = "potpot888";
const char* ssid = "ATHENA";
const char* password = "Achille$3";

const int ledPin = D0;  // GPIO pin for the built-in LED
bool ledState = false; // Initial LED state

ESP8266WebServer server(80);  // Create a web server object on port 80

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Hello from ESP8266!</h1>";
  html += "<p>Access Point status: " + String(WiFi.softAPgetStationNum() > 0 ? "UP" : "DOWN") + "</p>";
  html += "<p>LED state: " + String(ledState ? "ON" : "OFF") + "</p>";
  html += "<form method=\"POST\" action=\"/toggle\">";
  html += "<button type=\"submit\">" + String(ledState ? "Turn Off" : "Turn On") + "</button>";
  html += "</form>";

  html += "<h2>OTA Update</h2>";
  html += "<form method=\"POST\" action=\"/update\" enctype=\"multipart/form-data\">";
  html += "<input type=\"file\" name=\"update\">";
  html += "<input type=\"submit\" value=\"Upload\">";
  html += "</form>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Starting file upload...");
    if (upload.filename != "/") {
      fs::FS &fs = SPIFFS;
      fs.remove(upload.filename); // Delete the existing file, if any
      File file = fs.open(upload.filename, "w");
      if (!file) {
        Serial.println("Failed to create file");
        return;
      }
      file.close();
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.print(".");
    if (upload.filename != "/") {
      fs::FS &fs = SPIFFS;
      File file = fs.open(upload.filename, "a");
      if (file) {
        file.write(upload.buf, upload.currentSize);
        file.close();
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.println("File upload finished!");
    if (upload.filename != "/") {
      server.sendHeader("Location", "/"); // Redirect back to the root URL
      server.send(303);
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Initialize the LED pin

  // Access Point (AP) configuration
  WiFi.mode(WIFI_AP);  // Set the ESP8266 to Access Point mode
  WiFi.softAP(apSsid, apPassword);

  // Wait until the AP is ready
  while (WiFi.softAPgetStationNum() == 0) {
    delay(500);
  }

  Serial.println("Access Point Ready");

  // OTA (Over-the-Air) update configuration
  ArduinoOTA.setHostname("esp8266");

  // Set the OTA password (optional)
  // ArduinoOTA.setPassword("YourPassword");

  // Set the OTA port (optional, default: 8266)
  // ArduinoOTA.setPort(8266);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  // Initialize the file system
  SPIFFS.begin();

  // Wi-Fi station (client) configuration
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Start OTA
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/update", HTTP_POST, handleFileUpload);

  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  // LED heartbeat blink
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}