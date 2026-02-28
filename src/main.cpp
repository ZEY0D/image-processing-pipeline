#include "filters.h"
#include "noise.h"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Load/upload image from file
    Mat img = imread("image.png");
    if (img.empty()) {
        cout << "Error: Could not load image 'image.png'" << endl;
        return -1;
    }

    cout << "Original image loaded: " << img.rows << "x" << img.cols << " pixels" << endl;

    // --- Demonstrate Noise Effect ---
    // Add stronger Salt and Pepper noise to make it clearly visible
    Mat noisyImg = addSaltAndPepperNoise(img, 0.15); // 15% noise for clear visibility
    imwrite("noisy_output.jpg", noisyImg);
    cout << "Noisy image saved (15% salt and pepper noise applied)" << endl;

    // --- Demonstrate Filter Effect (Noise Removal) ---
    // Apply Median filter to remove the noise (smooths the image)
    Mat filteredImg = applyLowPassFilter(noisyImg, "Median", 5);
    imwrite("filtered_output.jpg", filteredImg);
    cout << "Filtered image saved (Median filter removes noise)" << endl;

    // Display results
    imshow("1. Original Image", img);
    imshow("2. Noisy Image (Salt & Pepper - 15% noise)", noisyImg);
    imshow("3. Filtered Image (Median - removes noise)", filteredImg);

    cout << "All images saved successfully!" << endl;
    cout << "Press any key to close windows..." << endl;
    waitKey(0);
    return 0;
}

