#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t OLED_ADDRESSES[] = {0x3C, 0x3D};
constexpr uint16_t OLED_WIDTH = 128;
constexpr uint16_t OLED_HEIGHT = 64;
constexpr uint32_t DEPARTURE_INTERVAL_MS = 30000;

struct Departure {
  const char *time;
  const char *destination;
  const char *platform;
};

const Departure departures[] = {
  {"08:42", "LISBON", "2"},
  {"09:05", "PORTO", "5"},
  {"09:18", "COIMBRA", "1"},
  {"09:32", "FARO", "3"},
};
constexpr uint8_t DEPARTURE_COUNT = sizeof(departures) / sizeof(departures[0]);

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;
uint8_t oledAddress = 0;
uint8_t firstDeparture = 0;
uint32_t departureStartedAt = 0;
uint32_t lastRenderAt = 0;

void drawDepartureRow(uint8_t row, const Departure &departure, bool active) {
  const int16_t y = 18 + row * 13;

  if (active) {
    display.fillRect(0, y, OLED_WIDTH, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }

  display.setCursor(1, y + 1);
  display.print(departure.time);
  display.setCursor(36, y + 1);
  display.print(departure.destination);
  display.setCursor(82, y + 1);
  display.print(departure.platform);
  display.setCursor(98, y + 1);
  display.print(active ? "BOARD" : "ON");

  display.setTextColor(SSD1306_WHITE);
}

void drawBoard(uint32_t now) {
  display.clearDisplay();

  // Fill the full 16-row yellow zone with the inverted column header.
  display.fillRect(0, 0, OLED_WIDTH, 16, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(1, 4);
  display.print("TIME");
  display.setCursor(36, 4);
  display.print("DEST");
  display.setCursor(82, 4);
  display.print("PF");
  display.setCursor(98, 4);
  display.print("STAT");

  display.setTextColor(SSD1306_WHITE);
  display.drawFastHLine(0, 16, OLED_WIDTH, SSD1306_WHITE);

  for (uint8_t row = 0; row < 3; ++row) {
    const uint8_t index = (firstDeparture + row) % DEPARTURE_COUNT;
    drawDepartureRow(row, departures[index], row == 0);
  }

  display.drawFastHLine(0, 55, OLED_WIDTH, SSD1306_WHITE);
  const uint32_t elapsed = now - departureStartedAt;
  const uint8_t secondsLeft = (DEPARTURE_INTERVAL_MS - elapsed) / 1000;
  display.setCursor(1, 57);
  display.print("NEXT TRAIN IN 00:");
  if (secondsLeft < 10) display.print('0');
  display.print(secondsLeft);
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  for (uint8_t address : OLED_ADDRESSES) {
    if (display.begin(SSD1306_SWITCHCAPVCC, address)) {
      oledReady = true;
      oledAddress = address;
      break;
    }
  }

  if (!oledReady) {
    Serial.println("SSD1306 OLED not found at 0x3C or 0x3D. Check wiring/address.");
    return;
  }

  Serial.printf("Train departure board ready at 0x%02X\n", oledAddress);
  departureStartedAt = millis();
}

void loop() {
  if (!oledReady) {
    delay(1000);
    return;
  }

  const uint32_t now = millis();
  if (now - departureStartedAt >= DEPARTURE_INTERVAL_MS) {
    firstDeparture = (firstDeparture + 1) % DEPARTURE_COUNT;
    departureStartedAt = now;
  }

  if (now - lastRenderAt >= 250) {
    drawBoard(now);
    lastRenderAt = now;
  }
}
