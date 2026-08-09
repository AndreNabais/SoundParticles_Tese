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

src1 = pd.read_csv(Path(data_dir, "source1.csv"), names=["t", "amp"])
src2 = pd.read_csv(Path(data_dir, "source2.csv"), names=["t", "amp"])
microphone = pd.read_csv(Path(data_dir, "microphone.csv"), names=["t", "amp"])


# Combine data for easier plotting
src1["signal"] = "Source1"
src2["signal"] = "Source2"
microphone["signal"] = "Microphone"



# Concatenate all into one DataFrame
df = pd.concat([src1, src2, microphone])

# Plot using Plotly Express
fig = px.line(df,
            x="t",
            y="amp",
            color="signal",
            color_discrete_map={
                "Source1": "red",
                "Source2": "blue",
                "Microphone": "green"
            },
            labels={"t": "Time (s)", "amp": "Amplitude"},
            title="Signal Comparison")

fig.show()
fig.write_html("signal_comparison.html", auto_open=True)


#print(df["t"].head())
#print(df["t"].describe())