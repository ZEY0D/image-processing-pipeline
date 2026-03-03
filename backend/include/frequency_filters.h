#ifndef FREQUENCY_FILTERS_H
#define FREQUENCY_FILTERS_H

#include <vector>
#include <opencv2/opencv.hpp>

/**
 * @file frequency_filters.h
 * @brief Tasks 9 & 10 — Ideal Frequency-Domain Filters and Hybrid Images.
 */

namespace freq {

    // =========================================================================
    // Task 9 — Helper: Fourier Shift (fftshift / ifftshift)
    // =========================================================================

    /**
     * @brief Shift zero-frequency component to the centre of the spectrum.
     *
     * Manually swaps the four quadrants of a DFT output so that the DC
     * component moves from the corners to the centre — the standard
     * "fftshift" operation.
     *
     * Operates on one or more planes stored in a vector<cv::Mat>.  Each
     * plane must be a single-channel floating-point matrix.
     *
     * @param planes  In/out vector of DFT planes (real + imaginary).
     */
    void fourierShift(std::vector<cv::Mat>& planes);

    // =========================================================================
    // Task 9 — Frequency Domain Filters
    // =========================================================================

    /**
     * @brief Apply a low-pass filter in the frequency domain (keep only low frequencies).
     *
     * This filter removes high-frequency components (edges, details) and keeps
     * the low-frequency components (smooth regions, overall structure).
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @return           Filtered image with low frequencies only (CV_8UC1).
     */
    cv::Mat applyLowPassFilter(const cv::Mat& grayImage);

    /**
     * @brief Apply a high-pass filter in the frequency domain (keep only high frequencies).
     *
     * This filter removes low-frequency components (smooth regions) and keeps
     * the high-frequency components (edges, details, textures). The result
     * will highlight edges and fine details.
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @return           Filtered image with high frequencies only (edges) (CV_8UC1).
     */
    cv::Mat applyHighPassFilter(const cv::Mat& grayImage);

    /**
     * @brief Visualize the frequency spectrum after applying low-pass filter.
     *
     * Returns the magnitude spectrum of the image after low-pass filtering,
     * showing which frequency components remain.
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @return           Magnitude spectrum visualization (CV_8UC1).
     */
    cv::Mat visualizeLowPassSpectrum(const cv::Mat& grayImage);

    /**
     * @brief Visualize the frequency spectrum after applying high-pass filter.
     *
     * Returns the magnitude spectrum of the image after high-pass filtering,
     * showing which frequency components remain.
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @return           Magnitude spectrum visualization (CV_8UC1).
     */
    cv::Mat visualizeHighPassSpectrum(const cv::Mat& grayImage);

    // =========================================================================
    // Task 10 — Hybrid Image with Filter Options
    // =========================================================================

    /**
     * @brief Enum to specify filter type for hybrid image creation
     */
    enum class FilterType {
        NONE,      // No filter applied
        LOW_PASS,  // Apply low-pass filter (keep low frequencies)
        HIGH_PASS  // Apply high-pass filter (keep high frequencies/edges)
    };

    /**
     * @brief Create a hybrid image by blending two images with custom filter options.
     *
     * Allows specifying which filter to apply to each image.
     *
     * @param img1          First input image.
     * @param img2          Second input image.
     * @param filterType1   Filter type to apply to first image.
     * @param filterType2   Filter type to apply to second image.
     * @return              Blended hybrid image (CV_8UC1).
     */
    cv::Mat createHybridImage(const cv::Mat& img1, 
                              const cv::Mat& img2,
                              FilterType filterType1,
                              FilterType filterType2);

} 

#endif // FREQUENCY_FILTERS_H