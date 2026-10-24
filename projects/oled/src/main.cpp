#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t OLED_ADDRESSES[] = {0x3C, 0x3D};
constexpr uint16_t OLED_WIDTH = 128;
constexpr uint16_t OLED_HEIGHT = 64;
constexpr uint8_t STAR_COUNT = 22;
constexpr uint8_t FRAME_DELAY_MS = 35;

struct Star {
  int16_t x;
  int16_t y;
  uint8_t speed;
  uint8_t twinkleOffset;
};

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
Star stars[STAR_COUNT];
bool oledReady = false;
uint8_t oledAddress = 0;
uint32_t lastSerialUpdate = 0;

void initializeStars() {
  randomSeed(esp_random());
  for (uint8_t i = 0; i < STAR_COUNT; ++i) {
    stars[i] = {
      static_cast<int16_t>(random(0, OLED_WIDTH)),
      static_cast<int16_t>(random(15, 52)),
      static_cast<uint8_t>(random(1, 4)),
      static_cast<uint8_t>(random(0, 32))
    };
  }
}

void drawStars(uint32_t frame) {
  for (uint8_t i = 0; i < STAR_COUNT; ++i) {
    Star &star = stars[i];
    const bool bright = ((frame / 8 + star.twinkleOffset) % 7) < 3;
    if (bright) {
      display.drawPixel(star.x, star.y, SSD1306_WHITE);
    }

    // A few stars pulse between a dot and a tiny cross.
    if (bright && i % 5 == 0) {
      display.drawPixel(star.x - 1, star.y, SSD1306_WHITE);
      display.drawPixel(star.x + 1, star.y, SSD1306_WHITE);
      display.drawPixel(star.x, star.y - 1, SSD1306_WHITE);
      display.drawPixel(star.x, star.y + 1, SSD1306_WHITE);
    }

    star.x -= star.speed;
    if (star.x < 0) {
      star.x = OLED_WIDTH - 1;
      star.y = random(15, 52);
    }
  }
}

void drawOrbitingSatellite(uint32_t frame) {
  const float angle = frame * 0.045f;
  const int16_t x = 64 + static_cast<int16_t>(cosf(angle) * 35.0f);
  const int16_t y = 34 + static_cast<int16_t>(sinf(angle) * 15.0f);

  // Two solar panels and the satellite body.
  display.drawRect(x - 5, y - 2, 4, 5, SSD1306_WHITE);
  display.drawRect(x + 2, y - 2, 4, 5, SSD1306_WHITE);
  display.fillRect(x - 1, y - 3, 3, 7, SSD1306_WHITE);
  display.drawPixel(x, y - 5, SSD1306_WHITE);
}

void drawPlanet(uint32_t frame) {
  // Dotted orbital path.
  for (uint8_t step = 0; step < 64; ++step) {
    const float angle = step * 0.09817f;
    const int16_t x = 64 + static_cast<int16_t>(cosf(angle) * 35.0f);
    const int16_t y = 34 + static_cast<int16_t>(sinf(angle) * 15.0f);
    display.drawPixel(x, y, SSD1306_WHITE);
  }

  // Planet with a moving highlight and two craters.
  display.fillCircle(64, 34, 10, SSD1306_WHITE);
  display.fillCircle(61, 31, 2, SSD1306_BLACK);
  display.fillCircle(68, 36, 3, SSD1306_BLACK);
  const int16_t highlightX = 64 + static_cast<int16_t>(cosf(frame * 0.025f) * 5.0f);
  display.drawPixel(highlightX, 28, SSD1306_BLACK);
}

void drawInterface(uint32_t frame) {
  display.clearDisplay();
  drawStars(frame);

  // Minimal instrument-panel frame.
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 2);
  display.print("ESP32");
  display.setCursor(60, 2);
  display.print("DEEP SPACE");
  display.drawFastHLine(4, 11, 120, SSD1306_WHITE);
  display.drawFastHLine(4, 55, 120, SSD1306_WHITE);
  display.drawPixel(2, 11, SSD1306_WHITE);
  display.drawPixel(125, 11, SSD1306_WHITE);

  drawPlanet(frame);
  drawOrbitingSatellite(frame);

  // A small animated status beacon in the footer.
  display.fillCircle(8, 60, (frame / 10) % 2 ? 2 : 1, SSD1306_WHITE);
  display.setCursor(15, 57);
  display.print("ORBITAL SYSTEM");
  display.setCursor(99, 57);
  display.print("LIVE");

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

  Serial.printf("OLED showcase ready at 0x%02X\n", oledAddress);
  initializeStars();
}

void loop() {
  if (!oledReady) {
    delay(1000);
    return;
  }

  const uint32_t frame = millis() / FRAME_DELAY_MS;
  drawInterface(frame);

  if (millis() - lastSerialUpdate >= 5000) {
    Serial.printf("OLED address: 0x%02X | animation running\n", oledAddress);
    lastSerialUpdate = millis();
  }

  delay(FRAME_DELAY_MS);
}
