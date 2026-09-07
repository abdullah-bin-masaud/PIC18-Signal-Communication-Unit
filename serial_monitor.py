"""
PC Serial Telemetry Monitor for PIC18 Microcontroller Unit.
Listens to COM port, decodes pulse event messages, and logs to CSV.
"""
import sys
import time
import argparse
import csv
from pathlib import Path

try:
    import serial
except ImportError:
    serial = None


def run_monitor(port: str, baud: int = 9600, output_csv: str = "events.csv") -> None:
    if serial is None:
        print("[!] pyserial not installed. Install via: pip install pyserial")
        print("[*] Running simulated serial monitor output for demonstration:")
        simulate_serial_stream(output_csv)
        return

    csv_path = Path(output_csv)
    file_exists = csv_path.exists()

    try:
        ser = serial.Serial(port, baud, timeout=1.0)
        print(f"[*] Connected to {port} @ {baud} baud. Waiting for PIC18 events...")
        print("    Press Ctrl+C to stop.")

        with open(csv_path, mode="a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            if not file_exists:
                writer.writerow(["Timestamp", "Raw_Telemetry"])

            while True:
                line = ser.readline().decode("utf-8", errors="replace").strip()
                if line:
                    ts = time.strftime("%Y-%m-%d %H:%M:%S")
                    print(f"[{ts}] {line}")
                    writer.writerow([ts, line])
                    f.flush()

    except serial.SerialException as e:
        print(f"[!] Serial Port Error: {e}")
        print("[*] Fallback: running simulated demonstration:")
        simulate_serial_stream(output_csv)
    except KeyboardInterrupt:
        print("\n[*] Monitor closed by user.")


def simulate_serial_stream(output_csv: str) -> None:
    """Emulates incoming serial telemetry packets when hardware is offline."""
    print("=" * 60)
    print("  SIMULATED PIC18 SERIAL MONITOR (Hardware Offline)")
    print("=" * 60)
    with open(output_csv, mode="w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Timestamp", "Event_ID", "Status", "EEPROM_Address"])

        for event_id in range(1, 6):
            ts = time.strftime("%Y-%m-%d %H:%M:%S")
            eeprom_addr = f"0x{(event_id * 2):02X}"
            log_line = f"[EVENT] #{event_id} | Pulse Captured | EEPROM Logged at {eeprom_addr}"
            print(f"[{ts}] {log_line}")
            writer.writerow([ts, event_id, "Pulse Captured", eeprom_addr])
            time.sleep(0.5)

    print(f"[*] Demonstration logs successfully saved to {output_csv}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="PIC18 Serial Telemetry Monitor")
    parser.add_argument("--port", default="COM3", help="Serial port identifier (e.g. COM3 or /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=9600, help="Baud rate (default: 9600)")
    parser.add_argument("--output", default="events.csv", help="CSV log output destination")
    args = parser.parse_args()

    run_monitor(args.port, args.baud, args.output)
