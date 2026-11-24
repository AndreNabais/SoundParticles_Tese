import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.collections import LineCollection
from matplotlib.patches import Circle
from pathlib import Path

# Physical constants (must match the C++ simulation)
C = 343.0     # speed of sound [m/s]
F0 = 440.0    # source frequency [Hz]

# Read CSV file
PROJECT_ROOT = Path(__file__).resolve().parent.parent
data_dir = PROJECT_ROOT / "bin"

circular = pd.read_csv(data_dir / "Circular.csv")

t = circular["time"].to_numpy()
sx, sy, sz = circular["source_x"].to_numpy(), circular["source_y"].to_numpy(), circular["source_z"].to_numpy()
mx, my, mz = circular["mic_x"].to_numpy(), circular["mic_y"].to_numpy(), circular["mic_z"].to_numpy()


# ---------------------------------------------------------------------------
# Derived quantities
# ---------------------------------------------------------------------------

def fit_circle(x, y):
    """Least-squares circle fit (works even if the source only covers part of the circle)."""
    if np.ptp(x) + np.ptp(y) < 1e-9:          # source not moving
        return x[0], y[0], 0.0
    A = np.column_stack([x, y, np.ones_like(x)])
    b = -(x ** 2 + y ** 2)
    (D, E, F), *_ = np.linalg.lstsq(A, b, rcond=None)
    cx, cy = -D / 2, -E / 2
    r2 = cx ** 2 + cy ** 2 - F
    return cx, cy, np.sqrt(r2) if r2 > 0 else 0.0

cx, cy, R = fit_circle(sx, sy)

# Distance and unit vector from source to mic
dx, dy, dz = mx - sx, my - sy, mz - sz
distance = np.sqrt(dx ** 2 + dy ** 2 + dz ** 2)
safe_d = np.where(distance > 0, distance, np.nan)
ux, uy, uz = dx / safe_d, dy / safe_d, dz / safe_d

# Velocities from the positions (numerical derivative)
vsx, vsy, vsz = np.gradient(sx, t), np.gradient(sy, t), np.gradient(sz, t)
vmx, vmy, vmz = np.gradient(mx, t), np.gradient(my, t), np.gradient(mz, t)

# Radial components (same formula as in main): > 0 means moving toward the other one
vs_toward = vsx * ux + vsy * uy + vsz * uz
vm_toward = -(vmx * ux + vmy * uy + vmz * uz)

# Predicted Doppler frequency for the sound EMITTED at time t ...
with np.errstate(divide="ignore", invalid="ignore"):
    f_pred = F0 * (C + vm_toward) / (C - vs_toward)
f_pred[vs_toward >= C] = np.nan   # supersonic toward the mic: formula not valid

# ... which is HEARD later, after travelling the distance
t_arrival = t + distance / C

# Angle of the source around the center
theta = np.arctan2(sy - cy, sx - cx)
angle_swept = np.degrees(np.unwrap(theta) - np.unwrap(theta)[0])


# ---------------------------------------------------------------------------
# Figure 1: dashboard
# ---------------------------------------------------------------------------
fig = plt.figure(figsize=(13, 10))
fig.suptitle(f"Circular movement  |  radius ≈ {R:.2f} m, swept angle ≈ {abs(angle_swept[-1]):.0f}°",
             fontsize=15, fontweight="bold")

# (a) Trajectory coloured by time
ax1 = fig.add_subplot(2, 2, 1)
phi = np.linspace(0, 2 * np.pi, 400)
ax1.plot(cx + R * np.cos(phi), cy + R * np.sin(phi), "--", color="lightgray", label="Full circle")
ax1.plot(cx, cy, "x", color="gray", markersize=8, label="Center")

points = np.column_stack([sx, sy]).reshape(-1, 1, 2)
segments = np.concatenate([points[:-1], points[1:]], axis=1)
lc = LineCollection(segments, cmap="viridis", linewidth=4)
lc.set_array(t[:-1])
ax1.add_collection(lc)
fig.colorbar(lc, ax=ax1, label="Time [s]")

ax1.plot(sx[0], sy[0], "o", color="black", markersize=7, label="Source start")
ax1.plot(mx, my, "*", color="tab:red", markersize=15, label="Microphone")
ax1.set_xlabel("X position [m]")
ax1.set_ylabel("Y position [m]")
ax1.set_title("Trajectory (colour = time)")
ax1.set_aspect("equal", adjustable="datalim")
ax1.legend(loc="lower left", fontsize=8)
ax1.grid(True)


# (d) Polar "pitch map": where on the circle the mic hears high or low pitch
ax4 = fig.add_subplot(2, 2, 4, projection="polar")
ax4.plot(phi, np.full_like(phi, F0), "--", color="gray", label=f"{F0:.0f} Hz")
sc = ax4.scatter(theta, f_pred, c=f_pred, cmap="coolwarm", s=8)
f_min, f_max = np.nanmin(f_pred), np.nanmax(f_pred)
margin = max(1.0, 0.15 * (f_max - f_min))
ax4.set_rlim(min(f_min, F0) - margin, max(f_max, F0) + margin)
ax4.set_title("Pitch map: f' vs source angle\n(red = higher, blue = lower)", fontsize=10)
fig.colorbar(sc, ax=ax4, label="Frequency [Hz]", pad=0.1)

fig.tight_layout()


# ---------------------------------------------------------------------------
# Figure 2: animation with the emitted wavefronts
# One wavefront is drawn per source period; each one is a circle centred where
# it was emitted, growing at the speed of sound. Where they bunch up, the pitch
# is higher; where they spread out, it is lower.
# ---------------------------------------------------------------------------
fig2, axa = plt.subplots(figsize=(8, 8))

xs_all = np.concatenate([sx, mx, [cx - R, cx + R]])
ys_all = np.concatenate([sy, my, [cy - R, cy + R]])
mid_x, mid_y = (xs_all.min() + xs_all.max()) / 2, (ys_all.min() + ys_all.max()) / 2
half = max(np.ptp(xs_all), np.ptp(ys_all)) / 2 * 1.4 + 0.5
axa.set_xlim(mid_x - half, mid_x + half)
axa.set_ylim(mid_y - half, mid_y + half)
axa.set_aspect("equal")
axa.set_xlabel("X position [m]")
axa.set_ylabel("Y position [m]")
axa.set_title("Wavefronts emitted by the moving source")
axa.plot(cx + R * np.cos(phi), cy + R * np.sin(phi), "--", color="lightgray")

dt = t[1] - t[0]
step = max(1, int(round(1.0 / (F0 * dt))))     # one wavefront per period
emit_idx = np.arange(0, len(t), step)
total_time = max(t[-1] - t[0], 1e-9)

wavefronts = []
for e in emit_idx:
    patch = Circle((sx[e], sy[e]), 0.0, fill=False, color="tab:blue", linewidth=1.2, alpha=0.0)
    axa.add_patch(patch)
    wavefronts.append(patch)

trail, = axa.plot([], [], color="tab:orange", linewidth=2)
src_dot, = axa.plot([], [], "o", color="tab:orange", markersize=11, label="Source")
mic_dot, = axa.plot([], [], "*", color="tab:red", markersize=16, label="Microphone")
ray, = axa.plot([], [], ":", color="black", linewidth=1)
info = axa.text(0.02, 0.98, "", transform=axa.transAxes, va="top", family="monospace",
                bbox=dict(boxstyle="round", facecolor="white", alpha=0.85))
axa.legend(loc="upper right")

frame_idx = np.linspace(0, len(t) - 1, min(250, len(t))).astype(int)

def update(fi):
    k = frame_idx[fi]
    t_now = t[k]

    for e, patch in zip(emit_idx, wavefronts):
        if e <= k:
            age = t_now - t[e]
            patch.set_radius(C * age)
            patch.set_alpha(max(0.1, 0.9 * (1 - age / total_time)))   # older wavefronts fade
        else:
            patch.set_alpha(0.0)

    trail.set_data(sx[:k + 1], sy[:k + 1])
    src_dot.set_data([sx[k]], [sy[k]])
    mic_dot.set_data([mx[k]], [my[k]])
    ray.set_data([sx[k], mx[k]], [sy[k], my[k]])
    info.set_text(f"t  = {t_now * 1000:7.2f} ms\n"
                  f"d  = {distance[k]:7.2f} m\n"
                  f"f' = {f_pred[k]:7.1f} Hz (emitted now)")
    return wavefronts + [trail, src_dot, mic_dot, ray, info]

anim = FuncAnimation(fig2, update, frames=len(frame_idx), interval=30, blit=False)
# To save it as a GIF, uncomment:
# anim.save(data_dir / "circular_wavefronts.gif", writer="pillow", fps=30)


plt.show()   # keeps all windows open
