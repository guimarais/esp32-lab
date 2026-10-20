#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: new_project.sh <name> [--description <text>]

Create a PlatformIO project scaffold under projects/.
EOF
}

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
      if [[ -n "$name" ]]; then
        echo "error: unexpected argument: $1" >&2
        usage >&2
        exit 2
      fi
      name="$1"
      shift
      ;;
  esac
done

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
project_dir="$repo_root/projects/$name"

if [[ -z "$description" ]]; then
  description="${name//_/ }"
  description="${description//-/ }"
fi

if ! mkdir -- "$project_dir" 2>/dev/null; then
  echo "error: project already exists or could not be created: projects/$name" >&2
  exit 1
fi

cleanup() {
  if [[ -d "$project_dir" ]]; then
    rm -rf -- "$project_dir"
  fi
}
trap cleanup ERR

mkdir -p "$project_dir/src" "$project_dir/include" "$project_dir/lib" "$project_dir/test"

cat > "$project_dir/platformio.ini" <<EOF
[platformio]
description = $description

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_extra_dirs = ../../shared/lib
EOF

cat > "$project_dir/src/main.cpp" <<'EOF'
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
}
EOF

touch "$project_dir/include/.gitkeep" \
  "$project_dir/lib/.gitkeep" \
  "$project_dir/test/.gitkeep"
trap - ERR

printf 'Created projects/%s\n' "$name"
printf 'Build with: uv run pio run -d projects/%s\n' "$name"
