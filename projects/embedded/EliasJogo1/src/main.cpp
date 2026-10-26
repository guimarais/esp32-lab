#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"

namespace {
constexpr uint8_t GREEN_LED_PIN = 21;
constexpr uint8_t YELLOW_LED_PIN = 22;
constexpr uint8_t RED_LED_PIN = 23;
constexpr unsigned long CLIENT_TIMEOUT_MS = 2000;

WiFiServer server(80);
bool greenLedOn = false;
bool yellowLedOn = false;
bool redLedOn = false;

void sendLedControl(WiFiClient &client, const char *color, uint8_t pin,
                    const char *colorName, bool isOn) {
  client.printf(
      "<a class=\"led-link\" href=\"/%u/%s\" aria-label=\"%s LED, %s\">"
      "<span class=\"led %s %s\"></span></a>\n",
      pin, isOn ? "off" : "on", color, isOn ? "on" : "off", colorName,
      isOn ? "on" : "off");
}

void sendPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println(
      "<style>body{font-family:monospace;text-align:center;margin:2rem auto;"
      "max-width:560px;background:#f4f4f4;color:#222}.leds{display:flex;"
      "justify-content:center;align-items:center;gap:2rem;margin:4rem 1rem}"
      ".led-link{display:block;padding:12px;border-radius:50%;line-height:0}"
      ".led{display:block;width:72px;height:72px;border-radius:50%;"
      "transition:filter .15s,box-shadow .15s}.green{background:#00b341}"
      ".yellow{background:#ffd400}.red{background:#ed2024}"
      ".led.off{filter:brightness(.35);box-shadow:inset 0 0 10px #0008}"
      ".led.on{filter:brightness(1.25);box-shadow:0 0 24px currentColor}"
      ".green.on{color:#00b341}.yellow.on{color:#ffd400}.red.on{color:#ed2024}"
      ".led-link:focus-visible{outline:3px solid #222;outline-offset:4px}"
      "</style></head><body>");
  client.println("<div class=\"leds\">");
  sendLedControl(client, "Green", GREEN_LED_PIN, "green", greenLedOn);
  sendLedControl(client, "Yellow", YELLOW_LED_PIN, "yellow", yellowLedOn);
  sendLedControl(client, "Red", RED_LED_PIN, "red", redLedOn);
  client.println("</div>");
  client.println("</body></html>");
}

void handleRequest(const String &requestLine) {
  Serial.print("HTTP request: ");
  Serial.println(requestLine);

  if (requestLine.startsWith("GET /21/on ")) {
    greenLedOn = true;
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else if (requestLine.startsWith("GET /21/off ")) {
    greenLedOn = false;
    digitalWrite(GREEN_LED_PIN, LOW);
  } else if (requestLine.startsWith("GET /22/on ")) {
    yellowLedOn = true;
    digitalWrite(YELLOW_LED_PIN, HIGH);
  } else if (requestLine.startsWith("GET /22/off ")) {
    yellowLedOn = false;
    digitalWrite(YELLOW_LED_PIN, LOW);
  } else if (requestLine.startsWith("GET /23/on ")) {
    redLedOn = true;
    digitalWrite(RED_LED_PIN, HIGH);
  } else if (requestLine.startsWith("GET /23/off ")) {
    redLedOn = false;
    digitalWrite(RED_LED_PIN, LOW);
  }
}

void handleClient(WiFiClient &client) {
  const unsigned long startedAt = millis();
  String requestLine;
  bool requestLineRead = false;

  while (client.connected() && millis() - startedAt < CLIENT_TIMEOUT_MS) {
    if (!client.available()) {
      delay(1);
      continue;
    }

    const char c = client.read();
    if (c == '\n') {
      if (requestLineRead) {
        handleRequest(requestLine);
        sendPage(client);
        return;
      }
      requestLineRead = true;
    } else if (c != '\r' && !requestLineRead && requestLine.length() < 128) {
      requestLine += c;
    }
  }

  Serial.println("Client request timed out before completion");
}
}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  Serial.println("Connecting to WiFi network...");
  Serial.print("Network name: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected successfully");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP server started on port 80");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.print("HTTP client connected from ");
  Serial.println(client.remoteIP());
  handleClient(client);
  client.stop();
}
