# Oscilloscope Data — 27 kΩ Shunt

Time-domain captures of the phototransistor output with a **27 kΩ** shunt
resistor. Used to inspect the harmonic content of the signal (e.g. mains /
ambient flicker) so a suitable high-pass filter can be designed.

Each CSV is named `27K_lighton_<surface>.csv`, where the surface is the target
the sensor was pointed at under illumination:

| File | Surface |
| --- | --- |
| `27K_lighton_black.csv` | Black |
| `27K_lighton_white.csv` | White |

Columns: time (s), voltage (V).
