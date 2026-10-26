# relay_1

relay 1

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/relay_1
uv run pio run -d projects/embedded/relay_1 -t upload
uv run pio device monitor -d projects/embedded/relay_1
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).
