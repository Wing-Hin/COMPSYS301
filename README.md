COMPSYS301 - Line-Following Autonomous Robot

Embedded design project built on a Cypress PSoC 5LP (CY8CKIT-059). A two-wheeled robot follows a projected black line, decides its route at intersections, and tracks its own position. all from custom sensor hardware and C firmware.

---

## What it does

- **Sense** — phototransistor array reads black line vs. white floor
- **Locate** — quadrature encoders give wheel position and speed
- **Drive** — PWM through an MC33926 H-bridge controls two DC gearmotors
- **Plan** — shortest-path algorithms decide the route at each intersection
- **Report** — USBUART streams telemetry to a PC for tuning and validation



## Hardware


| Part                             | Role                                         |
| -------------------------------- | -------------------------------------------- |
| CY8CKIT-059 (PSoC 5LP, CY8C58LP) | Cortex-M3 + UDB fabric; all peripherals      |
| TEMT6200FX01                     | Ambient light phototransistor (peak 550 nm)  |
| LM324                            | Buffer / signal conditioning                 |
| MC33926 (x2)                     | H-bridge motor drivers                       |
| DE3 magnetic encoder             | Quadrature feedback, A/B 90 degrees apart    |
| 416 rpm planetary gearmotor      | 19:1 gearbox                                 |
| Custom daughterboard             | Altium project: `COMPSYS301_PCB_TEMP.PrjPcb` |


