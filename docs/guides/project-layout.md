# Project layout conventions

- Put microcontroller and firmware projects in `projects/embedded/`.
- Put Raspberry Pi software and deployment projects in `projects/raspberry-pi/`.
- Put standalone KiCad designs in `projects/circuits/`.
- Keep a project's source, README, documentation, and related hardware files together. For a PCB belonging to firmware, use that project's `hardware/` directory.
- Keep reusable code or hardware in `shared/` only after more than one project uses it.
- Commit KiCad project, schematic, board, and custom library source. Do not commit local project settings, lock files, caches, or backups. Retain fabrication outputs when they are intentional release deliverables and explain them in the project README.

## Project README

Each project README should cover its purpose and status, hardware and power needs, setup/build/run or fabrication steps, and links to project-specific references. State what has been tested on real hardware when that is known.
