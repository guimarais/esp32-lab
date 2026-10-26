# web_server

WiFi station with HTTP LED controls

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/web_server
uv run pio run -d projects/embedded/web_server -t upload
uv run pio device monitor -d projects/embedded/web_server
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).
