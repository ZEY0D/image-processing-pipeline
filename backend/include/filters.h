#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>
#include <string>

// --- Low Pass Filters ---

// Apply low pass filter: type can be "Average", "Gaussian", or "Median"
cv::Mat applyLowPassFilter(const cv::Mat& input, std::string type, int kernelSize);

#endif // FILTERS_H

