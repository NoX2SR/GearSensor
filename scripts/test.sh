#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/gear-sensor-tests.XXXXXX")"
trap 'rm -rf "${BUILD_DIR}"' EXIT

COMMON=(-std=c++11 -Wall -Wextra -Werror -O0 -g --coverage
  -I "${ROOT_DIR}/tests/fakes"
  -I "${ROOT_DIR}/Libs/GearSensor")

g++ "${COMMON[@]}" \
  "${ROOT_DIR}/tests/fakes/Arduino.cpp" \
  "${ROOT_DIR}/Libs/GearSensor/GearSensor.cpp" \
  "${ROOT_DIR}/tests/test_gear_sensor.cpp" \
  -o "${BUILD_DIR}/test_gear_sensor"

g++ "${COMMON[@]}" \
  "${ROOT_DIR}/tests/fakes/Arduino.cpp" \
  "${ROOT_DIR}/Libs/GearSensor/GearSensor.cpp" \
  "${ROOT_DIR}/tests/test_gear_sketch.cpp" \
  -o "${BUILD_DIR}/test_gear_sketch"

for test_binary in test_gear_sensor test_gear_sketch; do
  "${BUILD_DIR}/${test_binary}"
  echo "PASS ${test_binary}"
done

coverage_line() {
  local object="$1"
  local source="$2"
  (cd "${BUILD_DIR}" && gcov -b -c "${object}" 2>&1) | awk -v source="${source}" '
    $0 ~ "^File .*" source { selected = 1; next }
    selected && /^Lines executed:/ { print source ": " substr($0, 16); exit }
  '
}

echo "Production line coverage:"
coverage_line "${BUILD_DIR}/test_gear_sensor-GearSensor.gcno" "GearSensor.cpp"
coverage_line "${BUILD_DIR}/test_gear_sketch-test_gear_sketch.gcno" "GearSensor.ino"
