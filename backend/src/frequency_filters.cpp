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
 * @brief Convert float image back to 8-bit displayable image
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
            //subtract the minimum so the lowest value becomes 0, then divide by the range so the highest becomes 1.
            normalized = (floatImg - minVal) / (maxVal - minVal);
        } else {
            normalized = floatImg.clone();
        }
        
        cv::Mat output;
        normalized.convertTo(output, CV_8UC1, 255.0);
        return output;
    }
}

/**
 * @brief Convert complex spectrum to magnitude spectrum for visualization
 */
static cv::Mat spectrumToMagnitude(const cv::Mat& complexSpectrum)
{
    CV_Assert(complexSpectrum.type() == CV_32FC2);
    
    // Split into real and imaginary parts
    std::vector<cv::Mat> planes;
    cv::split(complexSpectrum, planes);
    
    // Compute magnitude: sqrt(Re^2 + Im^2)
    cv::Mat magnitude;
    cv::magnitude(planes[0], planes[1], magnitude);
    
    // Add 1 to avoid log(0)
    magnitude += 1.0f;
    
    // Take log for better visualization
    cv::Mat logMagnitude;
    cv::log(magnitude, logMagnitude);
    
    // Normalize to 0-1 range
    cv::normalize(logMagnitude, logMagnitude, 0, 1, cv::NORM_MINMAX);
    
    return logMagnitude;
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
            //Both parts of each complex number get multiplied by the mask value 
            cRow[col][0]  *= m;
            cRow[col][1]  *= m;
        }
    }
}

// -----------------------------------------------------------------------------
// Build frequency mask
// this creates a grid of values 0-1 the same size as the image.
// -----------------------------------------------------------------------------
static cv::Mat buildFrequencyMask(int rows, int cols, bool isLowPass)
{
    cv::Mat mask(rows, cols, CV_32FC1);

    const double cy = rows / 2.0;
    const double cx = cols / 2.0;
    
    // Calculate the maximum distance from center to current pixel position
    const double maxDist = std::sqrt(cy * cy + cx * cx);
    
    if (isLowPass) {
        // Low-pass: keep only very low frequencies
        const double cutoffDist = maxDist * 0.15; // Increased a bit for more structure
        const double transitionWidth = cutoffDist * 0.3;
        // Between 15% and 19.5% (the transition zone), gradually fade out using a cosine curve
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
        const double stopDist = maxDist * 0.25; // remove everything within 25% of center (the low frequencies)
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

        //rounds down to the nearest even number to ensure the image can be split exactly in half.
        const int rows = plane.rows & -2;
        const int cols = plane.cols & -2;

        const int halfR = rows / 2;
        const int halfC = cols / 2;
        
        //Splits the image into four quadrants.
        cv::Mat q1 = plane(cv::Rect(0,     0,     halfC, halfR));
        cv::Mat q2 = plane(cv::Rect(halfC, 0,     halfC, halfR));
        cv::Mat q3 = plane(cv::Rect(0,     halfR, halfC, halfR));
        cv::Mat q4 = plane(cv::Rect(halfC, halfR, halfC, halfR));

        cv::Mat tmp;
        //Swaps opposite corners using a temporary holding variable
        q1.copyTo(tmp); q4.copyTo(q1); tmp.copyTo(q4);
        q2.copyTo(tmp); q3.copyTo(q2); tmp.copyTo(q3);
    }
}

// =============================================================================
// Internal: core DFT → filter mask → iDFT pipeline
// =============================================================================

static cv::Mat applyFrequencyFilterFloat(const cv::Mat& floatGray, bool isLowPass)
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
    //decomposes the image into its frequency components 
    cv::Mat complexSpectrum;
    cv::dft(padded, complexSpectrum, cv::DFT_COMPLEX_OUTPUT);

    // Step 3: Fourier shift
    //Moves the DC component to the center
    std::vector<cv::Mat> planes = { complexSpectrum };
    fourierShift(planes);
    complexSpectrum = planes[0];

    // Step 4: Build frequency mask
    cv::Mat mask = buildFrequencyMask(complexSpectrum.rows,
                                      complexSpectrum.cols,
                                      isLowPass);

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

    // Step 8: Removes the padding added in step 1
    cv::Mat cropped = spatialResult(cv::Rect(0, 0, origCols, origRows)).clone();

    return cropped;
}

/**
 * @brief Apply frequency filter and return the filtered magnitude spectrum
 */
static cv::Mat getFilteredSpectrumFloat(const cv::Mat& floatGray, bool isLowPass)
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
                                      isLowPass);

    // Step 5: Apply mask
    multiplyComplexByRealMask(complexSpectrum, mask);

    // Step 6: Convert to magnitude spectrum for visualization
    cv::Mat magnitude = spectrumToMagnitude(complexSpectrum);

    // Step 7: Resize back to original dimensions
    cv::Mat resized;
    cv::resize(magnitude, resized, cv::Size(origCols, origRows));

    return resized;
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

cv::Mat visualizeLowPassSpectrum(const cv::Mat& grayImage)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("visualizeLowPassSpectrum: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    cv::Mat magnitude = getFilteredSpectrumFloat(floatGray, true);
    
    cv::Mat output;
    magnitude.convertTo(output, CV_8UC1, 255.0);
    return output;
}

cv::Mat visualizeHighPassSpectrum(const cv::Mat& grayImage)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("visualizeHighPassSpectrum: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    cv::Mat magnitude = getFilteredSpectrumFloat(floatGray, false);
    
    cv::Mat output;
    magnitude.convertTo(output, CV_8UC1, 255.0);
    return output;
}

// =============================================================================
// Task 10 — Hybrid Image
// =============================================================================

cv::Mat createHybridImage(const cv::Mat& img1, 
                          const cv::Mat& img2,
                          FilterType filterType1,
                          FilterType filterType2)
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
            processed1 = applyFrequencyFilterFloat(float1, true);
            break;
        case FilterType::HIGH_PASS:
            processed1 = applyFrequencyFilterFloat(float1, false);
            break;
        case FilterType::NONE:
        default:
            processed1 = float1.clone();
            break;
    }
    
    switch (filterType2) {
        case FilterType::LOW_PASS:
            processed2 = applyFrequencyFilterFloat(float2, true);
            break;
        case FilterType::HIGH_PASS:
            processed2 = applyFrequencyFilterFloat(float2, false);
            break;
        case FilterType::NONE:
        default:
            processed2 = float2.clone();
            break;
    }

    // For hybrid image, we want to make edges much more prominent
    cv::Mat blended;
    
    if (filterType1 == FilterType::LOW_PASS && filterType2 == FilterType::HIGH_PASS) {
        
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