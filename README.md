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
- React 18
- Vite
- Tailwind CSS
- Axios

## Prerequisites

**Backend:**
- CMake 3.10+
- Visual Studio 2022 (or compatible C++ compiler)
- OpenCV (configured at `D:/Downloads/opencv/build/x64/vc16/lib`)

**Frontend:**
- Node.js 16+
- npm or yarn

## Setup Instructions

### Backend Setup

1. Navigate to the backend build directory:
```bash
cd backend/build
```

2. Configure CMake (if not already done):
```bash
cmake .. -G "Visual Studio 17 2022"
```

3. Build the project:
```bash
cmake --build . --config Release
```

4. The executable will be at: `backend/build/Release/image_processing_pipeline.exe`

### Frontend Setup

1. Navigate to the frontend directory:
```bash
cd frontend
```

2. Install dependencies:
```bash
npm install
```

## Running the Application

### Start the Backend Server

```bash
cd backend/build/Release
./image_processing_pipeline.exe
```

The API server will start on `http://127.0.0.1:18080`

### Start the Frontend Development Server

In a separate terminal:

```bash
cd frontend
npm run dev
```

The frontend will start on `http://localhost:5173`

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
