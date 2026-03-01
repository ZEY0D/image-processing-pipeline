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
    long long cumSum = 0;
    for (int i = 0; i < 256; ++i) {
        cumSum    += histogram[i];
        cdf[i]     = static_cast<double>(cumSum) / static_cast<double>(total);
    }

    return cdf;
}

// =============================================================================
// Task 8d — Histogram + CDF Plot Rendering
// =============================================================================

/**
 * @brief Internal helper — draws one histogram + CDF panel onto a canvas region.
 *
 * @param canvas    The full plot canvas (modified in-place).
 * @param region    Sub-rectangle within canvas for this panel.
 * @param histogram The 256-bin histogram.
 * @param cdf       The normalised CDF for this channel.
 * @param barColor  BGR colour for the histogram bars.
 * @param cdfColor  BGR colour for the CDF polyline.
 * @param label     Label string placed at the top-left of the panel.
 */
static void drawPanel(cv::Mat& canvas,
                      const cv::Rect& region,
                      const Histogram& histogram,
                      const CDF& cdf,
                      const cv::Scalar& barColor,
                      const cv::Scalar& cdfColor,
                      const std::string& label)
{
    // ---- Layout constants (all in pixel coordinates within the panel) --------
    const int MARGIN_LEFT   = 40;
    const int MARGIN_RIGHT  = 10;
    const int MARGIN_TOP    = 30;
    const int MARGIN_BOTTOM = 30;

    const int plotW = region.width  - MARGIN_LEFT - MARGIN_RIGHT;
    const int plotH = region.height - MARGIN_TOP  - MARGIN_BOTTOM;

    // ---- Background & border ------------------------------------------------
    cv::rectangle(canvas, region, cv::Scalar(30, 30, 30), cv::FILLED);
    cv::rectangle(canvas, region, cv::Scalar(80, 80, 80), 1);

    // ---- Panel label --------------------------------------------------------
    cv::putText(canvas, label,
                cv::Point(region.x + MARGIN_LEFT, region.y + MARGIN_TOP - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(220, 220, 220), 1, cv::LINE_AA);

    // ---- Axis lines ---------------------------------------------------------
    // X-axis (bottom)
    cv::line(canvas,
             cv::Point(region.x + MARGIN_LEFT,           region.y + MARGIN_TOP + plotH),
             cv::Point(region.x + MARGIN_LEFT + plotW,   region.y + MARGIN_TOP + plotH),
             cv::Scalar(150, 150, 150), 1);
    // Y-axis (left)
    cv::line(canvas,
             cv::Point(region.x + MARGIN_LEFT, region.y + MARGIN_TOP),
             cv::Point(region.x + MARGIN_LEFT, region.y + MARGIN_TOP + plotH),
             cv::Scalar(150, 150, 150), 1);

    // ---- Find the peak histogram value for normalisation --------------------
    int maxCount = 0;
    for (int count : histogram) {
        if (count > maxCount) maxCount = count;
    }
    if (maxCount == 0) return; // Nothing to draw

    // ---- Draw histogram bars ------------------------------------------------
    // We map 256 bins onto plotW pixels.  Each bin width may be fractional, so
    // we use floating-point x coordinates and round to integers.
    const double binW = static_cast<double>(plotW) / 256.0;

    for (int i = 0; i < 256; ++i) {
        const int barHeight =
            static_cast<int>(std::round(
                static_cast<double>(histogram[i]) / static_cast<double>(maxCount) * plotH));

        const int x1 = region.x + MARGIN_LEFT + static_cast<int>(std::round(i * binW));
        const int x2 = region.x + MARGIN_LEFT + static_cast<int>(std::round((i + 1) * binW));
        const int y1 = region.y + MARGIN_TOP + plotH - barHeight;
        const int y2 = region.y + MARGIN_TOP + plotH;

        if (x2 > x1) { // Guard against zero-width rectangles
            cv::rectangle(canvas, cv::Point(x1, y1), cv::Point(x2 - 1, y2),
                          barColor, cv::FILLED);
        }
    }

    // ---- Draw CDF polyline --------------------------------------------------
    // CDF values are in [0, 1]; map to [plotH, 0] (inverted y-axis).
    cv::Point prevPt(region.x + MARGIN_LEFT,
                     region.y + MARGIN_TOP + plotH - static_cast<int>(std::round(cdf[0] * plotH)));

    for (int i = 1; i < 256; ++i) {
        const int cx = region.x + MARGIN_LEFT + static_cast<int>(std::round(i * binW));
        const int cy = region.y + MARGIN_TOP  + plotH -
                       static_cast<int>(std::round(cdf[i] * plotH));

        cv::line(canvas, prevPt, cv::Point(cx, cy), cdfColor, 2, cv::LINE_AA);
        prevPt = cv::Point(cx, cy);
    }

    // ---- CDF legend label ---------------------------------------------------
    cv::putText(canvas, "--- CDF",
                cv::Point(region.x + region.width - 75, region.y + MARGIN_TOP + 14),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, cdfColor, 1, cv::LINE_AA);
}

// -----------------------------------------------------------------------------

cv::Mat plotHistogramsAndCDF(const cv::Mat& bgrImage,
                             const std::string& windowTitle,
                             const std::string& saveFilename)
{
    if (bgrImage.empty()) {
        throw std::invalid_argument("plotHistogramsAndCDF: input image is empty.");
    }

    // 1. Compute histograms for all three channels
    Histogram bHist, gHist, rHist;
    calculateHistograms(bgrImage, bHist, gHist, rHist);

    // 2. Compute CDFs
    CDF bCDF = calculateCDF(bHist);
    CDF gCDF = calculateCDF(gHist);
    CDF rCDF = calculateCDF(rHist);

    // 3. Allocate canvas: three panels side by side
    const int PANEL_W  = 360;
    const int PANEL_H  = 220;
    const int PADDING  = 8;
    const int CANVAS_W = PANEL_W * 3 + PADDING * 4;
    const int CANVAS_H = PANEL_H + PADDING * 2;

    cv::Mat canvas(CANVAS_H, CANVAS_W, CV_8UC3, cv::Scalar(20, 20, 20));

    // Panel layout (left-to-right: B, G, R)
    struct PanelInfo {
        const Histogram& hist;
        const CDF&       cdf;
        cv::Scalar       barColor;
        cv::Scalar       cdfColor;
        std::string      label;
    };

    // BGR bars are dimmed slightly so the bright CDF line stands out
    const std::vector<PanelInfo> panels = {
        { bHist, bCDF, cv::Scalar(180,  60,  60), cv::Scalar(255, 180, 100), "Blue Channel" },
        { gHist, gCDF, cv::Scalar( 60, 160,  60), cv::Scalar(100, 255, 180), "Green Channel" },
        { rHist, rCDF, cv::Scalar( 60,  60, 180), cv::Scalar(100, 180, 255), "Red Channel"  }
    };

    for (int p = 0; p < 3; ++p) {
        const cv::Rect region(PADDING + p * (PANEL_W + PADDING), PADDING, PANEL_W, PANEL_H);
        drawPanel(canvas, region,
                  panels[p].hist, panels[p].cdf,
                  panels[p].barColor, panels[p].cdfColor,
                  panels[p].label);
    }

    // 4. Title bar
    cv::putText(canvas, "Task 8: RGB Histograms & CDF",
                cv::Point(PADDING, CANVAS_H - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1, cv::LINE_AA);

    // 5. Save (optional) and display
    if (!saveFilename.empty()) {
        cv::imwrite(saveFilename, canvas);
    }
    cv::imshow(windowTitle, canvas);

    return canvas;
}

} // namespace hist
