# Electronics Lab

A collection of embedded, Raspberry Pi, and electronics design projects. Each project keeps its source, project-specific documentation, and related hardware design together.

## Repository layout

```text
projects/
  embedded/<name>/       Firmware and microcontroller projects (currently PlatformIO/ESP32)
  raspberry-pi/<name>/   Raspberry Pi software, deployment files, and project notes
  circuits/<name>/       Standalone KiCad designs
docs/
  guides/                Shared how-tos and workflows
  reference/             Board, component, and protocol references
  images/                Images used by shared documentation
shared/
  lib/                   Firmware libraries shared by multiple projects
  hardware/              Reusable KiCad libraries or hardware modules
tools/                   Project scaffolds and repository helpers
```

Projects that combine firmware and a PCB keep their KiCad files in a `hardware/` directory inside that project. Standalone board designs belong in `projects/circuits/`. Keep editable KiCad source files and project-specific libraries in version control; generated fabrication files belong in a documented release or manufacturing directory when they are intentional deliverables.

## Embedded projects

Install the development dependencies with:

```bash
uv sync
```

Build a project by its path:

```bash
uv run pio run -d projects/embedded/blink
```

To upload firmware, connect the board to your computer over USB and run PlatformIO's `upload` target. For example, this builds and uploads the OLED project:

```bash
uv run pio run -d projects/embedded/oled -t upload
```

Replace `projects/embedded/oled` with the directory of the project you want to flash. If PlatformIO cannot identify the board's serial port automatically, specify it with `--upload-port`, for example `--upload-port /dev/ttyUSB0` on Linux. After uploading, open the serial monitor with:

```bash
uv run pio device monitor -d projects/embedded/oled
```

In VS Code, install the PlatformIO IDE extension and open a project folder (`File > Add Folder to Workspace`), or open the repository root and use `pio` from the terminal.

## Create a project

```bash
./tools/new_project.sh embedded <name> [--description "Short description"]
./tools/new_project.sh raspberry-pi <name> [--description "Short description"]
./tools/new_project.sh circuit <name> [--description "Short description"]
```

The scaffold creates a starter README and directories appropriate to the project type. For KiCad designs, create the project with KiCad and save its `.kicad_pro`, `.kicad_sch`, and, when applicable, `.kicad_pcb` files in the generated circuit directory.

## Documentation

Start at [the documentation index](docs/index.md). Put project-specific instructions beside the project and shared references under `docs/`.
