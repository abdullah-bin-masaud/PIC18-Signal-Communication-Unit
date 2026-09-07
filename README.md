# PIC18 Microcontroller Signal Communication Unit

An embedded systems engineering project featuring interrupt-driven signal acquisition, hardware debouncing, non-volatile EEPROM telemetry logging, and UART serial transmission for the Microchip **PIC18F4550** microcontroller.

## Architecture
```
[ Hardware Signal / Pushbutton ]
                | (Falling edge trigger)
                v
        [ RB0 / INT0 Pin ]
                |
                v
       [ PIC18F4550 MCU ]
        /              \
       v                v
[ 256-Byte EEPROM ]   [ UART TX (RC6) @ 9600 ]
(Event Log Storage)             |
                                v
                      [ PC Serial Monitor / CSV ]
```

## Features
- **Interrupt-Driven:** High-priority ISR on `RB0/INT0` ensures zero missed sensor events.
- **Hardware Debouncing:** 15ms timing check eliminates mechanical contact bounce artifacts.
- **EEPROM Event History:** Circular logging of 16-bit event counters across on-chip non-volatile EEPROM.
- **UART Diagnostic Output:** Formatted telemetry output at standard 9600 baud 8-N-1.
- **Software Emulation Included:** Includes Python-based register & memory simulator for verification without physical hardware.

## Building Firmware
1. Open **Microchip MPLAB X IDE**.
2. Create a new standalone project for device `PIC18F4550`.
3. Select toolchain `Microchip XC8`.
4. Add files from `firmware/` (`main.c`, `uart.h`, `eeprom.h`).
5. Build project to generate `.hex` image.

## Simulation in Proteus
Refer to `proteus/schematic_description.md` for complete circuit schematic wiring and setup instructions.

## PC Serial Monitor
```bash
python serial_monitor.py --port COM3 --baud 9600
```
