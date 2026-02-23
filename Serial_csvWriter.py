import serial
import csv
import time
from datetime import datetime

# def stream_serial_to_csv(s
port= "/dev/cu.usbmodem101"
#"COM3",          # Change to your port (e.g., "/dev/ttyUSB0" on Linux/Mac)
baud_rate=115200
output_file="serial_data.csv"
delimiter=","         # Change if your device uses a different delimiter

# fieldnames = ["timestamp", "raw_data", "value1", "value2"]  # Customize as needed

with serial.Serial(port, baud_rate, timeout=1) as ser, \
        open(output_file, "w", newline="") as csvfile:

    writer = csv.writer(csvfile)
    # writer.writerow(fieldnames)  # Write header
    csvfile.flush()

    print(f"Streaming from {port} at {baud_rate} baud → {output_file}")
    print("Press Ctrl+C to stop.\n")

    try:
        while True:
            if ser.in_waiting > 0:
                raw = ser.readline().decode("utf-8", errors="replace").strip()

                if not raw:
                    continue

                timestamp = datetime.now().isoformat()
                parts = raw.split(delimiter)  # Split if device sends multiple values

                # Pad or trim parts to match expected columns (minus timestamp + raw)
                # row = [timestamp, raw] + parts
                writer.writerow(row)
                csvfile.flush()  # Flush so data appears in file immediately

                print(f"[{timestamp}] {raw}")

    except KeyboardInterrupt:
        print("\nStopped by user.")
    except serial.SerialException as e:
        print(f"Serial error: {e}")

# if __name__ == "__main__":
# stream_serial_to_csv(
#     port,
#     baud_rate,
#     output_file="serial_log.csv")