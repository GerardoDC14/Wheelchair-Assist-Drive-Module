# Contributing

## Scope

Changes should preserve the historical prototype while improving reproducibility, safety documentation, firmware modularity or clearly testable capabilities.

## Required checks

Run the repository-health gate before committing:

```bash
python3 tools/repository_health.py
```

For firmware changes, also build the project in STM32CubeIDE and document:

- toolchain and firmware-package version;
- whether the change was compiled only or flashed to hardware;
- test voltage and mechanical setup;
- observed motor, encoder and sensor behavior;
- any unresolved safety concern.

## Generated files

Do not commit:

- `Debug/` or `Release/` outputs;
- ELF, HEX, BIN, MAP, object or dependency files;
- local STM32CubeIDE workspace settings;
- temporary measurement exports.

## Change isolation

Keep control-law, hardware-interface and documentation changes in separate commits when practical. Avoid regenerating unrelated CubeMX files unless the peripheral configuration actually changed.

## Safety

Never test traction firmware without a physical power-disconnect method and a mechanically controlled setup. Clearly distinguish bench, unloaded-wheel and occupied-chair tests.
