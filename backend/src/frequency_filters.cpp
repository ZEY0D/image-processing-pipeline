/**
 * @file frequency_filters.cpp
 * @brief Tasks 9 & 10 — Ideal Frequency-Domain Filters and Hybrid Image.
 */

#include "frequency_filters.h"
#include "histograms.h"   // for hist::toGrayscale

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <corecrt_math_defines.h>

namespace freq {

/**
 * @brief Convert an image to single-channel float in [0, 1].
 */
static cv::Mat toFloat(const cv::Mat& src)
{
    cv::Mat gray;
    if (src.channels() == 3) {
        gray = hist::toGrayscale(src);
    } else if (src.channels() == 1) {
        gray = src.clone();
    } else {
        throw std::invalid_argument("toFloat: unsupported channel count.");
    }

    cv::Mat floatMat;
    gray.convertTo(floatMat, CV_32F, 1.0 / 255.0);
    return floatMat;
}

/**
 * @brief Dramatically enhance high-pass filtered images to make edges visible
 */
static cv::Mat enhanceHighPass(const cv::Mat& floatImg)
{
    CV_Assert(floatImg.type() == CV_32FC1);
    
    // Take absolute value (edges can be positive or negative)
    cv::Mat absImg;
    cv::absdiff(floatImg, cv::Scalar(0), absImg);
    
    // Apply strong Gaussian blur to remove noise while keeping edges
    cv::Mat blurred;
    cv::GaussianBlur(absImg, blurred, cv::Size(3, 3), 1.0);
    
    // Find the range
    double minVal, maxVal;
    cv::minMaxLoc(blurred, &minVal, &maxVal);
    
    // Create output image
    cv::Mat enhanced = cv::Mat::zeros(absImg.size(), CV_32FC1);
    
    // Use adaptive thresholding to keep only significant edges
    double threshold = maxVal * 0.1; // 10% of max as threshold
    
    for (int i = 0; i < blurred.rows; i++) {
        const float* blurRow = blurred.ptr<float>(i);
        const float* absRow = absImg.ptr<float>(i);
        float* outRow = enhanced.ptr<float>(i);
        
        for (int j = 0; j < blurred.cols; j++) {
            if (blurRow[j] > threshold) {
                // This is a significant edge - amplify it dramatically
                // Square the value to boost strong edges
                float val = absRow[j] / maxVal;  // Normalize to [0,1]
                val = val * val;  // Square to boost strong edges
                val = val * 2.0f; // Amplify further
                outRow[j] = std::min(val, 1.0f); // Clamp to [0,1]
            }
            // Else leave as 0 (black background)
        }
    }
    
    // Apply another round of amplification
    cv::multiply(enhanced, enhanced, enhanced); // Square again for extra boost
    
    return enhanced;
}

/**
 * @brief Convert float image back to 8-bit grayscale
 */
static cv::Mat toUint8(const cv::Mat& floatImg, bool isHighPass = false)
{
    CV_Assert(floatImg.type() == CV_32FC1);
    
    if (isHighPass) {
        // For high-pass, use special enhancement
        cv::Mat enhanced = enhanceHighPass(floatImg);
        
        cv::Mat output;
        enhanced.convertTo(output, CV_8UC1, 255.0);
        return output;
    } else {
        // For low-pass, just normalize
        double minVal, maxVal;
        cv::minMaxLoc(floatImg, &minVal, &maxVal);
        
        cv::Mat normalized;
        if (maxVal - minVal > 1e-9) {
            normalized = (floatImg - minVal) / (maxVal - minVal);
        } else {
            normalized = floatImg.clone();
        }
        
        cv::Mat output;
        normalized.convertTo(output, CV_8UC1, 255.0);
        return output;
    }
}

// -----------------------------------------------------------------------------
// Manual complex multiplication
// -----------------------------------------------------------------------------
static void multiplyComplexByRealMask(cv::Mat& complexMat, const cv::Mat& realMask)
{
    CV_Assert(complexMat.type()  == CV_32FC2);
    CV_Assert(realMask.type()    == CV_32FC1);
    CV_Assert(complexMat.size()  == realMask.size());
    
    for (int row = 0; row < complexMat.rows; ++row) {
        cv::Vec2f*    cRow = complexMat.ptr<cv::Vec2f>(row);
        const float*  mRow = realMask.ptr<float>(row);
        
        for (int col = 0; col < complexMat.cols; ++col) {
            const float m  = mRow[col];
            cRow[col][0]  *= m;
            cRow[col][1]  *= m;
        }
    }
}

// -----------------------------------------------------------------------------
// Build frequency mask
// -----------------------------------------------------------------------------
static cv::Mat buildFrequencyMask(int rows, int cols, bool isLowPass, double scale = -1.0)
{
    cv::Mat mask(rows, cols, CV_32FC1);

    const double cy = rows / 2.0;
    const double cx = cols / 2.0;
    
    // Calculate the maximum distance from center
    const double maxDist = std::sqrt(cy * cy + cx * cx);
    
    if (isLowPass) {
        // Low-pass: keep only very low frequencies
        // scale controls the cutoff as a fraction of maxDist (default 0.15)
        const double cutoffFrac = (scale > 0.0 && scale <= 1.0) ? scale : 0.15;
        const double cutoffDist = maxDist * cutoffFrac;
        const double transitionWidth = cutoffDist * 0.3;
        
        for (int u = 0; u < rows; ++u) {
            float* rowPtr = mask.ptr<float>(u);
            for (int v = 0; v < cols; ++v) {
                const double du = static_cast<double>(u) - cy;
                const double dv = static_cast<double>(v) - cx;
                const double D = std::sqrt(du * du + dv * dv);
                
                if (D <= cutoffDist) {
                    rowPtr[v] = 1.0f;
                } else if (D <= cutoffDist + transitionWidth) {
                    double t = (D - cutoffDist) / transitionWidth;
                    double val = std::cos(t * M_PI / 2.0);
                    rowPtr[v] = static_cast<float>(val);
                } else {
                    rowPtr[v] = 0.0f;
                }
            }
        }
    } else {
        // High-pass: keep high frequencies, but with a smoother transition
        // scale controls the cutoff as a fraction of maxDist (default 0.25)
        const double stopFrac = (scale > 0.0 && scale <= 1.0) ? scale : 0.25;
        const double stopDist = maxDist * stopFrac;
        const double transitionWidth = stopDist * 0.5;
        
        for (int u = 0; u < rows; ++u) {
            float* rowPtr = mask.ptr<float>(u);
            for (int v = 0; v < cols; ++v) {
                const double du = static_cast<double>(u) - cy;
                const double dv = static_cast<double>(v) - cx;
                const double D = std::sqrt(du * du + dv * dv);
                
                if (D >= stopDist + transitionWidth) {
                    rowPtr[v] = 1.0f;
                } else if (D >= stopDist) {
                    double t = (D - stopDist) / transitionWidth;
                    double val = std::sin(t * M_PI / 2.0);
                    rowPtr[v] = static_cast<float>(val);
                } else {
                    rowPtr[v] = 0.0f;
                }
            }
        }
    }
    
    return mask;
}

// =============================================================================
// Task 9 — fourierShift
// =============================================================================

void fourierShift(std::vector<cv::Mat>& planes)
{
    for (cv::Mat& plane : planes) {
        CV_Assert(plane.type() == CV_32FC1 || plane.type() == CV_32FC2);

        const int rows = plane.rows & -2;
        const int cols = plane.cols & -2;

        const int halfR = rows / 2;
        const int halfC = cols / 2;

        cv::Mat q1 = plane(cv::Rect(0,     0,     halfC, halfR));
        cv::Mat q2 = plane(cv::Rect(halfC, 0,     halfC, halfR));
        cv::Mat q3 = plane(cv::Rect(0,     halfR, halfC, halfR));
        cv::Mat q4 = plane(cv::Rect(halfC, halfR, halfC, halfR));

        cv::Mat tmp;
        q1.copyTo(tmp); q4.copyTo(q1); tmp.copyTo(q4);
        q2.copyTo(tmp); q3.copyTo(q2); tmp.copyTo(q3);
    }
}

// =============================================================================
// Internal: core DFT → filter mask → iDFT pipeline
// =============================================================================

static cv::Mat applyFrequencyFilterFloat(const cv::Mat& floatGray, bool isLowPass, double scale = -1.0)
{
    CV_Assert(floatGray.type() == CV_32FC1);

    const int origRows = floatGray.rows;
    const int origCols = floatGray.cols;

    // Step 1: Pad to optimal DFT size
    const int padRows = cv::getOptimalDFTSize(origRows);
    const int padCols = cv::getOptimalDFTSize(origCols);

    cv::Mat padded;
    cv::copyMakeBorder(floatGray, padded,
                       0, padRows - origRows,
                       0, padCols - origCols,
                       cv::BORDER_CONSTANT, cv::Scalar::all(0));

    // Step 2: Forward DFT
    cv::Mat complexSpectrum;
    cv::dft(padded, complexSpectrum, cv::DFT_COMPLEX_OUTPUT);

    // Step 3: Fourier shift
    std::vector<cv::Mat> planes = { complexSpectrum };
    fourierShift(planes);
    complexSpectrum = planes[0];

    // Step 4: Build frequency mask
    cv::Mat mask = buildFrequencyMask(complexSpectrum.rows,
                                      complexSpectrum.cols,
                                      isLowPass,
                                      scale);

    // Step 5: Apply mask
    multiplyComplexByRealMask(complexSpectrum, mask);

    // Step 6: Inverse Fourier shift
    planes = { complexSpectrum };
    fourierShift(planes);
    complexSpectrum = planes[0];

    // Step 7: Inverse DFT
    cv::Mat spatialResult;
    cv::idft(complexSpectrum, spatialResult,
             cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);

    // Step 8: Crop back
    cv::Mat cropped = spatialResult(cv::Rect(0, 0, origCols, origRows)).clone();

    return cropped;
}

cv::Mat applyLowPassFilter(const cv::Mat& grayImage)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("applyLowPassFilter: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    cv::Mat floatResult = applyFrequencyFilterFloat(floatGray, true);
    return toUint8(floatResult, false);
}

cv::Mat applyHighPassFilter(const cv::Mat& grayImage)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("applyHighPassFilter: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    cv::Mat floatResult = applyFrequencyFilterFloat(floatGray, false);
    return toUint8(floatResult, true);
}

// =============================================================================
// Task 10 — Hybrid Image
// =============================================================================

cv::Mat createHybridImage(const cv::Mat& img1, const cv::Mat& img2)
{
    return createHybridImage(img1, img2, 
                            FilterType::LOW_PASS, 
                            FilterType::HIGH_PASS,
                            -1.0, -1.0);
}

cv::Mat createHybridImage(const cv::Mat& img1, 
                          const cv::Mat& img2,
                          FilterType filterType1,
                          FilterType filterType2)
{
    return createHybridImage(img1, img2, filterType1, filterType2, -1.0, -1.0);
}

cv::Mat createHybridImage(const cv::Mat& img1, 
                          const cv::Mat& img2,
                          FilterType filterType1,
                          FilterType filterType2,
                          double scale1,
                          double scale2)
{
    if (img1.empty() || img2.empty()) {
        throw std::invalid_argument("createHybridImage: one or both input images are empty.");
    }

    // Convert both inputs to grayscale float
    cv::Mat float1 = toFloat(img1);
    cv::Mat float2 = toFloat(img2);

    // Resize if needed
    if (float1.size() != float2.size()) {
        cv::resize(float2, float2, float1.size(), 0, 0, cv::INTER_LINEAR);
    }

    // Apply filters
    cv::Mat processed1, processed2;
    
    switch (filterType1) {
        case FilterType::LOW_PASS:
            processed1 = applyFrequencyFilterFloat(float1, true, scale1);
            break;
        case FilterType::HIGH_PASS:
            processed1 = applyFrequencyFilterFloat(float1, false, scale1);
            break;
        case FilterType::NONE:
        default:
            processed1 = float1.clone();
            break;
    }
    
    switch (filterType2) {
        case FilterType::LOW_PASS:
            processed2 = applyFrequencyFilterFloat(float2, true, scale2);
            break;
        case FilterType::HIGH_PASS:
            processed2 = applyFrequencyFilterFloat(float2, false, scale2);
            break;
        case FilterType::NONE:
        default:
            processed2 = float2.clone();
            break;
    }

    // For hybrid image, we want to make edges much more prominent
    cv::Mat blended;
    
    if (filterType1 == FilterType::LOW_PASS && filterType2 == FilterType::HIGH_PASS) {
        // Classic hybrid: low-pass on img1, high-pass on img2
        // But we want the cat edges to be very visible
        
        // First, enhance the high-pass result
        cv::Mat enhancedHP = enhanceHighPass(processed2);
        
        // Blend with weights that favor the edges
        // 40% low-pass (structure), 60% high-pass (edges)
        cv::addWeighted(processed1, 0.4, enhancedHP, 0.6, 0.0, blended);
        
        // Boost contrast in the final result
        double minVal, maxVal;
        cv::minMaxLoc(blended, &minVal, &maxVal);
        if (maxVal - minVal > 1e-9) {
            blended = (blended - minVal) / (maxVal - minVal);
            // Apply gamma to make mid-tones pop
            cv::pow(blended, 0.8, blended);
        }
    } else {
        // For other combinations, use standard blending
        double weight1 = 0.5, weight2 = 0.5;
        cv::addWeighted(processed1, weight1, processed2, weight2, 0.0, blended);
    }

    bool hasHighPass = (filterType1 == FilterType::HIGH_PASS || 
                        filterType2 == FilterType::HIGH_PASS);
    
    return toUint8(blended, hasHighPass);
}

}