#include <iostream>
#include <opencv2/opencv.hpp>

// Include all custom headers
#include "noise.h"
#include "filters.h"
#include "edge_detection.h"
#include "histograms.h"           // Task 8
#include "frequency_filters.h"    // Tasks 9 & 10

using namespace cv;
using namespace std;

int main() {
    // =========================================================================
    // Load the Original Image
    // =========================================================================
    Mat img = imread("test.jpg");

    if (img.empty()) {
        cout << "Error: Could not load image. Make sure 'test.jpg' exists in "
                "the working directory." << endl;
        return -1;
    }

    cout << "--- Starting Combined Vision Pipeline ---" << endl;

    // --- ADD NOISE ---
    Mat noisyImg = addSaltAndPepperNoise(img, 0.15f);
    cout << "1. Noise applied." << endl;

    // --- FILTER NOISE ---
    Mat filteredImg = applyLowPassFilter(noisyImg, "Median", 5);
    cout << "2. Filter applied." << endl;

    // --- EDGE DETECTION ---
    Mat grayImg;
    cvtColor(filteredImg, grayImg, COLOR_BGR2GRAY);

    Mat sobelEdges   = edge::applySobel(grayImg);
    Mat prewittEdges = edge::applyPrewitt(grayImg);
    Mat robertsEdges = edge::applyRoberts(grayImg);
    Mat cannyEdges   = edge::applyCanny(grayImg, 50, 150);
    cout << "3. Edge detection applied." << endl;

    // =========================================================================
    // Grayscale Conversion, Histograms, and CDF Plot
    // =========================================================================
    cout << "\n--- Histograms & CDF ---" << endl;

    // Manual grayscale conversion (no cvtColor)
    Mat manualGray = hist::toGrayscale(img);
    imshow("Manual Grayscale", manualGray);
    imwrite("task8_grayscale.png", manualGray);
    cout << "Manual grayscale done." << endl;

    // Compute histograms + CDFs  (done internally by the plot function)

    // Render and display the histogram + CDF plot
    Mat histPlot = hist::plotHistogramsAndCDF(img,
                                              "Histogram & CDF",
                                              "task8_histogram_cdf.png");
    cout << "Histogram & CDF plot saved to task8_histogram_cdf.png" << endl;

    // =========================================================================
    // Frequency-Domain Filters (LPF and HPF)
    // =========================================================================
    cout << "\n--- Frequency-Domain Filters ---" << endl;

    // Use a grayscale version of the image for frequency filtering
    Mat grayForFreq = hist::toGrayscale(img);   // manual gray

    // Cutoff frequency radius (pixels in the frequency domain).
    // A higher value lets more frequencies through for LPF,
    // and a lower value removes more low-freq content for HPF.
    const float LPF_CUTOFF = 30.0f;
    const float HPF_CUTOFF = 30.0f;

    Mat lpfResult = freq::applyLowPassFilter(grayForFreq, LPF_CUTOFF);
    Mat hpfResult = freq::applyHighPassFilter(grayForFreq, HPF_CUTOFF);

    imshow("Low-Pass Filter Result",  lpfResult);
    imshow("High-Pass Filter Result", hpfResult);
    imwrite("lpf_result.png", lpfResult);
    imwrite("hpf_result.png", hpfResult);
    cout << "LPF and HPF results saved." << endl;

    // =========================================================================
    // Hybrid Image
    // =========================================================================
    cout << "\n--- Hybrid Image ---" << endl;

    // For hybrid images, two different source images should ideally be used.
    // If a second image 'test2.jpg' is available we use it; otherwise we
    // demonstrate with a horizontally flipped version of the same image.
    Mat img2 = imread("test2.jpg");
    if (img2.empty()) {
        cout << "  (test2.jpg not found — using a flipped copy of test.jpg for demo)" << endl;
        flip(img, img2, 1);
    }

    // Pre-process each image before combining (the caller controls this step):
    //   img1 -> LPF  (keeps smooth/blurry content visible up-close)
    //   img2 -> HPF  (keeps edges/detail visible from a distance)
    Mat gray1 = hist::toGrayscale(img);
    Mat gray2 = hist::toGrayscale(img2);
    const float CUTOFF = 20.0f;
    Mat lpfImg = freq::applyLowPassFilter(gray1, CUTOFF);
    Mat hpfImg = freq::applyHighPassFilter(gray2, CUTOFF);

    // createHybridImage just blends the two pre-processed images 50/50
    Mat hybridImg = freq::createHybridImage(lpfImg, hpfImg);
    imshow("Hybrid Image", hybridImg);
    imwrite("hybrid.png", hybridImg);
    cout << "Hybrid image saved to hybrid.png" << endl;

    // =========================================================================
    // Display Results
    // =========================================================================
    imshow("1. Original Image",  img);
    imshow("2. Noisy Image",     noisyImg);
    imshow("3. Filtered Image",  filteredImg);
    imshow("4. Sobel Edges",     sobelEdges);
    imshow("5. Prewitt Edges",   prewittEdges);
    imshow("6. Roberts Edges",   robertsEdges);
    imshow("7. Canny Edges",     cannyEdges);

    cout << "\nPipeline complete! Press any key on any image window to close all." << endl;
    waitKey(0);
    return 0;
}
