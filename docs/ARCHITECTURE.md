# Hardware and Firmware Architecture

## 1. System boundary

The project is a powered fifth-wheel module intended to assist wheelchair motion. The embedded controller manages motor-command output, speed feedback, operator adjustment, inertial sensing, analog telemetry and local feedback.

The firmware is organized around one main control loop with interrupt-driven encoder acquisition.

## 2. Power architecture

```text
48 V traction battery
├── BLDC motor-control stage
└── 48 V → 12 V conversion
    └── logic-level conversion
        ├── STM32F051 controller
        ├── sensors
        └── OLED interface
```

The traction and logic paths share the system but serve different current and voltage requirements. Final hardware should include protection, fusing, grounding and isolation appropriate for the selected motor drive.

## 3. Controller architecture

### 3.1 Main MCU

- Device: STM32F051R8Tx
- Package: LQFP64
- Clock baseline: 8 MHz
- Development source of truth: `FifthWheel_4.ioc`
- Toolchain: STM32CubeIDE

### 3.2 Timing and actuation

- **TIM1 channel 2:** PWM command output on PA9.
- **TIM2 channel 1:** input capture for wheel pulse counting on PA0.
- **SysTick/HAL tick:** mode timing, measurement windows and startup sequencing.

### 3.3 Digital inputs

- Rotary encoder inputs are read on PB11/PB12.
- EXTI interrupt logic decodes quadrature direction and updates the PWM target.
- Additional prototype inputs are read in `main.c` and remain available for future safety or mode logic.

### 3.4 I2C devices

I2C1 is configured in fast mode and shared by:

- MPU6050 inertial sensor;
- SSD1306 OLED display.

The IMU driver exposes Kalman-filtered X and Y angle estimates to the display layer.

### 3.5 Analog telemetry

The ADC layer converts raw readings into:

- temperature;
- battery voltage;
- motor current.

Conversion functions are isolated in `Core/Src/adc.c` so calibration constants can be revised without changing the main control loop.

## 4. Firmware modules

| Module | Responsibility |
|---|---|
| `main.c` | Initialization, control mode, PWM target, RPM estimation and orchestration |
| `adc.c` | ADC setup and conversion into engineering units |
| `mpu6050.c` | IMU communication and Kalman-filtered attitude estimates |
| `display.c` | OLED layout and telemetry rendering |
| `ssd1306.c` | Low-level display driver |
| `utilities.c` | Buzzer and support utilities |
| `bitmap_tec.h` | Startup graphic asset |

## 5. Runtime sequence

```text
Power-on
→ GPIO, PWM, encoder and ADC setup
→ HAL and I2C initialization
→ TIM2 capture enable
→ MPU6050 initialization
→ OLED startup sequence
→ audible ready indication
→ continuous control and telemetry loop
```

Within the loop:

1. encoder activity selects manual or automatic behavior;
2. wheel pulses are converted to RPM and filtered;
3. a bounded PWM target is written to TIM1;
4. analog sensors are converted to engineering units;
5. the IMU is sampled;
6. telemetry is rendered on the OLED.

## 6. Architectural limitations

- The application loop is cooperative rather than scheduled by an RTOS.
- PWM output represents an assist command; no redundant actuator-state feedback is implemented.
- Fault management is limited to the MCU error handler and available sensor readings.
- Power-stage protection is primarily a hardware responsibility and is not fully described in firmware.
- The system is a prototype, not a safety-certified mobility controller.
