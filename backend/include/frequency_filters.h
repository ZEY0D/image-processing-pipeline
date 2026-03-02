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
    // Task 9 — Ideal Low Pass Filter
    // =========================================================================

    /**
     * @brief Apply an ideal low-pass filter in the frequency domain.
     *
     * Steps performed internally:
     *  1. Pad the input to the optimal DFT size.
     *  2. Run cv::dft to obtain the spectrum.
     *  3. Apply fourierShift to centre the DC component.
     *  4. Manually generate a circular binary mask:
     *       H(u,v) = 1  if  D(u,v) <= cutoff
     *               0  otherwise
     *     where D(u,v) = sqrt((u - M/2)^2 + (v - N/2)^2).
     *  5. Manually multiply the complex spectrum by the mask element-wise.
     *  6. Inverse-shift and apply cv::idft to return to the spatial domain.
     *  7. Crop back to the original image size.
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @param cutoff     Radius of the pass-band in pixels (frequency domain).
     * @return           Filtered image (CV_8UC1, same size as input).
     */
    cv::Mat applyLowPassFilter(const cv::Mat& grayImage, float cutoff);

    // =========================================================================
    // Task 9 — Ideal High Pass Filter
    // =========================================================================

    /**
     * @brief Apply an ideal high-pass filter in the frequency domain.
     *
     * Identical pipeline to applyLowPassFilter, but the mask is inverted:
     *   H(u,v) = 0  if  D(u,v) <= cutoff
     *            1  otherwise
     *
     * @param grayImage  Single-channel 8-bit input image (CV_8UC1).
     * @param cutoff     Radius of the stop-band in pixels (frequency domain).
     * @return           Filtered image (CV_8UC1, same size as input).
     */
    cv::Mat applyHighPassFilter(const cv::Mat& grayImage, float cutoff);

    // =========================================================================
    // Task 10 — Hybrid Image
    // =========================================================================

    /**
     * @brief Create a hybrid image by blending two pre-processed images.
     *
     * Accepts any two images (colour or grayscale, already filtered or not).
     * Each input is automatically converted to grayscale if it is not already.
     * If the images differ in size, img2 is resized to match img1.
     * The two grayscale images are then blended with equal weights (0.5 + 0.5)
     * and the result is normalised to [0, 255].
     *
     * Apply your own LPF / HPF (or any other processing) to the images
     * before passing them here — this function does NOT apply any filters.
     *
     * @param img1  First input (e.g. low-pass filtered image).
     * @param img2  Second input (e.g. high-pass filtered image).
     * @return      Blended hybrid image (CV_8UC1).
     */
    cv::Mat createHybridImage(const cv::Mat& img1, const cv::Mat& img2);

} 

#endif // FREQUENCY_FILTERS_H
