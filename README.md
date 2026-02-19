# 5th Wheel - 5th Semester Project

## Project Overview
A constant challenge in technology across multiple sectors is the design and implementation of mechatronic systems for industrial equipment and process automation. This project focuses on a supplementary **5th wheel system for a wheelchair**, driven by a brushless motor to improve mobility.

## Requirements
![fw1](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/c327ff9d-fa48-490c-ad37-0654b138b9e4)

## Solution
A wheel mechanism with a brushless DC motor was developed and mounted to a wheelchair. The system is controlled through a user interface supported by sensors for steering assistance and stability feedback.

Main implemented features:
- Speed control through PWM and rotary encoder input.
- Speed estimation from pulse counting.
- Runtime telemetry display (speed, angles, temperature, voltage).
- Sensor integration with ADC and I2C devices.

![fw2](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/32165e3b-c78e-42e4-8f03-f944379d6138)

## Block Diagram
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/36439088-edaf-432a-93c4-470f54049488)

The STM32F0Discovery microcontroller is the center of control. It provides enough resources for real-time control tasks (PWM generation, ADC sampling, interrupt-based pulse counting, and I2C communication).

Power architecture summary:
- 48V battery for the controller/motor stage.
- Step-down conversion from 48V -> 12V.
- Step-down conversion from 12V -> logic-level supply for the MCU and electronics.

## Mechanical Elements
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/916a852a-03e1-4da3-b7b4-406fd9321c66)

The base structure uses two 1/4-inch aluminum plates connected by spacers for rigidity. Acrylic side covers protect the internal assembly against water ingress and mechanical damage.

## Development of Control Systems and Features
### Flowchart
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/69b865aa-6c43-4b1e-8efa-d1b823a7f296)

### PWM Control
Motor speed is adjusted by PWM duty cycle. The rotary encoder updates duty cycle and therefore speed.

Implementation details in firmware:
- `TIM1` is configured for PWM output.
- `ARR = 79` defines period scaling.
- `CCR2` sets duty cycle.
- Duty cycle is adjusted from encoder input and automatic control logic.

### Speed Reading
Motor rotational speed is estimated from pulse counting over time windows and converted into RPM.

Implementation details in firmware:
- Pulses are counted via timer capture interrupts.
- RPM is computed periodically from pulse count and pulses-per-revolution.

## Reading Defined Variables (Current, Voltage, Temperature)
### ADC
Analog signals are converted to digital values for runtime monitoring.

Implementation details in firmware:
- ADC initialization in `init_ADC()`.
- Channel sampling via `read_ADC_Channel()`.

### Temperature
LM35 output is converted from ADC code into degrees Celsius.

### Battery Voltage
A voltage divider scales battery voltage to ADC-safe levels, then firmware reconstructs the original battery value.

### Current
ACS712 current sensor output is converted into amperes using offset and sensitivity constants.

## Communication Protocol
### I2C (Inter-Integrated Circuit)
I2C is used for short-distance communication with peripherals such as IMU and display modules.

Implementation details in firmware:
- I2C1 initialization through `MX_I2C1_Init()`.
- SDA/SCL on PB7/PB6.
- MPU6050 and SSD1306 communication over I2C.

## Electronic Design
![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/0cdebcdc-aa42-4db5-8a88-6bbd0dc48da6)

![image](https://github.com/GerardoDC14/5thWheel_5thSemester/assets/123440177/6f9fdb4e-eb84-4556-9b52-72bee884d385)

## Prototype
https://drive.google.com/file/d/1q5f74_9eyorfq265GyVNEVVHl1ltfvB8/view?usp=sharing

## Repository Structure
- `Core/Inc`: project headers.
- `Core/Src`: firmware source modules.
- `Core/Startup`: startup assembly.
- `Drivers`: STM32 HAL and CMSIS.
- `FifthWheel_4.ioc`: CubeMX hardware/peripheral configuration.
- `STM32F051R8TX_FLASH.ld`: linker script.
- `docs/ARCHITECTURE.md`: firmware organization summary.
- `docs/BUILD.md`: build and flash instructions.
