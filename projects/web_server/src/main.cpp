#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"

namespace {
constexpr uint8_t LED_PIN_21 = 21;
constexpr uint8_t LED_PIN_22 = 22;
constexpr unsigned long CLIENT_TIMEOUT_MS = 2000;

WiFiServer server(80);
bool led16On = false;
bool led17On = false;

void sendPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println("<style>html{font-family:monospace;text-align:center}"
                 ".button{background:yellowgreen;border:0;color:white;padding:16px 40px;"
                 "text-decoration:none;font-size:32px;margin:2px;cursor:pointer}"
                 ".off{background:gray}</style></head><body>");
  client.println("<h1>ESP32 Web Server</h1><p>Control LED state</p>");
  client.printf("<p>GPIO 16: <a href=\"/16/%s\"><button class=\"button%s\">%s</button></a></p>\n",
                led16On ? "off" : "on", led16On ? " off" : "",
                led16On ? "OFF" : "ON");
  client.printf("<p>GPIO 17: <a href=\"/17/%s\"><button class=\"button%s\">%s</button></a></p>\n",
                led17On ? "off" : "on", led17On ? " off" : "",
                led17On ? "OFF" : "ON");
  client.println("</body></html>");
}

void handleRequest(const String &requestLine) {
  Serial.print("HTTP request: ");
  Serial.println(requestLine);

  if (requestLine.startsWith("GET /16/on ")) {
    led16On = true;
    digitalWrite(LED_PIN_21, HIGH);
    Serial.println("GPIO 16 LED turned on");
  } else if (requestLine.startsWith("GET /16/off ")) {
    led16On = false;
    digitalWrite(LED_PIN_21, LOW);
    Serial.println("GPIO 16 LED turned off");
  } else if (requestLine.startsWith("GET /17/on ")) {
    led17On = true;
    digitalWrite(LED_PIN_22, HIGH);
    Serial.println("GPIO 17 LED turned on");
  } else if (requestLine.startsWith("GET /17/off ")) {
    led17On = false;
    digitalWrite(LED_PIN_22, LOW);
    Serial.println("GPIO 17 LED turned off");
  } else {
    Serial.println("Request did not match a LED control route");
  }
}

void handleClient(WiFiClient &client) {
  const unsigned long startedAt = millis();
  String requestLine;
  bool requestLineRead = false;
  bool requestHandled = false;

  while (client.connected() && millis() - startedAt < CLIENT_TIMEOUT_MS) {
    if (!client.available()) {
      delay(1);
      continue;
    }

    const char c = client.read();
    if (c == '\n') {
      if (requestLineRead) {
        handleRequest(requestLine);
        requestHandled = true;
        sendPage(client);
        return;
      }
      requestLineRead = true;
    } else if (c != '\r' && !requestLineRead && requestLine.length() < 128) {
      requestLine += c;
    }
  }

  if (!requestHandled) {
    Serial.println("Client request timed out before completion");
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN_21, OUTPUT);
  pinMode(LED_PIN_22, OUTPUT);
  digitalWrite(LED_PIN_21, LOW);
  digitalWrite(LED_PIN_22, LOW);

  Serial.println("Connecting to WiFi network...");
  Serial.print("Network name: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  Serial.println("WiFi mode set to station");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected successfully");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting HTTP server on port 80...");
  server.begin();
  Serial.println("HTTP server started; open the ESP32 IP above on a device on the same network.");
}

void loop() {
  static unsigned long lastIpPrint = 0;
  const unsigned long now = millis();
  if (now - lastIpPrint >= 5000) {
    Serial.print("ESP32 IP address: ");
    Serial.println(WiFi.localIP());
    lastIpPrint = now;
  }

  WiFiClient client = server.available();
  if (!client) return;

  Serial.print("HTTP client connected from ");
  Serial.println(client.remoteIP());
  handleClient(client);
  client.stop();
  Serial.println("HTTP client disconnected");
}
