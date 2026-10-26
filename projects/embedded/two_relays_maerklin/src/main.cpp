#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "secrets.h"

namespace {
constexpr uint8_t RELAY_1_PIN = 15;
constexpr uint8_t RELAY_2_PIN = 2;
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t OLED_ADDRESSES[] = {0x3C, 0x3D};
constexpr uint16_t OLED_WIDTH = 128;
constexpr uint16_t OLED_HEIGHT = 64;
constexpr uint32_t RELAY_PULSE_MS = 200;
constexpr uint32_t CLIENT_TIMEOUT_MS = 2000;

enum class TurnoutPosition : uint8_t { Unknown, Relay1, Relay2 };

WiFiServer server(80);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;
uint8_t oledAddress = 0;
bool relayBusy = false;
uint8_t activeRelayPin = 0;
uint32_t relayStartedAt = 0;
TurnoutPosition turnoutPosition = TurnoutPosition::Unknown;
String requestMessage = "Ready";
uint32_t lastDisplayUpdate = 0;

void renderDisplay(uint32_t now) {
  if (!oledReady || now - lastDisplayUpdate < 100) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("MAERKLIN TURNOUT");
  display.drawFastHLine(0, 11, OLED_WIDTH, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 20);
  if (turnoutPosition == TurnoutPosition::Relay1) {
    display.print("POSITION 1");
  } else if (turnoutPosition == TurnoutPosition::Relay2) {
    display.print("POSITION 2");
  } else {
    display.print("POS: UNKN.");
  }

  display.setTextSize(1);
  display.setCursor(0, 47);
  if (relayBusy) {
    display.printf("PULSE: GPIO %u", activeRelayPin);
  } else {
    display.print("PULSE: IDLE");
  }
  display.setCursor(0, 57);
  display.print(requestMessage);
  display.display();
  lastDisplayUpdate = now;
}

void updateRelay(uint32_t now) {
  if (relayBusy && now - relayStartedAt >= RELAY_PULSE_MS) {
    digitalWrite(activeRelayPin, LOW);
    relayBusy = false;
    activeRelayPin = 0;
    requestMessage = "Ready";
    Serial.println("Relay pulse complete");
  }
}

bool activatePosition(TurnoutPosition position) {
  if (relayBusy) {
    requestMessage = "Busy - command ignored";
    Serial.println("Turnout command ignored while relay pulse is active");
    return false;
  }

  if (turnoutPosition == position) {
    requestMessage = position == TurnoutPosition::Relay1
                         ? "Already at position 1"
                         : "Already at position 2";
    Serial.println(requestMessage);
    return false;
  }

  const uint8_t pin = position == TurnoutPosition::Relay1 ? RELAY_1_PIN : RELAY_2_PIN;
  turnoutPosition = position;
  activeRelayPin = pin;
  relayStartedAt = millis();
  relayBusy = true;
  requestMessage = position == TurnoutPosition::Relay1 ? "Moving to position 1" : "Moving to position 2";
  digitalWrite(pin, HIGH);
  Serial.printf("Position %u selected; GPIO %u active for %lu ms\n",
                position == TurnoutPosition::Relay1 ? 1 : 2, pin,
                static_cast<unsigned long>(RELAY_PULSE_MS));
  return true;
}

void sendPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\"><title>Maerklin Turnout</title>");
  client.println("<style>body{font:18px monospace;text-align:center;margin:2em auto;max-width:32em;color:#222}"
                 ".button{display:inline-block;background:#287a48;color:white;padding:16px 22px;"
                 "margin:8px;border-radius:6px;text-decoration:none}.status{padding:12px;background:#eee;border-radius:6px}</style></head><body>");
  client.println("<h1>Turnout control</h1>");
  client.printf("<p class=\"status\">Position: <strong>%s</strong><br>Relay: %s</p>\n",
                turnoutPosition == TurnoutPosition::Relay1 ? "1" :
                turnoutPosition == TurnoutPosition::Relay2 ? "2" : "unknown",
                relayBusy ? "pulsing" : "idle");
  client.println("<p><a class=\"button\" href=\"/position/1\">Position 1</a>");
  client.println("<a class=\"button\" href=\"/position/2\">Position 2</a></p>");
  client.printf("<p>%s</p><p>Each command pulses its relay for 200 ms. Commands during a pulse are ignored.</p>\n",
                requestMessage.c_str());
  client.println("</body></html>");
}

void handleRequest(const String &requestLine) {
  Serial.print("HTTP request: ");
  Serial.println(requestLine);

  if (requestLine.startsWith("GET /position/1 ")) {
    activatePosition(TurnoutPosition::Relay1);
  } else if (requestLine.startsWith("GET /position/2 ")) {
    activatePosition(TurnoutPosition::Relay2);
  }
}

void handleClient(WiFiClient &client) {
  const uint32_t startedAt = millis();
  String requestLine;
  bool requestLineRead = false;

  while (client.connected() && millis() - startedAt < CLIENT_TIMEOUT_MS) {
    updateRelay(millis());
    renderDisplay(millis());
    if (!client.available()) {
      delay(1);
      continue;
    }

    const char c = client.read();
    if (c == '\n') {
      if (!requestLineRead) {
        requestLineRead = true;
        handleRequest(requestLine);
      } else {
        sendPage(client);
        return;
      }
    } else if (c != '\r' && !requestLineRead && requestLine.length() < 128) {
      requestLine += c;
    }
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);

  // Set inactive output levels before enabling output mode to avoid a startup pulse.
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  for (uint8_t address : OLED_ADDRESSES) {
    if (display.begin(SSD1306_SWITCHCAPVCC, address)) {
      oledReady = true;
      oledAddress = address;
      break;
    }
  }
  if (oledReady) {
    Serial.printf("SSD1306 OLED ready at 0x%02X\n", oledAddress);
  } else {
    Serial.println("SSD1306 OLED not found at 0x3C or 0x3D; continuing without display");
  }

  Serial.printf("Connecting to WiFi network %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.printf("\nWiFi connected; open http://%s/\n", WiFi.localIP().toString().c_str());
  server.begin();
}

void loop() {
  const uint32_t now = millis();
  updateRelay(now);
  renderDisplay(now);

  WiFiClient client = server.available();
  if (!client) return;

  handleClient(client);
  client.stop();
}
