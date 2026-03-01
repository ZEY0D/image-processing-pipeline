#include <iostream>
#include <opencv2/opencv.hpp>

// Include all the custom headers
#include "noise.h"
#include "filters.h"
#include "edge_detection.h"

using namespace cv;
using namespace std;

int main() {
    // 1. Load the Original Image
    // check the image path
    Mat img = imread("test.jpg"); 
    
    if (img.empty()) {
        cout << "Error: Could not load image. Make sure 'src/test.jpg' exists." << endl;
        return -1;
    }

    cout << "--- Starting Combined Vision Pipeline ---" << endl;

    // --- 1: ADD NOISE ---
    // Adding 15% Salt and Pepper noise
    Mat noisyImg = addSaltAndPepperNoise(img, 0.15); 
    cout << "1. Noise applied." << endl;

    // --- TASK 2: FILTER NOISE ---
    // Applying a Median filter (size 5x5) to clean the noise
    Mat filteredImg = applyLowPassFilter(noisyImg, "Median", 5);
    cout << "2. Filter applied." << endl;






    
    // --- 3: EDGE DETECTION ---
    // Edge detection requires grayscale, so we convert the filtered image
    Mat grayImg;
    cvtColor(filteredImg, grayImg, COLOR_BGR2GRAY);

    // Apply your edge detectors
    Mat sobelEdges = edge::applySobel(grayImg);
    Mat prewittEdges = edge::applyPrewitt(grayImg);
    Mat robertsEdges = edge::applyRoberts(grayImg);
    Mat cannyEdges = edge::applyCanny(grayImg, 50, 150); 
    cout << "3. Edge detection applied." << endl;

    // --- DISPLAY ALL RESULTS ---
    // Note: You can drag these windows around on your screen to see them side-by-side
    imshow("1. Original Image", img);
    imshow("2. Noisy Image", noisyImg);
    imshow("3. Filtered Image", filteredImg);
    imshow("4. Sobel Edges", sobelEdges);
    imshow("5. Prewitt Edges", prewittEdges);
    imshow("6. Roberts Edges", robertsEdges);
    imshow("7. Canny Edges", cannyEdges);

    cout << "Pipeline complete! Press any key on the image windows to close them..." << endl;
    
    // Wait for the user to press a key before closing
    waitKey(0);
    return 0;
}




