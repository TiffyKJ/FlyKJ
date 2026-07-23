import sys
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


# locate project root

script_dir = Path(__file__).resolve().parent

project_root = script_dir.parent


csv_path = (
    project_root
    / "output"
    / "noise.csv"
)


if not csv_path.exists():

    print(
        f"Error: {csv_path} not found."
    )

    print(
        "Run ./noise_export first."
    )

    sys.exit(1)

data = pd.read_csv(csv_path)

plt.figure(figsize=(12,6))

plt.plot(
    data["index"],
    data["power"],
    label="Power Envelope",
    alpha=0.7,
    linewidth=1.0
)

plt.plot(
    data["index"],
    data["noise"],
    label="Noise Floor Tracking",
    linewidth=2.0
)

plt.xlabel(
    "Sample Index"
)


plt.ylabel(
    "Power (I^2 + Q^2)"
)

plt.title(
    "DSP Stage 2: Asymmetric IIR Noise Floor Tracking"
)

plt.grid(True)

plt.legend()

plt.tight_layout()

plt.show()
