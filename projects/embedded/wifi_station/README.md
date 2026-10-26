# wifi_station

WiFi station mode with HTTP LED control

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/wifi_station
uv run pio run -d projects/embedded/wifi_station -t upload
uv run pio device monitor -d projects/embedded/wifi_station
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).
