# Per-sensor thresholds

Source: scope captures in this folder (room lights off, L and R sides of arena).
Method: simulated firmware sampling (20 samples, 400 us apart) on every capture,
took worst black (highest pk-pk) and worst white (lowest pk-pk) across L and R.

- mid = (worst black + worst white) / 2
- hys = (worst white - worst black) / 4
- WHITE if pk-pk > mid + hys, BLACK if pk-pk < mid - hys, else keep previous state
- counts = V x 819 (12-bit, Vssa-Vdda, Vdda = 5 V)

| Sensor | Worst black | Worst white | mid (counts) | hys (counts) | mid (V) | hys (V) |
|--------|-------------|-------------|--------------|--------------|---------|---------|
| Q1     | 391         | 708         | 550          | 79           | 0.672   | 0.096   |
| Q2     | 362         | 601         | 482          | 60           | 0.589   | 0.073   |
| Q3     | 449         | 823         | 636          | 93           | 0.777   | 0.114   |
| Q4     | 527         | 1054        | 790          | 132          | 0.965   | 0.161   |
| Q5     | 358         | 646         | 502          | 72           | 0.613   | 0.088   |
| Q6     | 370         | 650         | 510          | 70           | 0.623   | 0.085   |

Worst black came from L, worst white from R, for every sensor.
Previous global threshold (737 +/- 98) failed Q1, Q2, Q5, Q6 on R white.

TODO: repeat with room lights on and update if worst cases change.
