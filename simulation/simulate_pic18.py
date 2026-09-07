"""
High-Fidelity Software Simulation of PIC18F4550 Signal Unit.
Simulates registers, ISR logic, debouncing, 256-byte EEPROM memory, and UART transmission.
"""
import time


class SimulatedPIC18F4550:
    def __init__(self):
        # 256 bytes of non-volatile internal data EEPROM
        self.eeprom = [0x00] * 256
        self.eeprom_ptr = 0x00
        self.event_counter = 0
        self.led_rc0 = False
        print("[MCU SIM] PIC18F4550 initialized @ 20.0 MHz")
        print("[MCU SIM] INT0 external interrupt active on RB0 (falling edge)")

    def trigger_int0_pulse(self):
        """Simulates a sensor/button trigger on RB0 triggering high-priority ISR."""
        print("\n---> Hardware pulse received on RB0/INT0")
        # 1. Debounce verification
        time.sleep(0.015)
        self.event_counter += 1
        self.led_rc0 = not self.led_rc0

        # 2. Write to non-volatile EEPROM
        lsb = self.event_counter & 0xFF
        msb = (self.event_counter >> 8) & 0xFF
        self.eeprom[self.eeprom_ptr] = lsb
        self.eeprom[self.eeprom_ptr + 1] = msb

        saved_addr = self.eeprom_ptr
        self.eeprom_ptr = (self.eeprom_ptr + 2) % 250

        # 3. Transmit UART frame
        telemetry = f"[EVENT] #{self.event_counter} | Pulse Captured | EEPROM Logged at 0x{saved_addr:02X}"
        print(f"[UART TX 9600bps] {telemetry}")
        print(f"[GPIO STATUS] LED RC0 State: {'HIGH (ON)' if self.led_rc0 else 'LOW (OFF)'}")
        return telemetry


if __name__ == "__main__":
    mcu = SimulatedPIC18F4550()
    print("\nSimulating 5 hardware sensor trigger pulses...")
    for _ in range(5):
        mcu.trigger_int0_pulse()
        time.sleep(0.2)

    print("\nEEPROM Memory Dump (First 16 bytes):")
    print(" ".join(f"{b:02X}" for b in mcu.eeprom[:16]))
    print("[MCU SIM] Simulation complete.")
