# ESP32 DOIT DEV KIT V1 pinout

Pin reference transcribed from [`ESP32-DOIT-DEV-KIT-v1-pinout.png`](ESP32-DOIT-DEV-KIT-v1-pinout.png). The tables list pins from the top of the board to the bottom, as shown with the USB connector at the bottom.

## Left header

| Position | GPIO / pin | Functions shown in the image |
| --- | --- | --- |
| Top | EN | Enable / reset |
| 1 | GPIO36 (VP) | Input only; RTC GPIO0; Sensor VP; ADC1_0 |
| 2 | GPIO39 (VN) | Input only; RTC GPIO3; Sensor VN; ADC1_3 |
| 3 | GPIO34 | Input only; RTC GPIO4; ADC1_6 |
| 4 | GPIO35 | Input only; RTC GPIO5; ADC1_7 |
| 5 | GPIO32 | RTC GPIO9; XTAL32P; Touch9; ADC1_4 |
| 6 | GPIO33 | RTC GPIO8; XTAL32N; Touch8; ADC1_5 |
| 7 | GPIO25 | DAC1; RTC GPIO6; ADC2_8 |
| 8 | GPIO26 | DAC2; RTC GPIO7; ADC2_9 |
| 9 | GPIO27 | RTC GPIO17; Touch7; ADC2_7 |
| 10 | GPIO14 | RTC GPIO16; Touch6; HSPI_CLK; ADC2_6 |
| 11 | GPIO12 | RTC GPIO15; Touch5; HSPI_Q; ADC2_5 |
| 12 | GPIO13 | RTC GPIO14; Touch4; HSPI_ID; ADC2_4 |
| Bottom | GND | Ground |
| Bottom | VIN | Board input supply |

## Right header

| Position | GPIO / pin | Functions shown in the image |
| --- | --- | --- |
| Top | GPIO23 | VSPI_D; MOSI |
| 1 | GPIO22 | VSPI_WP; SCL; RTS0 |
| 2 | GPIO1 | TXD0; CLK3 |
| 3 | GPIO3 | RXD0; CLK2 |
| 4 | GPIO21 | VSPI_HD; SDA |
| 5 | GPIO19 | VSPI_Q; MISO; CTS0 |
| 6 | GPIO18 | VSPI_CLK; SCK |
| 7 | GPIO5 | VSPI_CS0; SS |
| 8 | GPIO17 | TXD2 |
| 9 | GPIO16 | RXD2 |
| 10 | GPIO4 | ADC2_0; HSPI_HD; Touch0; RTC GPIO10 |
| 11 | GPIO2 | ADC2_2; HSPI_WP0; Touch2; RTC GPIO12 |
| 12 | GPIO15 | ADC2_3; HSPI_CS0; Touch3; RTC GPIO13 |
| Bottom | GND | Ground |
| Bottom | 3V3 | 3.3 V supply |

## Notes

- GPIO34, GPIO35, GPIO36, and GPIO39 are input-only, as marked in the image.
- GPIO0 is shown as an RTC GPIO (on GPIO36); this diagram does not mark boot-strapping pins. Check the ESP32/module documentation before relying on pins that affect boot mode.
- `VIN` and `3V3` are supply pins, not GPIOs. Confirm the board's input-voltage limits before powering it.
- The image uses alternate-function abbreviations such as `HSPI_ID` and `VSPI_HD`; names are retained as printed.

## Image credit

The source image is watermarked `www.mischianti.org` and displays a CC BY-NC-ND license mark. This transcription is provided as a reference to that image.
