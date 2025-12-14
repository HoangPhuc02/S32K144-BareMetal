# Update Include Paths Script
# This script updates all #include statements to match the new structure

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Updating Include Paths" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Define path mappings
$pathMappings = @{
    # Old HAL paths -> New paths
    '"../../driver/adc/adc.h"' = '"lib/hal/adc/adc.h"'
    '"../../driver/can/can.h"' = '"lib/hal/can/can.h"'
    '"../../driver/gpio/gpio.h"' = '"lib/hal/gpio/gpio.h"'
    '"../../driver/lpit/lpit.h"' = '"lib/hal/lpit/lpit.h"'
    '"../../driver/nvic/nvic.h"' = '"lib/hal/nvic/nvic.h"'
    '"../../driver/pcc/pcc.h"' = '"lib/hal/pcc/pcc.h"'
    '"../../driver/port/port.h"' = '"lib/hal/port/port.h"'
    '"../../driver/uart/uart.h"' = '"lib/hal/uart/uart.h"'
    
    # Service paths
    '"../../service/adc_srv/adc_srv.h"' = '"lib/service/adc_srv/adc_srv.h"'
    '"../../service/can_srv/can_srv.h"' = '"lib/service/can_srv/can_srv.h"'
    '"../../service/clock_srv/clock_srv.h"' = '"lib/service/clock_srv/clock_srv.h"'
    '"../../service/gpio_srv/gpio_srv.h"' = '"lib/service/gpio_srv/gpio_srv.h"'
    '"../../service/uart_srv/uart_srv.h"' = '"lib/service/uart_srv/uart_srv.h"'
    
    # Device paths
    '"device_registers.h"' = '"lib/bsp/device/device_registers.h"'
    '"startup.h"' = '"lib/bsp/device/startup.h"'
    '"status.h"' = '"lib/bsp/device/status.h"'
    
    # Config paths
    '"clock_config.h"' = '"config/board/clock_config.h"'
    '"pin_mux.h"' = '"config/board/pin_mux.h"'
}

# Get all .c and .h files in lib/ and app/ directories
$sourceFiles = Get-ChildItem -Path "lib","app" -Include *.c,*.h -Recurse -ErrorAction SilentlyContinue

$updatedCount = 0

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw
    $originalContent = $content
    
    # Apply all replacements
    foreach ($oldPath in $pathMappings.Keys) {
        $newPath = $pathMappings[$oldPath]
        $content = $content -replace [regex]::Escape($oldPath), $newPath
    }
    
    # If content changed, write it back
    if ($content -ne $originalContent) {
        Set-Content -Path $file.FullName -Value $content -NoNewline
        Write-Host "Updated: $($file.FullName)" -ForegroundColor Gray
        $updatedCount++
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Updated $updatedCount files" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next: Manually update .cproject and build configurations" -ForegroundColor Yellow
