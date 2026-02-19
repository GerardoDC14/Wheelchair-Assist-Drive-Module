# 5th Wheel - Wheelchair Assistance System

A mechatronics semester project focused on a supplementary **5th wheel** for a wheelchair, powered by a brushless motor and controlled by an STM32F0 platform.

## Quick Navigation
- Build/flash guide: `docs/BUILD.md`
- Firmware architecture: `docs/ARCHITECTURE.md`
- CubeMX configuration: `FifthWheel_4.ioc`
- Linker script: `STM32F051R8TX_FLASH.ld`

## Project Overview
A constant challenge in technology and automation is designing integrated mechatronic systems that are reliable, responsive, and practical. This project addresses that challenge by implementing a mobility-assist wheel module for wheelchairs.

## Requirements
![fw1](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/c327ff9d-fa48-490c-ad37-0654b138b9e4)

## Solution
The system combines:
- Brushless DC motor actuation.
- Rotary-encoder-based speed control.
- Sensor-assisted steering/orientation feedback.
- OLED runtime telemetry.

![fw2](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/32165e3b-c78e-42e4-8f03-f944379d6138)

## Block Diagram
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/36439088-edaf-432a-93c4-470f54049488)

The STM32F0Discovery handles PWM generation, pulse counting, ADC sensing, and I2C communication.

Power path:
- 48V battery for motor/controller stage.
- Step-down conversion: 48V -> 12V.
- Step-down conversion: 12V -> logic-level supply for MCU/electronics.

## Mechanical Elements
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/916a852a-03e1-4da3-b7b4-406fd9321c66)

The structure uses two 1/4-inch aluminum plates with spacers for rigidity, plus acrylic side covers for component protection.

## Control System Development
### Flowchart
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/69b865aa-6c43-4b1e-8efa-d1b823a7f296)

### PWM Control
Motor speed is controlled by PWM duty cycle.
- `TIM1` is configured as PWM output.
- `ARR = 79` sets period scaling.
- `CCR2` controls duty cycle.
- Encoder input and automatic logic update target PWM.

### Speed Reading
Speed is estimated by pulse counting over fixed windows and converted to RPM.
- Timer capture interrupt counts pulses.
- RPM is derived using pulses-per-revolution.

## Sensor Variables (ADC)
### Temperature
LM35 output is converted from ADC code to degrees Celsius.

### Battery Voltage
A divider scales battery voltage to an ADC-safe range; firmware reconstructs real battery voltage.

### Current
ACS712 output is converted to amperes using offset and sensitivity calibration.

## Communication Protocol
### I2C
I2C is used for short-distance peripheral communication.
- I2C1 initialized in firmware (`MX_I2C1_Init()`).
- SDA/SCL on PB7/PB6.
- MPU6050 and SSD1306 communicate over I2C.

## Electronic Design
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/0cdebcdc-aa42-4db5-8a88-6bbd0dc48da6)

![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/6f9fdb4e-eb84-4556-9b52-72bee884d385)

## Prototype
https://drive.google.com/file/d/1q5f74_9eyorfq265GyVNEVVHl1ltfvB8/view?usp=sharing

## Repository Structure
- `Core/Inc`: firmware headers.
- `Core/Src`: application and peripheral source code.
- `Core/Startup`: startup assembly.
- `Drivers`: STM32 HAL + CMSIS libraries.
- `docs`: build and architecture documentation.
