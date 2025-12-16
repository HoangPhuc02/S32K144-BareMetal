# PDB (Programmable Delay Block) Driver

## Overview
PDB driver for precise ADC triggering on S32K144.

## Features
- Periodic ADC triggering
- Software trigger support
- Configurable prescaler and multiplier
- Pre-trigger delays for multi-channel ADC

## Usage
```c
#include "lib/hal/pdb/pdb.h"

// Configure PDB for 10ms period @ 48MHz bus clock
pdb_config_t pdb_cfg = {
    .prescaler = PDB_PRESCALER_4,
    .mult_factor = PDB_MULT_10,
    .trigger_source = PDB_TRIGGER_SOFTWARE,
    .mod_value = 12000,  // (48MHz / 4 / 10) * 0.01s
    .continuous_mode = true,
    .enable_interrupt = false
};

PDB_Init(PDB_INSTANCE_0, &pdb_cfg);
PDB_ConfigADCTrigger(PDB_INSTANCE_0, 0, 0, 0);
PDB_LoadConfig(PDB_INSTANCE_0);
PDB_Enable(PDB_INSTANCE_0);
PDB_SoftwareTrigger(PDB_INSTANCE_0);
```

## Integration with ADC
See `lib/hal/adc/adc.h` for high-level API:
- `ADC_ConfigPDBTrigger()`
- `ADC_StartPDBTrigger()`
- `ADC_StopPDBTrigger()`
