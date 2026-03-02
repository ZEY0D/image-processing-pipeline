#include "edge_detection.h"
#include <cmath>
#include <iostream>

namespace edge {

    // --- THE CORE ENGINE (From Scratch) ---
    // This function slides any kernel over the image to calculate gradients.
    cv::Mat customConvolve(const cv::Mat& input, const cv::Mat& kernel) {
        // Create an empty image of the same size, using 32-bit floats for accurate math
        cv::Mat output = cv::Mat::zeros(input.size(), CV_32F);
        
        int kRows = kernel.rows;
        int kCols = kernel.cols;
        int padH = kRows / 2;
        int padW = kCols / 2;

        // Pad the image borders so the kernel doesn't crash when it hits the edges
        cv::Mat padded;
        cv::copyMakeBorder(input, padded, padH, padH, padW, padW, cv::BORDER_REPLICATE);

        // Slide the kernel over every pixel in the original image
        for (int i = 0; i < input.rows; ++i) {
            for (int j = 0; j < input.cols; ++j) {
                float sum = 0.0f;
                
                // Multiply pixel values by kernel weights
                for (int ki = 0; ki < kRows; ++ki) {
                    for (int kj = 0; kj < kCols; ++kj) {
                        float pixel = padded.at<float>(i + ki, j + kj);
                        float weight = kernel.at<float>(ki, kj);
                        sum += pixel * weight;
                    }
                }
                output.at<float>(i, j) = sum; // Store the result
            }
        }
        return output;
    }

    // --- 1. SOBEL EDGE DETECTOR ---
    cv::Mat applySobel(const cv::Mat& inputImage) {
        // 1. Convert image to float to prevent numbers from wrapping around during math
        cv::Mat floatImg;
        inputImage.convertTo(floatImg, CV_32F, 1.0 / 255.0);

        // 2. Define the Sobel X kernel (detects vertical edges)
        cv::Mat kernelX = (cv::Mat_<float>(3, 3) << 
            -1,  0,  1,
            -2,  0,  2,
            -1,  0,  1);

        // 3. Define the Sobel Y kernel (detects horizontal edges)
        cv::Mat kernelY = (cv::Mat_<float>(3, 3) << 
            -1, -2, -1,
             0,  0,  0,
             1,  2,  1);

        // 4. Apply our custom convolution engine
        cv::Mat gradX = customConvolve(floatImg, kernelX);
        cv::Mat gradY = customConvolve(floatImg, kernelY);

        // 5. Calculate the final magnitude: sqrt(Gx^2 + Gy^2)
        cv::Mat magnitude = cv::Mat::zeros(inputImage.size(), CV_32F);
        for (int i = 0; i < magnitude.rows; ++i) {
            for (int j = 0; j < magnitude.cols; ++j) {
                float gx = gradX.at<float>(i, j);
                float gy = gradY.at<float>(i, j);
                magnitude.at<float>(i, j) = std::sqrt(gx * gx + gy * gy);
            }
        }

        // 6. Convert back to a normal 8-bit image (0-255) so it can be displayed
        cv::Mat finalImage;
        cv::normalize(magnitude, finalImage, 0, 255, cv::NORM_MINMAX);
        finalImage.convertTo(finalImage, CV_8U);

        return finalImage;
    }




    // --- 2. PREWITT EDGE DETECTOR (From Scratch) ---
    cv::Mat applyPrewitt(const cv::Mat& inputImage) {
        cv::Mat floatImg;
        inputImage.convertTo(floatImg, CV_32F, 1.0 / 255.0);

        // Prewitt X kernel
        cv::Mat kernelX = (cv::Mat_<float>(3, 3) << 
            -1,  0,  1,
            -1,  0,  1,
            -1,  0,  1);

        // Prewitt Y kernel
        cv::Mat kernelY = (cv::Mat_<float>(3, 3) << 
            -1, -1, -1,
             0,  0,  0,
             1,  1,  1);

        cv::Mat gradX = customConvolve(floatImg, kernelX);
        cv::Mat gradY = customConvolve(floatImg, kernelY);

        cv::Mat magnitude = cv::Mat::zeros(inputImage.size(), CV_32F);
        for (int i = 0; i < magnitude.rows; ++i) {
            for (int j = 0; j < magnitude.cols; ++j) {
                float gx = gradX.at<float>(i, j);
                float gy = gradY.at<float>(i, j);
                magnitude.at<float>(i, j) = std::sqrt(gx * gx + gy * gy);
            }
        }

        cv::Mat finalImage;
        cv::normalize(magnitude, finalImage, 0, 255, cv::NORM_MINMAX);
        finalImage.convertTo(finalImage, CV_8U);
        return finalImage;
    }

    // --- 3. ROBERTS CROSS EDGE DETECTOR (From Scratch) ---
    cv::Mat applyRoberts(const cv::Mat& inputImage) {
        cv::Mat floatImg;
        inputImage.convertTo(floatImg, CV_32F, 1.0 / 255.0);

        // Roberts kernels are 2x2 instead of 3x3
        cv::Mat kernelX = (cv::Mat_<float>(2, 2) << 
             1,  0,
             0, -1);

        cv::Mat kernelY = (cv::Mat_<float>(2, 2) << 
             0,  1,
            -1,  0);

        cv::Mat gradX = customConvolve(floatImg, kernelX);
        cv::Mat gradY = customConvolve(floatImg, kernelY);

        cv::Mat magnitude = cv::Mat::zeros(inputImage.size(), CV_32F);
        for (int i = 0; i < magnitude.rows; ++i) {
            for (int j = 0; j < magnitude.cols; ++j) {
                float gx = gradX.at<float>(i, j);
                float gy = gradY.at<float>(i, j);
                magnitude.at<float>(i, j) = std::sqrt(gx * gx + gy * gy);
            }
        }

        cv::Mat finalImage;
        cv::normalize(magnitude, finalImage, 0, 255, cv::NORM_MINMAX);
        finalImage.convertTo(finalImage, CV_8U);
        return finalImage;
    }

    // --- 4. CANNY EDGE DETECTOR (OpenCV allowed) ---
    cv::Mat applyCanny(const cv::Mat& inputImage, double lowThreshold, double highThreshold) {
        cv::Mat edges;
        // Canny is a built-in OpenCV algorithm, so we just call it directly
        cv::Canny(inputImage, edges, lowThreshold, highThreshold);
        return edges;
    }
}