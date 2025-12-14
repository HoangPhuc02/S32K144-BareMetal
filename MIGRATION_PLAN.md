# S32KK144 Project Restructuring Plan

## Overview
This document outlines the step-by-step migration plan to restructure the project for better organization, maintainability, and scalability.

## New Structure
```
S32KK144_BareMetal/
├── docs/                          # All documentation
├── config/                        # Centralized configuration
├── lib/                           # Core libraries
│   ├── bsp/                       # Board Support Package
│   ├── hal/                       # Hardware Abstraction Layer
│   ├── service/                   # Service layer
│   └── middleware/                # Middleware components
├── app/                           # Applications
├── examples/                      # Example programs
├── tests/                         # Unit tests
├── tools/                         # Scripts and tools
└── build/                         # Build outputs (gitignored)
```

## Migration Steps

### Phase 1: Backup and Prepare
1. **Create backup**: `git commit -m "Backup before restructuring"`
2. **Run migration script**: `.\migrate_structure.ps1`

### Phase 2: Verify and Test
1. Check all files moved correctly
2. Update build configurations
3. Test compile for each target
4. Run all examples

### Phase 3: Cleanup
1. Remove old empty directories
2. Update documentation
3. Commit changes

## What Gets Moved

### docs/
- `README.md` → `docs/README.md`
- `Doc/*.md` → `docs/guides/`
- `coding_convention*.md` → `docs/standards/`
- `Doxygen/` → `docs/api/`

### config/
- `board/` → `config/board/`
- `Core/config/` → `config/drivers/`
- `Project_Settings/` → `config/project/`

### lib/bsp/
- `Core/Devices/` → `lib/bsp/device/`

### lib/hal/
- `Core/Drivers/` → `lib/hal/` (primary drivers)
- Delete `Core/BareMetal/` (duplicates)

### lib/service/
- `App2/service/` → `lib/service/`

### lib/middleware/
- `Middleware/` → `lib/middleware/`

### app/
- `App/App1/` → `app/board1/`
- `App/App2/` → `app/board2/`
- `src/main.c` → `app/test/` (if test code)

### examples/
- `example/` → `examples/`

### build/ (NEW - gitignored)
- `Debug_FLASH/` → `build/debug/`
- `Debug_Configurations/` → `build/configs/`

## Post-Migration Updates

### Update Include Paths
All `#include` statements need path updates:
- Old: `#include "../../driver/can/can.h"`
- New: `#include "lib/hal/can/can.h"`

### Update Build Files
- `.cproject`: Update source paths
- `Makefile`: Update include directories
- Launch configs: Update binary paths

## Rollback Plan
If issues occur:
```powershell
git reset --hard HEAD
```
