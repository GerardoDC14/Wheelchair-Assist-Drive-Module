# Build and Flash Guide

## 1. Supported baseline

- STM32CubeIDE 1.13.x or compatible
- STM32CubeMX database compatible with project file version 6
- STM32Cube FW_F0 V1.11.4 or compatible
- Target MCU: STM32F051R8Tx
- ST-LINK programmer/debugger

The CubeMX configuration source of truth is `FifthWheel_4.ioc` and the linker script is `STM32F051R8TX_FLASH.ld`.

## 2. Import into STM32CubeIDE

1. Open STM32CubeIDE.
2. Select `File → Import`.
3. Choose `General → Existing Projects into Workspace`.
4. Select the repository root.
5. Confirm that the `FifthWheel_4` project is detected.
6. Keep the project in its current location rather than copying it into the workspace.

## 3. Toolchain checks

Before building, confirm:

- the selected device is `STM32F051R8Tx`;
- the STM32F0 firmware package is installed;
- the startup file and linker script resolve correctly;
- include paths contain `Core/Inc`, CMSIS and STM32F0 HAL directories;
- floating-point math links successfully because `sqrtf` is used by the control model.

## 4. Build

Use the STM32CubeIDE `Debug` configuration:

```text
Project → Build Project
```

Generated `Debug/` and `Release/` directories are intentionally ignored and must not be committed.

## 5. Flash and debug

1. Connect the board through ST-LINK/SWD.
2. Verify target voltage and common ground.
3. Select `Run` or `Debug`.
4. Confirm that the startup OLED sequence and buzzer indication occur.
5. Begin motor tests with the traction stage mechanically unloaded or safely supported.

## 6. Hardware preflight

Before enabling the motor stage:

- verify the PWM pin and polarity expected by the drive;
- confirm the 48 V and logic power rails;
- verify encoder pulse quality;
- inspect the mechanical attachment and wheel clearance;
- ensure a physical power-disconnect path is available;
- start with the minimum allowed command.

## 7. Repository-only validation

On any machine with Python 3:

```bash
python3 tools/repository_health.py
```

This validates repository structure and configuration metadata. It does not compile ARM firmware.
