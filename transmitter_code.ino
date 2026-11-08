#include <WiFi.h>
#include <Network.h>

NetworkClient client;

unsigned long lastAttempt = 0;
unsigned long lastHeartbeat = 0;

const unsigned long HEARTBEAT_INTERVAL = 100;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin("ESP32-Network", "esp32pass");
  WiFi.setAutoReconnect(true);

  Serial.println("Enter 1 for ON or 0 for OFF.");
}

void loop() {

  // Reconnect to receiver if disconnected
  if (WiFi.status() == WL_CONNECTED &&
      !client.connected() &&
      millis() - lastAttempt >= 1000) {

    lastAttempt = millis();

    client.stop();

    if (client.connect("192.168.4.1", 5000)) {
      Serial.println("Connected to receiver!");
      lastHeartbeat = 0;
    }
  }

  // Send heartbeat every 100 ms
  if (client.connected() &&
      millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {

    client.write('H');
    lastHeartbeat = millis();
  }

  // Read ON/OFF commands from Serial Monitor
  if (Serial.available()) {

    char command = Serial.read();

    if (command == '1' || command == '0') {

      if (client.connected()) {
        client.write(command);

        Serial.print("Sent: ");
        Serial.println(command);
      }
      else {
        Serial.println("Not connected!");
      }
    }
  }
}

