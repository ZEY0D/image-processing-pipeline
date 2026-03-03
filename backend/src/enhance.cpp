#include "enhance.h"

using namespace cv;

Mat applyNormalization(const Mat& inputImage) {
    Mat normalizedImg;
    normalize(inputImage, normalizedImg, 0, 255, NORM_MINMAX);
    return normalizedImg;
}

Mat applyEqualization(const Mat& inputImage) {
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
