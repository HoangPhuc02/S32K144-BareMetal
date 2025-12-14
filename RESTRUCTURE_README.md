# Project Restructuring Guide

## Quick Start

Follow these steps to restructure your project:

### 1. Backup Your Work
```powershell
git add .
git commit -m "Backup before restructuring"
```

### 2. Run Migration
```powershell
.\migrate_structure.ps1
```

### 3. Update Include Paths
```powershell
.\update_includes.ps1
```

### 4. Update Build Configuration

#### For Eclipse/S32DS:
1. Right-click project → **Properties**
2. **C/C++ General** → **Paths and Symbols** → **Includes**
3. Add new include paths:
   - `${workspace_loc:/${ProjName}/lib/hal}`
   - `${workspace_loc:/${ProjName}/lib/service}`
   - `${workspace_loc:/${ProjName}/lib/bsp/device}`
   - `${workspace_loc:/${ProjName}/lib/middleware}`
   - `${workspace_loc:/${ProjName}/config}`

4. **C/C++ Build** → **Settings** → **Tool Settings**
5. Update source folder paths to:
   - `lib/hal`
   - `lib/service`
   - `lib/bsp`
   - `lib/middleware`
   - `app/board1` or `app/board2`
   - `config`

### 5. Test Build
```powershell
# Clean build
# Right-click project → Clean Project
# Then build
```

### 6. Verify Everything Works
- [ ] Project compiles without errors
- [ ] All drivers work correctly
- [ ] Examples compile
- [ ] Applications run properly

### 7. Cleanup (ONLY after verification)
```powershell
.\cleanup_old_dirs.ps1
```

## New Structure Overview

```
S32KK144_BareMetal/
├── lib/                    # All reusable code
│   ├── bsp/               # Board Support Package
│   │   └── device/        # Device-specific files
│   ├── hal/               # Hardware Abstraction Layer
│   │   ├── adc/
│   │   ├── can/
│   │   ├── gpio/
│   │   └── ...
│   ├── service/           # Service layer (abstraction)
│   │   ├── adc_srv/
│   │   ├── can_srv/
│   │   └── ...
│   └── middleware/        # Middleware components
│       ├── HMI/
│       └── LCD_I2C/
│
├── app/                   # Applications
│   ├── board1/           # Board 1 app
│   ├── board2/           # Board 2 app
│   └── common/           # Shared app code
│
├── config/               # Configuration
│   ├── board/           # Board config (clocks, pins)
│   ├── drivers/         # Driver configs
│   └── project/         # Build configs
│
├── examples/            # Example programs
├── tests/              # Unit tests
├── docs/               # Documentation
│   ├── api/           # API docs, Doxygen
│   ├── guides/        # Technical guides
│   └── standards/     # Coding standards
│
└── build/             # Build outputs (gitignored)
```

## Benefits

✅ **Clear separation of concerns**
- HAL, Service, BSP, Middleware, App layers

✅ **No duplicate code**
- Single source of truth for each driver

✅ **Scalable**
- Easy to add new drivers, services, apps

✅ **Testable**
- Clear dependencies, easy to mock

✅ **Professional**
- Industry-standard structure

## Include Path Examples

### Old way:
```c
#include "../../driver/can/can.h"
#include "../../service/can_srv/can_srv.h"
```

### New way:
```c
#include "lib/hal/can/can.h"
#include "lib/service/can_srv/can_srv.h"
```

## Troubleshooting

### Build errors after migration?
1. Check include paths in `.cproject`
2. Verify source folders are correct
3. Run `update_includes.ps1` again

### Files missing?
1. Check `build/` directory (old Debug_FLASH contents)
2. Verify migration script completed successfully
3. Check git status to see moved files

### Need to rollback?
```powershell
git reset --hard HEAD
```

## Support

If you encounter issues:
1. Check `MIGRATION_PLAN.md` for detailed steps
2. Review git diff to see what changed
3. Restore from backup if needed
