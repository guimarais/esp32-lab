#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: new_project.sh <embedded|raspberry-pi|circuit> <name> [--description <text>]

Create a project scaffold in the matching projects/ category.
EOF
}

kind=""
name=""
description=""

while (($#)); do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --description)
      if (($# < 2)); then
        echo "error: --description requires text" >&2
        usage >&2
        exit 2
      fi
      description="$2"
      shift 2
      ;;
    --*)
      echo "error: unknown option: $1" >&2
      usage >&2
      exit 2
      ;;
    *)
      if [[ -z "$kind" ]]; then
        kind="$1"
      elif [[ -z "$name" ]]; then
        name="$1"
      else
        echo "error: unexpected argument: $1" >&2
        usage >&2
        exit 2
      fi
      shift
      ;;
  esac
done

case "$kind" in
  embedded) category="embedded" ;;
  raspberry-pi) category="raspberry-pi" ;;
  circuit) category="circuits" ;;
  "") echo "error: project type is required" >&2; usage >&2; exit 2 ;;
  *) echo "error: unsupported project type: $kind" >&2; usage >&2; exit 2 ;;
esac

if [[ -z "$name" ]]; then
  echo "error: project name is required" >&2
  usage >&2
  exit 2
fi
if [[ ! "$name" =~ ^[a-zA-Z0-9][a-zA-Z0-9_-]*$ ]]; then
  echo "error: project name must start with a letter or number and contain only letters, numbers, underscores, or hyphens" >&2
  exit 2
fi
if [[ "$description" == *$'\n'* || "$description" == *$'\r'* ]]; then
  echo "error: description must be a single line" >&2
  exit 2
fi

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"
project_dir="$repo_root/projects/$category/$name"
if [[ -z "$description" ]]; then
  description="${name//_/ }"
  description="${description//-/ }"
fi
if [[ -e "$project_dir" ]]; then
  echo "error: project already exists: projects/$category/$name" >&2
  exit 1
fi
if ! mkdir -p -- "$project_dir" 2>/dev/null; then
  echo "error: could not create project: projects/$category/$name" >&2
  exit 1
fi

cleanup() {
  if [[ -d "$project_dir" ]]; then
    rm -rf -- "$project_dir"
  fi
}
trap cleanup ERR

case "$kind" in
  embedded)
    mkdir -p "$project_dir/src" "$project_dir/include" "$project_dir/lib" "$project_dir/test" "$project_dir/docs"
    cat > "$project_dir/platformio.ini" <<EOF
[platformio]
description = $description

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_extra_dirs = ../../../shared/lib
EOF
    cat > "$project_dir/src/main.cpp" <<'EOF'
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
}
EOF
    touch "$project_dir/include/.gitkeep" "$project_dir/lib/.gitkeep" "$project_dir/test/.gitkeep"
    ;;
  raspberry-pi)
    mkdir -p "$project_dir/src" "$project_dir/tests" "$project_dir/deploy" "$project_dir/docs"
    touch "$project_dir/src/.gitkeep" "$project_dir/tests/.gitkeep" "$project_dir/deploy/.gitkeep"
    ;;
  circuit)
    mkdir -p "$project_dir/libraries" "$project_dir/manufacturing" "$project_dir/docs"
    touch "$project_dir/libraries/.gitkeep" "$project_dir/manufacturing/.gitkeep"
    ;;
esac

cat > "$project_dir/README.md" <<EOF
# $name

$description

## Status

In progress

## Hardware

<!-- Record boards, components, power requirements, and wiring here. -->

## Setup and use

<!-- Add project-specific setup, build, deployment, or fabrication steps here. -->

## Documentation

Keep project-specific notes and images in this directory. Link shared references from `docs/` at the repository root.
EOF

trap - ERR
printf 'Created projects/%s/%s\n' "$category" "$name"
if [[ "$kind" == embedded ]]; then
  printf 'Build with: uv run pio run -d projects/%s/%s\n' "$category" "$name"
fi
