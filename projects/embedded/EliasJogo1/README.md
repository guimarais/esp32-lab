# EliasJogo1

WiFi webpage for controlling three LEDs

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/EliasJogo1
uv run pio run -d projects/embedded/EliasJogo1 -t upload
uv run pio device monitor -d projects/embedded/EliasJogo1
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).
