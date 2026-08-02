# Project Status

## Classification

**Functional historical prototype / mechatronics portfolio reference**

## Maturity matrix

| Area | Status | Notes |
|---|---|---|
| Mechanical prototype | Demonstrated | Custom fifth-wheel structure documented with project imagery |
| Electronics integration | Demonstrated | Power conversion, sensing and motor interface documented |
| Firmware | Implemented prototype | Main loop, interrupts, telemetry and display are present |
| Manual control | Implemented | Rotary-encoder command with bounded PWM |
| Automatic assist | Experimental | Heuristic RPM-to-PWM adjustment, not a certified controller |
| Telemetry | Implemented | IMU, temperature, battery and current acquisition paths |
| Build reproducibility | Toolchain-dependent | STM32CubeIDE project and CubeMX configuration retained |
| CI | Repository-level | Static configuration and structure checks only |
| Safety certification | Not performed | Project must not be represented as a medical device |

## What the project demonstrates

- embedded C development on STM32;
- direct peripheral-register configuration;
- interrupt-driven measurement;
- sensor acquisition and signal conversion;
- empirical actuator characterization;
- electromechanical system integration;
- power and mechanical design considerations;
- local human-machine feedback.

## Recommended future work

1. Extract RPM and PWM-model functions into testable pure-C modules.
2. Add host-side unit tests for conversion and clamping logic.
3. Replace the assist heuristic with a documented feedback controller.
4. Add watchdog, current, voltage and command-timeout fault handling.
5. Add a hardware e-stop and independent power-stage shutdown.
6. Capture updated oscilloscope, telemetry and prototype media under `docs/assets/`.
7. Add a reproducible command-line ARM build independent of the IDE.
