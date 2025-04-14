@echo off
g++ main.cpp -o main.exe
if %ERRORLEVEL% NEQ 0 (
    echo Erro ao compilar.
    pause
    exit /b
)
echo Rodando...
main.exe
pause