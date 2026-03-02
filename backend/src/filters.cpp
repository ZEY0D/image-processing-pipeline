#include "filters.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// --- Low Pass Filters ---

Mat applyLowPassFilter(const Mat& input, string type, int kernelSize) {
    Mat output;
    if (type == "Average") {
        blur(input, output, Size(kernelSize, kernelSize));
    } else if (type == "Gaussian") {
        GaussianBlur(input, output, Size(kernelSize, kernelSize), 0);
    } else if (type == "Median") {
        medianBlur(input, output, kernelSize);
    }
    return output;
}

