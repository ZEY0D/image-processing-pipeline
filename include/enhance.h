#ifndef ENHANCE_H
#define ENHANCE_H

#include <opencv2/opencv.hpp>

// Normalizes the image (stretches pixel values to cover the full 0-255 range)
cv::Mat applyNormalization(const cv::Mat& inputImage);

// Equalizes the image histogram (flattens the distribution to improve contrast)
cv::Mat applyEqualization(const cv::Mat& inputImage);

#endif // ENHANCE_H