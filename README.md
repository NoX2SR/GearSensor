# GearSensor

`GearSensor` is the low-speed sensor node for the motorcycle. It reconstructs the selected gear from shift events, uses the neutral switch as an absolute reference, reads temperature and oil-pressure inputs, and exposes the results to the rest of the motorcycle over a half-duplex RS-485 link.

This is a separate Arduino project because these inputs belong together physically and logically, while the timing-sensitive rotational inputs are handled by the companion [RPMReader](https://github.com/NoX2SR/RPMReader) node. Keeping the dashboard-facing [MainMonitor](https://github.com/NoX2SR/MainMonitor) separate also allows this node to continue tracking gear state independently of display work.

## Why gear is tracked

The code models a sequential five-speed motorcycle gearbox rather than reading five dedicated gear-position signals:

- At startup, changes are ignored until neutral is detected. Neutral establishes a known state instead of guessing the current gear.
- Shift-up and shift-down switches update the remembered gear once per press. A latch prevents one long switch pulse from counting as several shifts.
- From neutral, shifting down selects first and shifting up selects second, matching the usual `1-N-2-3-4-5` shift pattern.
- Neutral immediately restores gear `0`.
- An attempted shift above fifth sets a warning. The warning is cleared after a valid RS-485 request is answered.

The current code assumes active-low up/down switches and an active-high neutral switch.

## Hardware and pins

Target: Arduino Uno / ATmega328P, 5 V logic.

| Pin | Purpose | Current interpretation |
| --- | --- | --- |
| D2 | Gear-up switch | Active low |
| D3 | Gear-down switch | Active low |
| D4 | Neutral switch | Active high |
| A0 | Engine-temperature input | Raw ADC value |
| A1 | Outside-temperature input | Raw ADC value |
| A2 | Oil-pressure input | High means low pressure |
| D8 | RS-485 DE/RE | Low receives, high transmits |
| D0/D1 | Hardware serial | 115200 baud |

The sketch does not currently enable internal pull-ups. External biasing and protection are therefore part of the expected motorcycle-side wiring.

## Serial protocol

Requests are two ASCII bytes. The first byte is sensor ID `A`; the second selects data:

| Request | Response content |
| --- | --- |
| `AA` | Gear, warning, engine temperature, outside temperature, oil pressure |
| `AB` | Gear and warning |
| `AC` | Engine-temperature ADC value |
| `AD` | Outside-temperature ADC value |
| `AE` | Oil-pressure state |

Responses start with `A:` and use `:` separators. Every value is sent twice so the receiver can reject a pair that disagrees. An `AA` response has this shape:

```text
A:<gear>:<warning>:<gear>:<warning>:<engine>:<engine>:<outside>:<outside>:<oil>:<oil>::
```

The temperature values are still raw ADC readings; conversion belongs to unfinished monitor logic.

## Build and test

On Debian or Ubuntu, install the AVR toolchain if it is not already present:

```bash
sudo apt install arduino-builder arduino-core-avr gcc-avr avr-libc
```

Then run:

```bash
make verify
```

Firmware artifacts are written to `build/firmware/`. Native tests cover the gear state machine and complete sketch protocol; both production sources currently report 100% line coverage.

## Wokwi simulation

The repository includes `diagram.json` and `wokwi.toml` for the Wokwi VS Code extension. The simulation models the firmware-visible circuit, not a production motorcycle schematic:

- Gear-up and gear-down buttons have external 10K pull-ups and are active low.
- The neutral button has an external 10K pull-down and is active high.
- Two interactive NTC modules feed A0 and A1.
- An SPDT switch drives the oil-pressure input low or high.
- An LED shows the D8 RS-485 transmit-enable state.
- A logic analyzer records UART RX/TX, DE/RE, and the three gear inputs.

Build the firmware first, open `diagram.json`, then run `Wokwi: Start Simulator` from the VS Code command palette:

```bash
make build
```

Press `N` once to establish neutral before using `U` and `D` to shift. In the serial terminal, send `AA`, `AB`, `AC`, `AD`, or `AE` to request data. Wokwi is connected directly to the Uno UART; the D8 LED shows the transceiver direction signal, but a MAX485 and the differential A/B bus are not simulated.

## Editor setup

The checked-in `.vscode` files select `arduino:avr:uno`, associate `.ino` files with C++, and point IntelliSense at the Debian Arduino AVR core and the local `Libs/GearSensor` library. Open this repository as a VS Code folder, or open `Motorcycle.code-workspace` from the parent directory to work on all three repositories together.

## Current limitations

- Switch debouncing is only the release latch; there is no time-based electrical debounce.
- Gear state is inferred, so a missed or false shift pulse remains wrong until neutral is seen.
- Temperature conversion and sensor calibration are not implemented here.
- The serial protocol has duplication but no checksum, framing escape, address arbitration, or timeout handling.
- Motorcycle power, transients, grounding, RS-485 termination, and input protection are outside the firmware and still need explicit hardware design.
