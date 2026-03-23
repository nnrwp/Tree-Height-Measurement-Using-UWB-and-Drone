import serial
import csv
from datetime import datetime
import os

PORT = "COM3"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

base_dir = os.path.dirname(os.path.abspath(__file__))
filename = os.path.join(
    base_dir,
    f"uwb_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
)

print("Logging to:", filename)

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)

    # header
    writer.writerow([
        "pc_time",
        "esp_time",
        "tag_id",
        "range_m",
        "rx_power_dbm"
    ])

    while True:
        line = ser.readline().decode(errors="ignore").strip()

        # print("RAW:", line)

        if not line:
            continue

        data = line.split(",")
        if len(data) != 4:
            continue

        pc_time = datetime.now().isoformat(timespec="seconds")
        writer.writerow([
            pc_time,
            data[0],  # esp_time
            data[1],  # tag id
            data[2],  # range
            data[3],  # rx power
        ])

        print(pc_time, data)


