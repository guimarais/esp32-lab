# esp32-lab

Monorepo of ESP32 experiments. Each firmware project is a standalone PlatformIO project.

## Layout

```
projects/<name>/   PlatformIO project (platformio.ini, src/, include/, lib/, test/)
shared/lib/        Libraries shared across projects (via lib_extra_dirs)
tools/             Python helper scripts (managed with uv)
docs/              Notes, wiring diagrams, datasheets
```

## Usage

```bash
uv sync                                  # install Python tooling (incl. platformio)
uv run pio run -d projects/blink         # build
uv run pio run -d projects/blink -t upload
uv run pio device monitor -d projects/blink
```

In VS Code, install the PlatformIO IDE extension and open a project folder
(`File > Add Folder to Workspace` for each `projects/<name>`), or open the repo root
and use `pio` from the terminal.

## New project

```bash
uv run pio project init -d projects/<name> -b esp32dev -O framework=arduino
```

## Upload

```bash
uv run pio run -d projects/<name> -t upload
```

