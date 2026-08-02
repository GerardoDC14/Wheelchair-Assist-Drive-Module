# Validation Record and Test Matrix

## 1. Evidence retained in the repository

The project history documents a physical prototype, electronics diagrams, mechanical integration, firmware source and a demonstration video. The firmware contains direct evidence of the following implemented paths:

| Function | Implementation evidence |
|---|---|
| PWM actuation | TIM1 channel 2 configuration and compare updates in `main.c` |
| Speed feedback | TIM2 input capture, pulse counter and RPM conversion |
| Manual command | PB11/PB12 rotary-encoder state decoding |
| Automatic-mode prototype | inactivity timer and `CalculatePWMFromRPM()` |
| Inertial telemetry | MPU6050 acquisition and Kalman angles |
| Power telemetry | ADC conversion functions for temperature, voltage and current |
| Operator feedback | SSD1306 display and buzzer utilities |

This table establishes code traceability. It does not claim that every function has been recently revalidated on hardware.

## 2. Recommended repeatable test matrix

| Test | Method | Acceptance criterion |
|---|---|---|
| Power-on sequence | Bench supply, motor stage disabled | OLED and buzzer sequence completes without reset |
| PWM bounds | Observe TIM1 output with oscilloscope | Compare value remains within configured bounds |
| Encoder direction | Rotate command encoder both directions | Target increments and decrements consistently |
| Speed pulse acquisition | Signal generator or supported wheel rotation | TIM2 count increases without spurious bursts |
| RPM calculation | Inject known pulse frequency | Calculated RPM matches expected value within chosen tolerance |
| Automatic-mode transition | Stop encoder input for more than 2 s | Controller enters automatic branch once per inactivity condition |
| ADC calibration | Apply known sensor voltages | Engineering-unit conversion matches calibration data |
| IMU attitude | Place module at known angles | Displayed angles remain stable and directionally correct |
| Integrated low-speed test | Supported wheelchair, minimum command | Smooth assist without reset, excessive current or mechanical interference |

## 3. Required instrumentation

- current-limited bench supply;
- oscilloscope or logic analyzer;
- multimeter;
- known-frequency pulse source or calibrated tachometer;
- safe mechanical support for unloaded wheel testing;
- independent emergency power disconnect.

## 4. Known validation gaps

- no automated host-side unit tests for control equations;
- no hardware-in-the-loop fixture;
- no overcurrent or undervoltage shutdown verification;
- no formal braking-distance or incline test;
- no medical-device or accessibility certification;
- no recent CI firmware compilation because the project depends on the STM32CubeIDE-generated toolchain configuration.
