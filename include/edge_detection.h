#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <opencv2/opencv.hpp>

// We use namespaces to keep your code separate from your teammates' code
namespace edge {

    // 1. Sobel Edge Detection (From scratch)
    cv::Mat applySobel(const cv::Mat& inputImage);

    // 2. Prewitt Edge Detection (From scratch)
    cv::Mat applyPrewitt(const cv::Mat& inputImage);

    // 3. Roberts Edge Detection (From scratch)
    cv::Mat applyRoberts(const cv::Mat& inputImage);

    // 4. Canny Edge Detection (OpenCV allowed per your notes)
    cv::Mat applyCanny(const cv::Mat& inputImage, double lowThreshold, double highThreshold);

}

#endif // EDGE_DETECTION_H