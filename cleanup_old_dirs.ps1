# Cleanup Script - Remove old directories after successful migration
# ONLY run this after verifying the new structure works!

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Red
Write-Host "CLEANUP OLD DIRECTORIES" -ForegroundColor Red
Write-Host "========================================" -ForegroundColor Red
Write-Host ""
Write-Host "WARNING: This will DELETE old directories!" -ForegroundColor Red
Write-Host ""

$confirmation = Read-Host "Have you verified that the new structure works correctly? (yes/no)"
if ($confirmation -ne "yes") {
    Write-Host "Aborted. Test the new structure first!" -ForegroundColor Yellow
    exit
}

$secondConfirmation = Read-Host "Are you ABSOLUTELY sure you want to delete old directories? (yes/no)"
if ($secondConfirmation -ne "yes") {
    Write-Host "Aborted." -ForegroundColor Yellow
    exit
}

Write-Host "`nRemoving old directories..." -ForegroundColor Yellow

$oldDirs = @(
    "Core/BareMetal",
    "Core/Drivers",
    "Core/config",
    "Doc",
    "board",
    "App",
    "example",
    "Middleware",
    "Debug_FLASH",
    "Debug_Configurations"
)

foreach ($dir in $oldDirs) {
    if (Test-Path $dir) {
        Remove-Item $dir -Recurse -Force
        Write-Host "  Removed: $dir" -ForegroundColor Gray
    }
}

# Remove empty Core directory if empty
if (Test-Path "Core") {
    $coreItems = Get-ChildItem "Core" -Recurse
    if ($coreItems.Count -eq 0) {
        Remove-Item "Core" -Force
        Write-Host "  Removed: Core (empty)" -ForegroundColor Gray
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Cleanup completed!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Project structure is now clean and organized." -ForegroundColor Green
