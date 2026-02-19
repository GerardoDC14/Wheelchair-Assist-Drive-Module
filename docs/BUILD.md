# Build and Flash

## Prerequisites
- STM32CubeIDE 1.13.x or compatible
- Target MCU: STM32F051R8Tx

## Open in STM32CubeIDE
1. `File -> Import -> Existing Projects into Workspace`
2. Select this repository root.
3. Build the `Debug` configuration.

## Flash
1. Connect ST-LINK to the board.
2. Click `Run` or `Debug` in STM32CubeIDE.
3. Confirm target voltage and SWD connection.

## Notes
- The linker script is `STM32F051R8TX_FLASH.ld`.
- Peripheral setup source of truth is `FifthWheel_4.ioc`.
