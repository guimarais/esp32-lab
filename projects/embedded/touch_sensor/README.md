# touch_sensor

touch sensor

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/touch_sensor
uv run pio run -d projects/embedded/touch_sensor -t upload
uv run pio device monitor -d projects/embedded/touch_sensor
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).
