#include "enhance.h"

using namespace cv;

Mat applyNormalization(const Mat& inputImage) {
    Mat normalizedImg;
    // NORM_MINMAX finds the darkest pixel and makes it 0, 
    // finds the brightest and makes it 255, and stretches everything in between.
    normalize(inputImage, normalizedImg, 0, 255, NORM_MINMAX);
    return normalizedImg;
}

Mat applyEqualization(const Mat& inputImage) {
    // Standard histogram equalization in OpenCV requires a 1-channel Grayscale image.
    // So, we convert it first if it is colored.
    Mat grayImg;
    if (inputImage.channels() == 3) {
        cvtColor(inputImage, grayImg, COLOR_BGR2GRAY);
    } else {
        grayImg = inputImage.clone();
    }

    Mat equalizedImg;
    equalizeHist(grayImg, equalizedImg);
    return equalizedImg;
}