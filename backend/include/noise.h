#ifndef NOISE_H
#define NOISE_H

#include <opencv2/opencv.hpp>

// --- Noise Functions ---

// Salt and Pepper Noise: 'ratio' is the probability of a pixel being affected (0.0 to 1.0)
cv::Mat addSaltAndPepperNoise(const cv::Mat& input, float ratio);

// Gaussian Noise: 'sigma' controls the intensity/spread of the noise
cv::Mat addGaussianNoise(const cv::Mat& input, double mean, double sigma);

// Uniform Noise: 'alpha' controls the intensity of the noise (0.0 to 1.0)
cv::Mat addUniformNoise(const cv::Mat& input, float alpha);

#endif // NOISE_H

