# Light-off sensor captures (COMPSYS 301, Group 14)

Scope captures of the six TEMT6200 phototransistors (Q1–Q6) on the sensor PCB, taken with the room lights off, on white and on black, at two nodes and two setups.

- Captured: 24 Sep 2026
- Setups: `L` = left project, `R` = right project (different light levels)
- Nodes: `Vsense` = sense resistor voltage, `ADC` = PSoC ADC input pin
- Sensor layout: front row Q3, Q6, Q5, Q4; rear pair Q1, Q2

## File naming

```
<node>_lightoff_<colour>_<setup>_Q<n>.csv
e.g. ADC_lightoff_white_L_Q1.csv
```

48 files: 2 nodes × 2 colours × 2 setups × 6 sensors.

## File format

- Row 1–2: headers (`x-axis,1` then `second,Volt`)
- 2000 samples, −50 ms to +50 ms, 50 µs spacing
- Scope resolution: 20 mV

## Mean voltage (V)

| Sensor | Vsense white L | Vsense black L | Vsense white R | Vsense black R | ADC white L | ADC white R | ADC black (L and R) |
|---|---|---|---|---|---|---|---|
| Q1 | 0.90 | 0.23 | 0.61 | 0.18 | 4.64 | 4.83 | 4.90 |
| Q2 | 0.87 | 0.21 | 0.50 | 0.16 | 4.65 | 4.88 | 4.90 |
| Q3 | 1.20 | 0.32 | 0.79 | 0.28 | 4.36 | 4.80 | 4.90 |
| Q4 | 1.46 | 0.45 | 0.95 | 0.40 | 4.22 | 4.73 | 4.90 |
| Q5 | 0.86 | 0.22 | 0.49 | 0.15 | 4.64 | 4.88 | 4.90 |
| Q6 | 0.89 | 0.22 | 0.55 | 0.16 | 4.64 | 4.86 | 4.90 |



## Notes

- Don't mix setups. Pair L with L and R with R.
- Vsense reads slightly negative on black (down to −55 mV), a small probe offset.
