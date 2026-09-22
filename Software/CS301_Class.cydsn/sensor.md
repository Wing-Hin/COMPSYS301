

# Sensor module
 
Reads the six line sensors (Q1–Q6) and reports each one as WHITE or BLACK.
 
## Why it works this way
 
The track light flickers at about 120 Hz, so the sensor output is a pulse train, not a steady voltage. A single sample means nothing. Instead, each sensor is sampled 20 times over one flicker period (8 ms), and the **peak-to-peak** value (max − min) decides the colour. White gives big pulses, black gives small ones.
 
## Hardware setup (PSoC Creator)
 
| Setting | Value |
|---|---|
| Component | `ADC_Sensor` (ADC_SAR_Seq) |
| Channels | 6 — inputs 0–5 wired to Q1–Q6 |
| Resolution | 12-bit |
| Sample rate | 15000 SPS total (2500 SPS per sensor) |
| Input range | Vssa to Vdda (5 V) |
| Reference | Internal Vref (not bypassed) |
| Mode | Free running |


## How it runs
 
1. Every 400 µs the ADC finishes a scan of all six sensors and fires `eocHandler`.
2. The ISR updates each sensor's running max and min.   
3. After 20 scans (8 ms) it computes pk-pk for each sensor and classifies it:
   - pk-pk > mid + hys → **WHITE**
   - pk-pk < mid − hys → **BLACK**
   - in between → keep the previous state (hysteresis, stops flicker at line edges)
4. Results go into a shared `SensorFrame`, `fresh` is set to 1, and the next window starts.

## API
 
| Function | What it does |
|---|---|
| `sensor_init()` | Call once at startup, after `CyGlobalIntEnable`. Starts the ADC and ISR. |
| `sensors_GetFrame()` | Returns a copy of all six states from the same window, plus `fresh`. Safe to call anytime. |
| `getSingleSensorState(i)` | Returns one sensor's state. `SENSOR_UNKNOWN` if `i` is out of range. |
 
States: `SENSOR_BLACK` (0), `SENSOR_WHITE` (1), `SENSOR_UNKNOWN` (2, before the first window finishes).
 
`fresh` is 1 only on the first `sensors_GetFrame()` call after a new window, so `main` can act once per 8 ms.

## Example
 
```c
CyGlobalIntEnable;
sensor_init();
 
for (;;) {
    SensorFrame f = sensors_GetFrame();
    if (f.fresh) {
        if (f.state[Q3] == SENSOR_WHITE) { /* ... */ }
    }
}
```



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