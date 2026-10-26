# two_relays_maerklin

two relays maerklin

## Hardware

- Board: ESP32 DevKit
- Framework: Arduino via PlatformIO

## Build and upload

From the repository root:

```sh
uv run pio run -d projects/embedded/two_relays_maerklin
uv run pio run -d projects/embedded/two_relays_maerklin -t upload
uv run pio device monitor -d projects/embedded/two_relays_maerklin
```

## Project notes

Add wiring, setup details, and project-specific images in this directory. Link shared board references from [the docs index](../../../docs/index.md).

See the [Märklin controller research](docs/marklin-control-research.md) for the current design investigation and unresolved electrical checks.
