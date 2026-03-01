#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H

#include <array>
#include <opencv2/opencv.hpp>

/**
 * @file histograms.h
 * @brief Task 8 — RGB Histograms, CDF Calculation, and Plot Rendering.
 */

namespace hist {

    // -------------------------------------------------------------------------
    // custom shortcuts for lists of 256 numbers to represent color counts and statistics.
    // -------------------------------------------------------------------------
    using Histogram = std::array<int, 256>;
    using CDF       = std::array<double, 256>;

    /**
     * @brief Convert a BGR color image to grayscale manually.
     *
     * Uses the luminosity-weighted formula:
     *   Gray = 0.299*R + 0.587*G + 0.114*B
     *
     * @param bgrImage  Input BGR color image (CV_8UC3).
     * @return          Single-channel grayscale image (CV_8UC1).
     */
    cv::Mat toGrayscale(const cv::Mat& bgrImage);

    /**
     * @brief Manually compute 256-bin histograms for each BGR channel.
     *
     * Iterates over every pixel with cv::Mat::at<cv::Vec3b> and tallies
     * the intensity count into three separate 256-element arrays.
     *
     * @param bgrImage  Input BGR color image (CV_8UC3).
     * @param bHist     Output histogram for the Blue channel.
     * @param gHist     Output histogram for the Green channel.
     * @param rHist     Output histogram for the Red channel.
     */
    void calculateHistograms(const cv::Mat& bgrImage,
                             Histogram& bHist,
                             Histogram& gHist,
                             Histogram& rHist);

    /**
     * @brief Compute the Cumulative Distribution Function (CDF) from a histogram.
     *
     * Normalises the running cumulative sum so that the final value is 1.0.
     *
     * @param histogram  256-bin histogram (counts).
     * @return           Normalised CDF (values in [0, 1]).
     */
    CDF calculateCDF(const Histogram& histogram);

    /**
     * @brief Render a three-panel histogram + CDF plot and display / save it.
     *
     * Produces one horizontal strip containing three sub-panels (R, G, B).
     * Each panel shows:
     *   - A bar histogram drawn with cv::rectangle.
     *   - The corresponding CDF overlaid as a connected polyline with cv::line.
     *
     * @param bgrImage      Input BGR color image (CV_8UC3).
     * @param windowTitle   Window title used for cv::imshow.
     * @param saveFilename  If non-empty the plot is saved to this path.
     * @return              The rendered plot as a cv::Mat (CV_8UC3).
     */
    cv::Mat plotHistogramsAndCDF(const cv::Mat& bgrImage,
                                 const std::string& windowTitle  = "Histogram & CDF",
                                 const std::string& saveFilename = "");

}

#endif // HISTOGRAMS_H
