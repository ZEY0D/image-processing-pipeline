#include "noise.h"
#include <opencv2/opencv.hpp>
#include <random>

using namespace cv;
using namespace std;

// --- Adding Noise ---

// Salt and Pepper Noise: 'ratio' is the probability of a pixel being affected (0.0 to 1.0)
Mat addSaltAndPepperNoise(const Mat& input, float ratio) {
    Mat output = input.clone();
    int total_pixels = output.rows * output.cols * output.channels();
    int noise_pixels = static_cast<int>(ratio * total_pixels);

    for (int i = 0; i < noise_pixels; ++i) {
        int r = rand() % output.rows;
        int c = rand() % output.cols;
        int chan = rand() % output.channels();

        // Randomly choose Salt (255) or Pepper (0)
        output.ptr<uchar>(r)[c * output.channels() + chan] = (rand() % 2 == 0) ? 255 : 0;
    }
    return output;
}

// Gaussian Noise: 'sigma' controls the intensity/spread of the noise
Mat addGaussianNoise(const Mat& input, double mean, double sigma) {
    Mat noise = Mat(input.size(), input.type());
    randn(noise, mean, sigma); // OpenCV built-in for Gaussian distribution
    Mat output;
    add(input, noise, output);
    return output;
}

// Uniform Noise: 'alpha' controls the intensity of the noise (0.0 to 1.0)
Mat addUniformNoise(const Mat& input, float alpha) {
    Mat noise = Mat(input.size(), input.type());
    randu(noise, 0, 255); // OpenCV built-in for uniform distribution (0-255)
    noise.convertTo(noise, CV_32F);
    noise = noise * alpha; // Scale by alpha

    Mat output;
    Mat input_float;
    input.convertTo(input_float, CV_32F);
    add(input_float, noise, output);

    // Clip values to valid range [0, 255]
    output.convertTo(output, input.type());
    return output;
}

