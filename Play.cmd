@echo off
rem Double-click this file to build and play; keep the console open afterwards.
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0play.ps1"
pause
