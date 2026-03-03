/**
 * @file histograms.cpp
 * @brief Task 8 — Implementation of RGB Histograms, CDF, and Plot Rendering.
 */

#include "histograms.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace hist {

// =============================================================================
// Task 8a — Manual Grayscale Conversion
// =============================================================================

cv::Mat toGrayscale(const cv::Mat& bgrImage)
{
    if (bgrImage.empty()) {
        throw std::invalid_argument("toGrayscale: input image is empty.");
    }
    if (bgrImage.channels() != 3) {
        throw std::invalid_argument("toGrayscale: expected a 3-channel BGR image.");
    }

    // Allocate output: same size, single channel, 8-bit unsigned
    cv::Mat gray(bgrImage.rows, bgrImage.cols, CV_8UC1);

    // Iterate every pixel and apply the luminosity-weighted formula:
    //   Gray = 0.299 * R + 0.587 * G + 0.114 * B
    // OpenCV stores BGR, so Vec3b: [0]=B, [1]=G, [2]=R
    for (int row = 0; row < bgrImage.rows; ++row) {
        for (int col = 0; col < bgrImage.cols; ++col) {
            const cv::Vec3b pixel = bgrImage.at<cv::Vec3b>(row, col);
            const double B = static_cast<double>(pixel[0]);
            const double G = static_cast<double>(pixel[1]);
            const double R = static_cast<double>(pixel[2]);

            // Clamp result to [0, 255] before casting to uchar
            const double grayVal = 0.299 * R + 0.587 * G + 0.114 * B;
            gray.at<uchar>(row, col) = static_cast<uchar>(std::min(255.0, std::max(0.0, grayVal)));
        }
    }

    return gray;
}

// =============================================================================
// Task 8b — Manual Histogram Calculation
// =============================================================================

void calculateHistograms(const cv::Mat& bgrImage,
                         Histogram& bHist,
                         Histogram& gHist,
                         Histogram& rHist)
{
    if (bgrImage.empty()) {
        throw std::invalid_argument("calculateHistograms: input image is empty.");
    }
    if (bgrImage.channels() != 3) {
        throw std::invalid_argument("calculateHistograms: expected a 3-channel BGR image.");
    }

    // Zero-initialise all three 256-bin histograms
    bHist.fill(0);
    gHist.fill(0);
    rHist.fill(0);

    // Single pass over every pixel — O(rows * cols)
    // Vec3b: [0]=Blue, [1]=Green, [2]=Red
    for (int row = 0; row < bgrImage.rows; ++row) {
        // Raw row pointer for maximum speed (avoids at<> bounds checks)
        const uchar* rowPtr = bgrImage.ptr<uchar>(row);
        for (int col = 0; col < bgrImage.cols; ++col) {
            const int b = rowPtr[col * 3 + 0];
            const int g = rowPtr[col * 3 + 1];
            const int r = rowPtr[col * 3 + 2];

            ++bHist[b];
            ++gHist[g];
            ++rHist[r];
        }
    }
}

// =============================================================================
// Task 8c — CDF Calculation
// =============================================================================

CDF calculateCDF(const Histogram& histogram)
{
    CDF cdf{};

    // Compute the total number of pixels represented in the histogram
    long long total = 0;
    for (int count : histogram) {
        total += count;
    }

    if (total == 0) {
        // Return an all-zero CDF for an empty histogram
        cdf.fill(0.0);
        return cdf;
    }

    // Running cumulative sum, normalised by the total pixel count
    long long cumulativeSum = 0;
    for (int i = 0; i < 256; ++i) {
        cumulativeSum    += histogram[i];
        cdf[i]     = static_cast<double>(cumulativeSum) / static_cast<double>(total);
    }

    return cdf;
}

} // namespace hist
