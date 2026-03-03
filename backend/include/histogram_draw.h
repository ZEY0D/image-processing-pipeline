#ifndef HISTOGRAM_DRAW_H
#define HISTOGRAM_DRAW_H

#include <opencv2/opencv.hpp>

// Calculates the histogram and draws both the bar chart and the distribution curve
cv::Mat drawHistogram(const cv::Mat& inputImage);

#endif // HISTOGRAM_DRAW_H
