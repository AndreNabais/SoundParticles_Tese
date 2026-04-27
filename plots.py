import os
import pandas as pd
import plotly.express as px
import plotly.io as pio
print(pio.renderers.default)
#use plotly
from pathlib import Path

# Get the directory where plots.py is located
#script_dir = os.path.dirname(os.path.abspath(__file__))

# Go one level up from there (to reach /home/andre/Sound-proto)
#base_path = os.path.join(script_dir, "..")

data_dir = Path(".")
src = pd.read_csv(Path(data_dir, "source.csv"), names=["t", "amp"])
left = pd.read_csv(Path(data_dir, "mic_left.csv"), names=["t", "amp"])
right = pd.read_csv(Path(data_dir, "mic_right.csv"), names=["t", "amp"])

# Combine data for easier plotting
src["signal"] = "Source"
right["signal"] = "Mic Right"
left["signal"] = "Mic Left"


# Concatenate all into one DataFrame
df = pd.concat([src, left, right])

# Plot using Plotly Express
fig = px.line(df,
            x="t",
            y="amp",
            color="signal",
            color_discrete_map={
                "Source": "red",
                "Mic Left": "blue",
                "Mic Right": "green"
            },
            labels={"t": "Time (s)", "amp": "Amplitude"},
            title="Signal Comparison")

fig.show()
fig.write_html("signal_comparison.html", auto_open=True)


#print(df["t"].head())
#print(df["t"].describe())


