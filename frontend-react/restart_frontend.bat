@echo off
echo ========================================
echo   Restarting Frontend with Proxy Fix
echo ========================================
echo.

echo Killing any existing React dev servers...
taskkill /F /IM node.exe 2>nul

echo.
echo Starting frontend with proxy configuration...
echo.
echo The proxy will forward /process to http://127.0.0.1:18080
echo.

npm start
