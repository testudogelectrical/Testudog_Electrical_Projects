#include <WiFi.h>
#include <Network.h>

NetworkServer server(5000);
NetworkClient client;

const int LED_PIN = 23;

const unsigned long HEARTBEAT_TIMEOUT = 500;

unsigned long lastHeartbeat = 0;

bool armed = false;
bool heartbeatSeen = false;

void turnOff() {
  digitalWrite(LED_PIN, LOW);
  armed = false;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  turnOff();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-Network", "esp32pass");

  server.begin();

  Serial.print("Receiver IP: ");
  Serial.println(WiFi.softAPIP());

  Serial.println("Waiting for transmitter...");
}

void loop() {

  // Handle disconnected transmitter
  if (!client || !client.connected()) {

    if (armed) {
      Serial.println("Connection lost: OFF");
    }

    turnOff();
    heartbeatSeen = false;

    NetworkClient newClient = server.accept();

    if (newClient) {
      client = newClient;
      Serial.println("Transmitter connected!");
      lastHeartbeat = millis();
    }
  }

  // Read incoming data
  if (client && client.connected()) {

    while (client.available()) {

      char command = client.read();

      if (command == 'H') {
        lastHeartbeat = millis();
        heartbeatSeen = true;
      }

      else if (command == '1') {

        if (heartbeatSeen &&
            millis() - lastHeartbeat <= HEARTBEAT_TIMEOUT) {

          armed = true;
          digitalWrite(LED_PIN, HIGH);
          Serial.println("Received: ON");
        }
        else {
          Serial.println("ON rejected: No heartbeat");
        }
      }

      else if (command == '0') {
        turnOff();
        Serial.println("Received: OFF");
      }
    }

    // Check for heartbeat timeout
    if (heartbeatSeen &&
        millis() - lastHeartbeat > HEARTBEAT_TIMEOUT) {

      if (armed) {
        Serial.println("HEARTBEAT LOST! Turning OFF!");
      }

      turnOff();
      heartbeatSeen = false;
      client.stop();
    }
  }

  delay(5);
}

