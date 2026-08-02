# Control System

## 1. PWM command

TIM1 channel 2 drives the motor interface. The auto-reload register is configured to 79 and the compare value is bounded between 25 and 80 in the original prototype.

The controller treats the compare value as the assist command. Hardware polarity and motor-drive scaling must be verified before reuse.

## 2. Manual mode

A quadrature rotary encoder updates `targetPWM` through an EXTI interrupt. The firmware detects clockwise and counter-clockwise state transitions and applies a one-count adjustment.

```text
Rotary encoder transition
→ direction decoding
→ targetPWM ± 1
→ clamp to [25, 80]
→ TIM1 compare update
```

Any detected encoder-state change resets the inactivity timer and returns the controller to manual mode.

## 3. Automatic assist prototype

After `AUTO_MODE_TRIGGER_TIME`—2,000 ms—without encoder activity, the controller enters automatic mode.

The current implementation:

1. estimates the fifth-wheel RPM;
2. defines a nearby desired RPM;
3. calculates a candidate PWM using a fitted quadratic motor-response equation;
4. applies the result only when it lies inside the permitted PWM range.

The fitted relation is solved in `CalculatePWMFromRPM()` using:

```text
a = -0.04
b = 19.36
c = -581.43 - desired_rpm
```

Both quadratic roots are evaluated and only a root inside the command interval is accepted.

This is a prototype assist heuristic. It is not a full closed-loop speed controller and does not model wheelchair-user intent, braking distance or motor-drive faults.

## 4. RPM estimation

Wheel pulses are counted by the TIM2 capture interrupt. RPM is calculated from:

```text
RPM = (pulse_count / pulses_per_revolution)
      × 60
      × 1000 / elapsed_time_ms
```

The prototype uses 15 pulses per revolution and a two-sample moving average.

Wheel speed is then converted to km/h using a 16 cm wheel diameter.

## 5. Inertial sensing

The MPU6050 driver supplies Kalman-filtered X and Y angles. These values are currently used for operator telemetry. They provide a foundation for future incline-aware assist limits, but the present control law does not close the loop on inclination.

## 6. Analog telemetry

The ADC reads three channels for:

- LM35 temperature;
- scaled battery voltage;
- ACS712 current.

Battery voltage and temperature are passed to the display layer. Current is acquired for monitoring and future protection logic.

## 7. Safety interpretation

Current firmware safeguards include bounded PWM commands and explicit error-handler lockup after HAL initialization failures. Missing capabilities for safety-critical use include:

- hardware e-stop integration;
- watchdog-supervised recovery;
- overcurrent shutdown;
- undervoltage lockout;
- redundant speed sensing;
- command timeout handling;
- independent braking control;
- formal hazard analysis.
