# Image Processing Pipeline

A full-stack image processing application with a C++ backend using OpenCV and Crow, and a React frontend.

## Features

- Salt & Pepper Noise
- Gaussian Noise
- Median Filter
- Sobel Edge Detection
- Canny Edge Detection
- More operations available in the backend

## Tech Stack

**Backend:**
- C++ 17
- OpenCV
- Crow (REST API framework)
- Asio (networking)

**Frontend:**
- React 19
- Create React App
- Tailwind CSS
- Axios

## Prerequisites

**Backend:**
- CMake 3.10+
- Visual Studio 2022 (or compatible C++ compiler)
- OpenCV (configured at `D:/Downloads/opencv/build/x64/vc16/lib`)

**Frontend:**
- Node.js 16+
- npm

---

## 🚀 Quick Start (After Pulling the Repo)

### Step 1: Build the Backend

1. **Navigate to the backend build directory:**
   ```bash
   cd backend/build
   ```

2. **Configure CMake:**
   ```bash
   cmake .. -G "Visual Studio 17 2022"
   ```
   
   Or if using Visual Studio 2019:
   ```bash
   cmake .. -G "Visual Studio 16 2019"
   ```

3. **Build the project:**
   ```bash
   cmake --build . --config Release
   ```

4. **Verify the executable was created:**
   - The executable will be at: `backend/build/Release/image_processing_pipeline.exe`

### Step 2: Install Frontend Dependencies

1. **Navigate to the frontend directory:**
   ```bash
   cd frontend-react
   ```

2. **Install npm packages:**
   ```bash
   npm install
   ```

---

## ▶️ Running the Application

### Start the Backend Server

**Option A: Using the provided batch script (Recommended for Windows)**

```bash
cd backend
run_backend.bat
```

**Option B: Manual execution**

```bash
cd backend/build/Release
image_processing_pipeline.exe
```

**⚠️ Important:** If you get an error about missing `opencv_world4120.dll`:

**Quick Fix:**
```bash
copy D:\Downloads\opencv\build\x64\vc16\bin\*.dll backend\build\Release\
```

**Permanent Fix:** Add OpenCV bin directory to System PATH:
- Add `D:\Downloads\opencv\build\x64\vc16\bin` to your System Environment Variables PATH
- Restart your terminal

**Expected output:**
```
Server running at http://127.0.0.1:18080
```

The API server will be running on `http://127.0.0.1:18080`

### Start the Frontend Development Server

Open a **separate terminal** and run:

```bash
cd frontend-react
npm start
```

**Expected output:**
```
Compiled successfully!

You can now view frontend-react in the browser.

  Local:            http://localhost:3000
  On Your Network:  http://192.168.x.x:3000
```

The frontend will automatically open in your browser at `http://localhost:3000`

## Usage

1. Open your browser to `http://localhost:5173`
2. Upload an image using the file input
3. Select an operation from the dropdown
4. Adjust parameters if needed
5. Click "Run" to process the image
6. View the original and processed images side by side

## API Endpoints

### POST /process

Process an image with the specified operation.

**Request Body:**
```json
{
  "operation": "salt_pepper_noise",
  "image": "base64_encoded_image",
  "ratio": 0.05
}
```

**Response:**
```json
{
  "result": "base64_encoded_result_image"
}
```

## Available Operations

| Operation | Parameters | Description |
|-----------|-----------|-------------|
| `salt_pepper_noise` | `ratio` (0-1) | Adds salt and pepper noise |
| `gaussian_noise` | `sigma` (number) | Adds Gaussian noise |
| `median_filter` | `kernel` (odd number) | Applies median filter |
| `sobel` | None | Sobel edge detection |
| `canny` | `th1`, `th2` (numbers) | Canny edge detection |

## Project Structure

```
image-processing-pipeline/
├── backend/
│   ├── include/          # Header files
│   ├── src/              # Source files
│   ├── build/            # Build directory
│   └── CMakeLists.txt    # CMake configuration
├── frontend/
│   ├── src/
│   │   ├── api/          # API client
│   │   ├── components/   # React components
│   │   ├── pages/        # Page components
│   │   ├── App.jsx       # Main app component
│   │   └── index.js      # Entry point
│   ├── index.html        # HTML template
│   ├── package.json      # Dependencies
│   └── vite.config.js    # Vite configuration
└── README.md
```

## Troubleshooting

**Backend won't compile:**
- Ensure OpenCV path in `CMakeLists.txt` matches your installation
- Check that Asio is properly extracted in `backend/include/`
- Verify Visual Studio is installed with C++ tools

**Frontend won't start:**
- Run `npm install` to ensure all dependencies are installed
- Check that Node.js version is 16 or higher
- Clear node_modules and reinstall if needed

**Connection errors:**
- Ensure backend is running on port 18080
- Check that frontend API baseURL matches backend port
- Verify no firewall is blocking the connection

## License

MIT


---

## 📝 Usage

1. **Open your browser** to `http://localhost:3000` (should open automatically)
2. **Upload an image** using the file input
3. **Select an operation** from the dropdown menu
4. **Adjust parameters** if needed (varies by operation)
5. **Click "Run"** to process the image
6. **View results** - original and processed images displayed side by side

---

## 🔧 Troubleshooting

### Backend Issues

**Problem: CMake can't find OpenCV**
- Solution: Update the OpenCV path in `backend/CMakeLists.txt` line 13:
  ```cmake
  set(OpenCV_DIR "YOUR_PATH/opencv/build/x64/vc16/lib")
  ```

**Problem: Build fails with Asio errors**
- Solution: Ensure Asio is extracted at `backend/include/asio-asio-1-30-2/`
- If missing, extract `backend/include/asio.zip`

**Problem: "ws2_32.lib not found"**
- Solution: Ensure you're using Visual Studio with Windows SDK installed

**Problem: Backend won't start**
- Check if port 18080 is already in use
- Run as administrator if permission denied

### Frontend Issues

**Problem: npm install fails**
- Solution: Delete `node_modules` and `package-lock.json`, then run `npm install` again
- Try: `npm cache clean --force` then `npm install`

**Problem: "Cannot connect to backend"**
- Ensure backend is running on port 18080
- Check console for CORS errors
- Verify proxy setting in `frontend-react/package.json` is `"proxy": "http://localhost:18080"`

**Problem: Port 3000 already in use**
- Solution: Kill the process using port 3000 or set a different port:
  ```bash
  set PORT=3001 && npm start
  ```

---

## 🌐 API Endpoints

### POST /process

Process an image with the specified operation.

**Request Body:**
```json
{
  "operation": "salt_pepper_noise",
  "image": "base64_encoded_image",
  "ratio": 0.05
}
```

**Response:**
```json
{
  "result": "base64_encoded_result_image"
}
```

## Available Operations

| Operation | Parameters | Description |
|-----------|-----------|-------------|
| `salt_pepper_noise` | `ratio` (0-1) | Adds salt and pepper noise |
| `gaussian_noise` | `sigma` (number) | Adds Gaussian noise |
| `median_filter` | `kernel` (odd number) | Applies median filter |
| `sobel` | None | Sobel edge detection |
| `canny` | `th1`, `th2` (numbers) | Canny edge detection |

## 📁 Project Structure

```
image-processing-pipeline/
├── backend/
│   ├── include/          # Header files & libraries
│   │   ├── asio-asio-1-30-2/  # Asio networking library
│   │   ├── crow_all.h    # Crow REST framework
│   │   ├── json.hpp      # JSON library
│   │   └── *.h           # Custom headers
│   ├── src/              # C++ source files
│   │   ├── main.cpp      # API server entry point
│   │   ├── filters.cpp
│   │   ├── edge_detection.cpp
│   │   └── ...
│   ├── build/            # CMake build directory
│   └── CMakeLists.txt    # CMake configuration
├── frontend-react/
│   ├── src/
│   │   ├── components/   # React components
│   │   ├── App.js        # Main app component
│   │   └── index.js      # Entry point
│   ├── public/           # Static assets
│   ├── package.json      # Dependencies
│   └── .env              # Environment variables
└── README.md
```

## 🎯 Quick Command Reference

**Backend:**
```bash
# Build
cd backend/build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Run
cd Release
image_processing_pipeline.exe
```

**Frontend:**
```bash
# Install dependencies (first time only)
cd frontend-react
npm install

# Run development server
npm start
```

## License

MIT
