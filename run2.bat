@echo off
g++ main_ag.cpp -o main_ag.exe
if %ERRORLEVEL% NEQ 0 (
    echo Erro ao compilar.
    pause
    exit /b
)
echo Rodando...
main_ag.exe
pause