# Firmware Architecture

## Core modules
- `Core/Src/main.c`: Application loop, PWM logic, encoder handling, mode switching.
- `Core/Src/adc.c`: ADC initialization and sensor conversion helpers.
- `Core/Src/mpu6050.c`: IMU readout and Kalman angle estimation.
- `Core/Src/display.c`: OLED value formatting and screen updates.
- `Core/Src/ssd1306.c`: OLED low-level driver.
- `Core/Src/utilities.c`: Utility functions (buzzer).

## Data flow summary
1. Encoder and pulse capture update motor control references.
2. ADC reads temperature, battery voltage, and current channels.
3. MPU6050 provides orientation estimates over I2C.
4. Display module renders runtime telemetry.

## Hardware interfaces
- I2C1: MPU6050 + SSD1306
- TIM1: PWM output to motor control
- TIM2: Pulse counting / speed estimate
- ADC: analog sensing channels
