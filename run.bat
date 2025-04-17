@echo off
g++ main_ts.cpp -o main_ts.exe
if %ERRORLEVEL% NEQ 0 (
    echo Erro ao compilar.
    pause
    exit /b
)
echo Rodando...
main_ts.exe
pause