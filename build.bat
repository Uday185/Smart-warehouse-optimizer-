echo ========================================================
echo Compiling Smart Warehouse and Inventory Management System
echo ========================================================

set OUTDIR=%TEMP%\kjit-c-project
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

gcc -Iinclude -Wall -std=c99 -o "%OUTDIR%\warehouse.exe" src/main.c src/linear_ds.c src/hierarchical_ds.c src/search_sort.c src/algorithms.c
copy /Y "%OUTDIR%\warehouse.exe" warehouse.exe >nul

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] warehouse.exe compiled successfully to %OUTDIR% and project root.
) else (
    echo [ERROR] Compilation failed. Please inspect errors above.
)
