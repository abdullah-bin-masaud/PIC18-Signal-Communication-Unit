# Proteus Design Suite - Circuit Schematic Description

## Target Microcontroller
**Microchip PIC18F4550 (40-Pin DIP package)**

## Component Bill of Materials (BOM)
| Component ID | Description | Value / Part Number | Proteus Library Keyword |
|---|---|---|---|
| U1 | Microcontroller | PIC18F4550 | `PIC18F4550` |
| X1 | Crystal Oscillator | 20.0 MHz | `CRYSTAL` |
| C1, C2 | Ceramic Capacitors | 22 pF | `CAP` |
| R1 | Reset Pull-up Resistor | 10 kΩ | `RES` |
| SW1 | Reset Pushbutton (MCLR) | Tactile Switch | `BUTTON` |
| SW2 | Sensor / Signal Input | Tactile Switch | `BUTTON` |
| R2 | Signal Pull-up Resistor | 10 kΩ | `RES` |
| D1 | Event Status Indicator LED | 5mm Green LED | `LED-GREEN` |
| R3 | Current Limiting Resistor | 330 Ω | `RES` |
| COMPIM | Virtual Serial Terminal | RS-232 Physical Model | `COMPIM` |

## Pin Wiring Connections
| MCU Pin | Name | Connected Component | Function |
|---|---|---|---|
| 1 | `VPP/MCLR` | SW1 (to GND) + R1 (to +5V) | Active-low master clear |
| 13 | `OSC1/CLKI` | X1 (Pin 1) + C1 (to GND) | 20 MHz Clock Source |
| 14 | `OSC2/CLKO` | X1 (Pin 2) + C2 (to GND) | 20 MHz Clock Source |
| 33 | `RB0/INT0` | SW2 (to GND) + R2 (to +5V) | External Interrupt 0 Sensor Input |
| 15 | `RC0/T1OSO` | R3 in series with D1 (LED) | Output toggle on captured pulses |
| 25 | `RC6/TX` | COMPIM Pin 3 (RXD) | Serial Telemetry Transmission |
| 26 | `RC7/RX` | COMPIM Pin 2 (TXD) | Serial Command Reception |

## Simulation Configuration in Proteus 8
1. Double click **U1 (PIC18F4550)**.
2. Set **Processor Clock Frequency** to `20MHz`.
3. In **Program File**, attach compiled `.hex` output from MPLAB XC8.
4. Add a **Virtual Terminal** connected to RC6/TX to monitor telemetry logs directly in Proteus.
5. Press the **Play** button in the lower left transport toolbar.
