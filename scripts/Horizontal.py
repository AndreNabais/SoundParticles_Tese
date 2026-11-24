import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# Read CSV file
PROJECT_ROOT = Path(__file__).resolve().parent.parent
data_dir = PROJECT_ROOT / "bin"

horizontal = pd.read_csv(data_dir / "Horizontal.csv")


# 1. Horizontal movement (x position vs time)
plt.figure(figsize=(8, 5))

plt.plot(horizontal["time"], horizontal["source_x"], label="Source")
plt.plot(horizontal["time"], horizontal["mic_x"], label="Microphone")

plt.xlabel("Time [s]")
plt.ylabel("X position [m]")
plt.title("Horizontal movement - x position vs time")
plt.legend()
plt.grid(True)
plt.tight_layout()


# 2. Horizontal movement (x position vs y position)
plt.figure(figsize=(8, 5))

plt.plot(horizontal["source_x"], horizontal["source_y"], label="Source trajectory")
plt.plot(horizontal["mic_x"].iloc[0], horizontal["mic_y"].iloc[0], "o", markersize=8, label="Microphone (stationary)")

plt.xlabel("X position [m]")
plt.ylabel("Y position [m]")
plt.title("Horizontal movement - x vs y")
plt.axis("equal")   # same scale on both axes, so distances look correct
plt.legend()
plt.grid(True)
plt.tight_layout()


plt.show()   # keeps both windows open
