#include <iostream>
#include <opencv2/opencv.hpp>
#include "edge_detection.h"

// Commenting out the team's code for now so we can focus on Task 3
// #include "filters.h"
// #include "noise.h"

using namespace cv;
using namespace std;

int main() {
    // Make sure you put a test image named "test.png" in your project folder!
    Mat img = imread("test.png"); 
    
    if (img.empty()) {
        cout << "Error: Could not load image. Make sure 'test.png' is in the folder." << endl;
        return -1;
    }

    // Convert to grayscale for edge detection
    Mat grayImg;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);

    // Apply YOUR Sobel function
    Mat sobelEdges = edge::applySobel(grayImg);

    // Show the results
    imshow("1. Original Image", grayImg);
    imshow("2. Sobel Edges (From Scratch)", sobelEdges);

    cout << "Press any key in the image windows to close them..." << endl;
    waitKey(0);
    return 0;
}






















// #include "filters.h"
// #include "noise.h"
// #include <iostream>

// using namespace cv;
// using namespace std;

// int main() {
//     // Load/upload image from file
//     Mat img = imread("image.png");
//     if (img.empty()) {
//         cout << "Error: Could not load image 'image.png'" << endl;
//         return -1;
//     }

//     cout << "Original image loaded: " << img.rows << "x" << img.cols << " pixels" << endl;

//     // --- Demonstrate Noise Effect ---
//     // Add stronger Salt and Pepper noise to make it clearly visible
//     Mat noisyImg = addSaltAndPepperNoise(img, 0.15); // 15% noise for clear visibility
//     imwrite("noisy_output.jpg", noisyImg);
//     cout << "Noisy image saved (15% salt and pepper noise applied)" << endl;

//     // --- Demonstrate Filter Effect (Noise Removal) ---
//     // Apply Median filter to remove the noise (smooths the image)
//     Mat filteredImg = applyLowPassFilter(noisyImg, "Median", 5);
//     imwrite("filtered_output.jpg", filteredImg);
//     cout << "Filtered image saved (Median filter removes noise)" << endl;

//     // Display results
//     imshow("1. Original Image", img);
//     imshow("2. Noisy Image (Salt & Pepper - 15% noise)", noisyImg);
//     imshow("3. Filtered Image (Median - removes noise)", filteredImg);

//     cout << "All images saved successfully!" << endl;
//     cout << "Press any key to close windows..." << endl;
//     waitKey(0);
//     return 0;
// }