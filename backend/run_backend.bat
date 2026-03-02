@echo off
echo Starting Image Processing Backend...
echo.

REM Set OpenCV DLL path temporarily
set PATH=D:\Downloads\opencv\build\x64\vc16\bin;%PATH%

REM Navigate to the executable directory
cd /d "%~dp0build\Release"

REM Check if executable exists
if not exist "image_processing_pipeline.exe" (
    echo ERROR: image_processing_pipeline.exe not found!
    echo Please build the project first using:
    echo   cd backend/build
    echo   cmake .. -G "Visual Studio 17 2022"
    echo   cmake --build . --config Release
    pause
    exit /b 1
)

REM Run the backend
echo Running backend server...
echo Backend will be available at: http://127.0.0.1:18080
echo.
echo Press Ctrl+C to stop the server
echo.
image_processing_pipeline.exe

pause
