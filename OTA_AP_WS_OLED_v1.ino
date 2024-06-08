#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Wire.h>
#include <SSD1306Wire.h>

const char* apSsid = "esp8266oLED";
const char* apPassword = "potpot888";
const char* ssid = "ATHENA";
const char* password = "Achille$3";

const int ledPin = D0;  // GPIO pin for the built-in LED
bool ledState = false; // Initial LED state

SSD1306Wire display(0x3C, D5, D6); // OLED display object

ESP8266WebServer server(80);  // Create a web server object on port 80

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Hello from ESP8266 OLED!</h1>";
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
    display.println("Starting file upload...");
    if (upload.filename != "/") {
      fs::FS &fs = SPIFFS;
      fs.remove(upload.filename); // Delete the existing file, if any
      File file = fs.open(upload.filename, "w");
      if (!file) {
        display.println("Failed to create file");
        return;
      }
      file.close();
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    display.print(".");
    if (upload.filename != "/") {
      fs::FS &fs = SPIFFS;
      File file = fs.open(upload.filename, "a");
      if (file) {
        file.write(upload.buf, upload.currentSize);
        file.close();
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    display.println("File upload finished!");
    if (upload.filename != "/") {
      server.sendHeader("Location", "/"); // Redirect back to the root URL
      server.send(303);
      ESP.restart();
    }
  }
}

void setup() {
  // Initialize the OLED display
  display.init();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "AP & WS with OTA");
  display.display();

  // Access Point (AP) configuration
  WiFi.mode(WIFI_AP);  // Set the ESP8266 to Access Point mode
  WiFi.softAP(apSsid, apPassword);

  // Wait for 1 minute to check for connection
  unsigned long startTime = millis();
  while (WiFi.softAPgetStationNum() == 0 && millis() - startTime < 60000) {
    delay(500);
  }

  // If no connection within 1 minute, switch to Web Server mode
  if (WiFi.softAPgetStationNum() == 0) {
    display.println("No connection detected within 1 minute. Switching to Web Server mode.");

    // Wi-Fi station (client) configuration
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      display.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
  } else {
    display.println("Access Point mode activated.");
  }

  display.println("Access Point Ready");
  display.display();

  // OTA (Over-The-Air) update configuration
  ArduinoOTA.onStart([]() {
    display.println("OTA Update: Start");
    display.display();
  });

  ArduinoOTA.onEnd([]() {
    display.println("\nOTA Update: End");
    display.display();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
    display.display();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    display.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) display.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) display.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) display.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) display.println("Receive Failed");
    else if (error == OTA_END_ERROR) display.println("End Failed");
    display.display();
  });

  ArduinoOTA.begin();

  // Web server configuration
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/update", HTTP_POST, handleFileUpload);

  server.begin();

  display.println("Web Server Started");
  display.display();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}