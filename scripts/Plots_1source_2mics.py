import os
import pandas as pd
import plotly.express as px
import plotly.io as pio
print(pio.renderers.default)
#use plotly
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
data_dir = PROJECT_ROOT / "bin"
# Path(__file__).resolve().parent is scripts/, and .parent again is Sonic_Boom_Thesis/
src = pd.read_csv(Path(data_dir, "source.csv"), names=["t", "amp"])
microphone1 = pd.read_csv(Path(data_dir, "microphone1.csv"), names=["t", "amp"])
microphone2 = pd.read_csv(Path(data_dir, "microphone2.csv"), names=["t", "amp"])


# Combine data for easier plotting
src["signal"] = "Source"
microphone1["signal"] = "Microphone1"
microphone2["signal"] = "Microphone2"



# Concatenate all into one DataFrame
df = pd.concat([src, microphone1, microphone2])

# Plot using Plotly Express
fig = px.line(df,
            x="t",
            y="amp",
            color="signal",
            color_discrete_map={
                "Source": "red",
                "Microphone1": "blue",
                "Microphone2": "green"
            },
            labels={"t": "Time (s)", "amp": "Amplitude"},
            title="Signal Comparison")

fig.show()
fig.write_html("signal_comparison.html", auto_open=True)


#print(df["t"].head())
#print(df["t"].describe())
