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

namespace freq {

/**
 * @brief Convert an image to single-channel float in [0, 1].
 *
 * Accepts CV_8UC1 or CV_8UC3 (converted to gray first).
 */
static cv::Mat toFloat(const cv::Mat& src)
{
    cv::Mat gray;
    if (src.channels() == 3) {
        gray = hist::toGrayscale(src); // manual BGR→gray
    } else if (src.channels() == 1) {
        gray = src.clone();
    } else {
        throw std::invalid_argument("toFloat: unsupported channel count.");
    }

    cv::Mat floatMat;
    gray.convertTo(floatMat, CV_32F, 1.0 / 255.0);
    return floatMat;
}

// -----------------------------------------------------------------------------
// Manual complex multiplication:
//   (Re_a + j*Im_a) * Re_mask  =  Re_a * Re_mask + j * Im_a * Re_mask
// The filter mask is purely real (0 or 1), so there is no cross-term.
// complexMat: CV_32FC2, realMask: CV_32FC1
// -----------------------------------------------------------------------------
static void multiplyComplexByRealMask(cv::Mat& complexMat, const cv::Mat& realMask)
{
    CV_Assert(complexMat.type()  == CV_32FC2);
    CV_Assert(realMask.type()    == CV_32FC1);
    CV_Assert(complexMat.size()  == realMask.size());
    
    //Iterates over image height.
    for (int row = 0; row < complexMat.rows; ++row) {
        cv::Vec2f*    cRow = complexMat.ptr<cv::Vec2f>(row);  //cRow → pointer to row of complex pixels, Each pixel is cv::Vec2f which is a pair of floats (real, imag)
        const float*  mRow = realMask.ptr<float>(row); //mRow → pointer to mask row
        
        //Iterates across width.
        for (int col = 0; col < complexMat.cols; ++col) {
            const float m  = mRow[col];       // mask value: 0.0f or 1.0f
            cRow[col][0]  *= m;               // Real part
            cRow[col][1]  *= m;               // Imaginary part
        }
    }
}

// -----------------------------------------------------------------------------
// Build an ideal circular filter mask of size (rows x cols).
//   passInsideRadius = true  → Low-pass  (1 inside circle, 0 outside)
//   passInsideRadius = false → High-pass (0 inside circle, 1 outside)
// -----------------------------------------------------------------------------
static cv::Mat buildIdealMask(int rows, int cols,
                              float cutoff,
                              bool  passInsideRadius)
{
    //Creates floating-point single-channel matrix.
    cv::Mat mask(rows, cols, CV_32FC1);

    const double cy = rows / 2.0;   // centre row
    const double cx = cols / 2.0;   // centre column

    for (int u = 0; u < rows; ++u) {
        float* rowPtr = mask.ptr<float>(u);
        for (int v = 0; v < cols; ++v) {
            // Distance formula: D(u,v) = sqrt((u - M/2)^2 + (v - N/2)^2)
            const double du = static_cast<double>(u) - cy;
            const double dv = static_cast<double>(v) - cx;
            const double D  = std::sqrt(du * du + dv * dv);

            if (passInsideRadius) {
                // Ideal LPF: pass frequencies within radius
                rowPtr[v] = (D <= static_cast<double>(cutoff)) ? 1.0f : 0.0f;
            } else {
                // Ideal HPF: pass frequencies outside radius
                rowPtr[v] = (D >  static_cast<double>(cutoff)) ? 1.0f : 0.0f;
            }
        }
    }

    return mask;
}

// =============================================================================
// Task 9 — fourierShift  (manual quadrant swap)
// =============================================================================

void fourierShift(std::vector<cv::Mat>& planes)
{
    for (cv::Mat& plane : planes) {
        CV_Assert(plane.type() == CV_32FC1 || plane.type() == CV_32FC2);

        // Work with even dimensions (trim one row/col if odd)
        const int rows = plane.rows & -2; // equivalent to rows - (rows % 2)
        const int cols = plane.cols & -2;

        const int halfR = rows / 2;
        const int halfC = cols / 2;

        // Define the four quadrants as cv::Rect regions
        // Q1 = top-left, Q2 = top-right, Q3 = bottom-left, Q4 = bottom-right
        cv::Mat q1 = plane(cv::Rect(0,     0,     halfC, halfR));
        cv::Mat q2 = plane(cv::Rect(halfC, 0,     halfC, halfR));
        cv::Mat q3 = plane(cv::Rect(0,     halfR, halfC, halfR));
        cv::Mat q4 = plane(cv::Rect(halfC, halfR, halfC, halfR));

        // Swap Q1 ↔ Q4 and Q2 ↔ Q3 using a temporary buffer
        cv::Mat tmp;
        q1.copyTo(tmp); q4.copyTo(q1); tmp.copyTo(q4);
        q2.copyTo(tmp); q3.copyTo(q2); tmp.copyTo(q3);
    }
}

// =============================================================================
// Internal: core DFT → filter mask → iDFT pipeline
// =============================================================================

/**
 * @param floatGray   CV_32FC1 image, values in [0, 1].
 * @param cutoff      Frequency-domain radius.
 * @param isLowPass   true = LPF, false = HPF.
 * @return            CV_8UC1 filtered image, same size as the input image.
 */
static cv::Mat applyIdealFilter(const cv::Mat& floatGray,
                                float          cutoff,
                                bool           isLowPass)
{
    CV_Assert(floatGray.type() == CV_32FC1);

    //Get original image dimensions.
    const int origRows = floatGray.rows;
    const int origCols = floatGray.cols;

    // ---- Step 1: Pad to the optimal DFT size --------------------------------
    const int padRows = cv::getOptimalDFTSize(origRows);
    const int padCols = cv::getOptimalDFTSize(origCols);

    cv::Mat padded;
    //Pads the input image with zeros to the optimal DFT size.
    cv::copyMakeBorder(floatGray, padded,
                       0, padRows - origRows,
                       0, padCols - origCols,
                       cv::BORDER_CONSTANT, cv::Scalar::all(0));

    // ---- Step 2: Forward DFT — produce a 2-channel complex result -----------
    cv::Mat complexSpectrum; // CV_32FC2 (Re, Im)
    cv::dft(padded, complexSpectrum, cv::DFT_COMPLEX_OUTPUT);

    // ---- Step 3: Fourier shift (DC → centre) --------------------------------
    std::vector<cv::Mat> planes = { complexSpectrum };
    fourierShift(planes);
    complexSpectrum = planes[0];

    // ---- Step 4: Build the ideal filter mask manually -----------------------
    cv::Mat mask = buildIdealMask(complexSpectrum.rows,
                                  complexSpectrum.cols,
                                  cutoff, isLowPass);

    // ---- Step 5: Manual element-wise complex × real-mask multiplication -----
    multiplyComplexByRealMask(complexSpectrum, mask);

    // ---- Step 6: Inverse Fourier shift (centre → corners) -------------------
    planes = { complexSpectrum };
    //restores the DC component to the corners.
    fourierShift(planes);        // fourierShift is its own inverse for even dims
    complexSpectrum = planes[0];

    // ---- Step 7: Inverse DFT ------------------------------------------------
    cv::Mat spatialResult;
    //DFT_REAL_OUTPUT → discard imaginary residue.
    //DFT_SCALE → scale the result by 1/N.
    cv::idft(complexSpectrum, spatialResult,
             cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);

    // ---- Step 8: Crop back to the original image size -----------------------
    //removes the padding.
    cv::Mat cropped = spatialResult(cv::Rect(0, 0, origCols, origRows)).clone();

    // ---- Step 9: Normalise to [0, 255] and convert to 8-bit ----------------
    double minVal, maxVal;
    cv::minMaxLoc(cropped, &minVal, &maxVal);

    cv::Mat output;
    if (maxVal - minVal > 1e-9) {
        //scales the values to the range [0, 1]
        cropped = (cropped - minVal) / (maxVal - minVal);
    }
    //converts the values to the range [0, 255]
    cropped.convertTo(output, CV_8UC1, 255.0);

    return output; //Standard grayscale image
}


cv::Mat applyLowPassFilter(const cv::Mat& grayImage, float cutoff)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("applyLowPassFilter: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    return applyIdealFilter(floatGray, cutoff, /*isLowPass=*/true);
}

cv::Mat applyHighPassFilter(const cv::Mat& grayImage, float cutoff)
{
    if (grayImage.empty()) {
        throw std::invalid_argument("applyHighPassFilter: input image is empty.");
    }
    cv::Mat floatGray = toFloat(grayImage);
    return applyIdealFilter(floatGray, cutoff, /*isLowPass=*/false);
}

// =============================================================================
// Task 10 — Hybrid Image
// =============================================================================

cv::Mat createHybridImage(const cv::Mat& img1, const cv::Mat& img2)
{
    if (img1.empty() || img2.empty()) {
        throw std::invalid_argument("createHybridImage: one or both input images are empty.");
    }

    // Convert both inputs to grayscale float [0,1] — no filters applied here.
    // The caller is responsible for any pre-processing (LPF, HPF, noise, etc.)
    cv::Mat float1 = toFloat(img1);
    cv::Mat float2 = toFloat(img2);

    // If sizes differ, resize img2 to match img1
    if (float1.size() != float2.size()) {
        cv::resize(float2, float2, float1.size(), 0, 0, cv::INTER_LINEAR);
    }

    // Blend: 50% img1 + 50% img2
    cv::Mat combined;
    cv::addWeighted(float1, 0.5, float2, 0.5, 0.0, combined);

    // Normalise to [0, 255] and return as a standard grayscale image
    double minVal, maxVal;
    cv::minMaxLoc(combined, &minVal, &maxVal);
    if (maxVal - minVal > 1e-9) {
        combined = (combined - minVal) / (maxVal - minVal);
    }

    cv::Mat output;
    combined.convertTo(output, CV_8UC1, 255.0);
    return output;
}

}
