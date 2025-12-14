# S32KK144 Project Restructuring Script
# Run this script from the project root directory
# PowerShell 5.1+ required

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "S32KK144 Project Restructuring" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Confirm before proceeding
$confirmation = Read-Host "This will restructure your project. Have you committed your changes? (yes/no)"
if ($confirmation -ne "yes") {
    Write-Host "Aborted. Please commit your changes first." -ForegroundColor Yellow
    exit
}

Write-Host "Starting migration..." -ForegroundColor Green

# Create new directory structure
Write-Host "Creating new directories..." -ForegroundColor Yellow

$newDirs = @(
    "docs/api",
    "docs/guides",
    "docs/standards",
    "docs/examples",
    "config/board",
    "config/drivers",
    "config/project",
    "lib/bsp/device",
    "lib/bsp/common",
    "lib/hal",
    "lib/service",
    "lib/middleware",
    "app/board1",
    "app/board2",
    "app/common",
    "examples",
    "tests/hal",
    "tests/service",
    "tests/mocks",
    "tools",
    "build"
)

foreach ($dir in $newDirs) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "  Created: $dir" -ForegroundColor Gray
    }
}

# Move Documentation
Write-Host "`nMoving documentation..." -ForegroundColor Yellow
if (Test-Path "Doc") {
    Move-Item "Doc/*.md" "docs/guides/" -Force -ErrorAction SilentlyContinue
}
Move-Item "coding_convention.md" "docs/standards/" -Force -ErrorAction SilentlyContinue
Move-Item "coding_convention_short.md" "docs/standards/" -Force -ErrorAction SilentlyContinue
Move-Item "commit_rules.md" "docs/standards/" -Force -ErrorAction SilentlyContinue
if (Test-Path "Doxygen") {
    Move-Item "Doxygen" "docs/api/" -Force -ErrorAction SilentlyContinue
}

# Move Configuration
Write-Host "Moving configuration files..." -ForegroundColor Yellow
if (Test-Path "board") {
    Move-Item "board/*" "config/board/" -Force -ErrorAction SilentlyContinue
}
if (Test-Path "Core/config") {
    Move-Item "Core/config/*" "config/drivers/" -Force -ErrorAction SilentlyContinue
}
if (Test-Path "Project_Settings") {
    Copy-Item "Project_Settings/*" "config/project/" -Recurse -Force -ErrorAction SilentlyContinue
}

# Move BSP (Device files)
Write-Host "Moving BSP files..." -ForegroundColor Yellow
if (Test-Path "Core/Devices") {
    Move-Item "Core/Devices/*" "lib/bsp/device/" -Force -ErrorAction SilentlyContinue
}

# Move HAL (Drivers) - Keep Core/Drivers as primary
Write-Host "Moving HAL drivers..." -ForegroundColor Yellow
if (Test-Path "Core/Drivers") {
    $driverModules = @("adc", "can", "gpio", "lpit", "nvic", "pcc", "port", "uart", "clock", "dma", "i2c", "spi", "systick")
    foreach ($module in $driverModules) {
        if (Test-Path "Core/Drivers/$module") {
            if (!(Test-Path "lib/hal/$module")) {
                New-Item -ItemType Directory -Path "lib/hal/$module" -Force | Out-Null
            }
            Copy-Item "Core/Drivers/$module/*" "lib/hal/$module/" -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host "  Copied: $module driver" -ForegroundColor Gray
        }
    }
}

# Move Service Layer
Write-Host "Moving service layer..." -ForegroundColor Yellow
if (Test-Path "App/App2/service") {
    Copy-Item "App/App2/service/*" "lib/service/" -Recurse -Force -ErrorAction SilentlyContinue
}

# Move Middleware
Write-Host "Moving middleware..." -ForegroundColor Yellow
if (Test-Path "Middleware") {
    Move-Item "Middleware/*" "lib/middleware/" -Force -ErrorAction SilentlyContinue
}

# Move Applications
Write-Host "Moving applications..." -ForegroundColor Yellow
if (Test-Path "App/App1") {
    Copy-Item "App/App1/*" "app/board1/" -Recurse -Force -ErrorAction SilentlyContinue
}
if (Test-Path "App/App2") {
    Copy-Item "App/App2/*" "app/board2/" -Recurse -Force -ErrorAction SilentlyContinue
}

# Move Examples
Write-Host "Moving examples..." -ForegroundColor Yellow
if (Test-Path "example") {
    Move-Item "example/*" "examples/" -Force -ErrorAction SilentlyContinue
}

# Update .gitignore
Write-Host "`nUpdating .gitignore..." -ForegroundColor Yellow
$gitignoreContent = @"
# Build outputs
build/
Debug_FLASH/
Debug_RAM/
Release_FLASH/
Release_RAM/

# Eclipse/S32DS
.metadata/
.settings/
RemoteSystemsTempFiles/

# IDE specific
.cproject
.project
*.launch

# Compiled files
*.o
*.elf
*.hex
*.bin
*.map
*.lst

# Logs
*.log

# OS files
.DS_Store
Thumbs.db
"@

Set-Content -Path ".gitignore" -Value $gitignoreContent

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Migration completed!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Review the new structure" -ForegroundColor White
Write-Host "2. Update include paths in source files" -ForegroundColor White
Write-Host "3. Update .cproject and build configurations" -ForegroundColor White
Write-Host "4. Test compilation" -ForegroundColor White
Write-Host "5. Remove old directories after verification" -ForegroundColor White
Write-Host ""
Write-Host "Old directories to remove after testing:" -ForegroundColor Yellow
Write-Host "  - Core/BareMetal/" -ForegroundColor Gray
Write-Host "  - Core/Drivers/ (after copying to lib/hal/)" -ForegroundColor Gray
Write-Host "  - Doc/" -ForegroundColor Gray
Write-Host "  - board/" -ForegroundColor Gray
Write-Host "  - App/" -ForegroundColor Gray
Write-Host "  - example/" -ForegroundColor Gray
Write-Host "  - Middleware/" -ForegroundColor Gray
Write-Host ""
Write-Host "Run update_includes.ps1 to fix include paths" -ForegroundColor Cyan
