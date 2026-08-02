#!/usr/bin/env python3
"""Dependency-free structural checks for the wheelchair assist repository."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

REQUIRED_PATHS = (
    "README.md",
    "FifthWheel_4.ioc",
    "STM32F051R8TX_FLASH.ld",
    "Core/Src/main.c",
    "Core/Src/adc.c",
    "Core/Src/mpu6050.c",
    "Core/Src/display.c",
    "Core/Src/ssd1306.c",
    "docs/ARCHITECTURE.md",
    "docs/CONTROL_SYSTEM.md",
    "docs/BUILD.md",
    "docs/VALIDATION.md",
    "docs/PROJECT_STATUS.md",
)

FORBIDDEN_NAMES = {
    "Debug",
    "Release",
    "__pycache__",
    ".settings",
}

REQUIRED_MAIN_SYMBOLS = (
    "setupTimer1PWM",
    "ConfigureTIM2",
    "CalculateRPM",
    "CalculateMovingAverage",
    "CalculatePWMFromRPM",
    "MPU6050_Read_All",
    "UpdateDisplay",
)

REQUIRED_IOC_VALUES = {
    "Mcu.Name": "STM32F051R8Tx",
    "ProjectManager.TargetToolchain": "STM32CubeIDE",
    "Mcu.IP0": "I2C1",
    "PB6.Signal": "I2C1_SCL",
    "PB7.Signal": "I2C1_SDA",
}


def parse_ioc(path: Path) -> dict[str, str]:
    values: dict[str, str] = {}
    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        values[key] = value
    return values


def main() -> int:
    failures: list[str] = []

    for relative in REQUIRED_PATHS:
        if not (ROOT / relative).exists():
            failures.append(f"missing required path: {relative}")

    for path in ROOT.rglob("*"):
        if path.is_dir() and path.name in FORBIDDEN_NAMES and ".git" not in path.parts:
            failures.append(f"generated/local directory is present: {path.relative_to(ROOT)}")

    ioc_path = ROOT / "FifthWheel_4.ioc"
    if ioc_path.exists():
        ioc_values = parse_ioc(ioc_path)
        for key, expected in REQUIRED_IOC_VALUES.items():
            actual = ioc_values.get(key)
            if actual != expected:
                failures.append(f"CubeMX value {key} is {actual!r}, expected {expected!r}")

    main_path = ROOT / "Core/Src/main.c"
    if main_path.exists():
        main_text = main_path.read_text(encoding="utf-8", errors="replace")
        for symbol in REQUIRED_MAIN_SYMBOLS:
            if symbol not in main_text:
                failures.append(f"main.c no longer references required symbol: {symbol}")

    if failures:
        for failure in failures:
            print(f"ERROR: {failure}", file=sys.stderr)
        print(f"Repository health check failed with {len(failures)} issue(s).", file=sys.stderr)
        return 1

    print(
        "Repository health check passed: CubeMX target, firmware modules, "
        "control symbols and documentation are present."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
