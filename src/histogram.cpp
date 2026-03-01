#include "histogram.h"

using namespace cv;
using namespace std;

Mat drawHistogram(const Mat& inputImage) {
    // 1. Convert to grayscale if it isn't already
    Mat grayImg;
    if (inputImage.channels() == 3) {
        cvtColor(inputImage, grayImg, COLOR_BGR2GRAY);
    } else {
        grayImg = inputImage.clone();
    }

    // 2. Set up the parameters for OpenCV's calcHist
    int histSize = 256; // 256 distinct pixel values (0 to 255)
    float range[] = { 0, 256 }; // The range of values
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;

    // 3. Calculate the histogram
    Mat hist;
    calcHist(&grayImg, 1, 0, Mat(), hist, 1, &histSize, histRange, uniform, accumulate);

    // 4. Set up a black canvas to draw our graphs on
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(15, 15, 15)); // Dark gray background

    // 5. Normalize the histogram data so it perfectly fits the height of our canvas
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    // 6. Draw the graphs!
    for (int i = 1; i < histSize; i++) {
        // Draw the Histogram (Gray Bars)
        rectangle(histImage, 
                  Point(bin_w * (i - 1), hist_h), 
                  Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))), 
                  Scalar(100, 100, 100), -1);

        // Draw the Distribution Curve (Bright Green Line connecting the tops)
        line(histImage, 
             Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
             Scalar(0, 255, 0), 2, 8, 0);
    }

    return histImage;
}