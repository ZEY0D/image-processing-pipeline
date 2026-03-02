#include <iostream>
#include <opencv2/opencv.hpp>
#include "crow_all.h"
#include "base64.h"
#include "json.hpp"

// Include your existing headers
#include "noise.h"
#include "filters.h"
#include "edge_detection.h"
#include "histograms.h"
#include "frequency_filters.h"

using namespace cv;
using namespace std;
using json = nlohmann::json;

// ==================== Helpers ====================
string matToBase64(const Mat& img) {
    vector<uchar> buf;
    imencode(".png", img, buf);
    auto* enc_msg = reinterpret_cast<const unsigned char*>(buf.data());
    return base64_encode(enc_msg, buf.size());
}

Mat base64ToMat(const string& b64) {
    string decoded = base64_decode(b64);
    vector<uchar> data(decoded.begin(), decoded.end());
    return imdecode(data, IMREAD_COLOR);
}

// ==================== Main REST API ====================
int main() {
    crow::SimpleApp app;

    // Endpoint to process image
    CROW_ROUTE(app, "/process").methods("POST"_method)([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            string operation = body["operation"];
            Mat img = base64ToMat(body["image"]);
            Mat result;

            // ------------------------
            // Use your existing functions here
            // ------------------------
            if (operation == "salt_pepper_noise") {
                float ratio = static_cast<float>(body["ratio"]);
                result = addSaltAndPepperNoise(img, ratio);
            } 
            else if (operation == "gaussian_noise") {
                double mean = 0.0;  // default mean
                double sigma = static_cast<double>(body["sigma"]);
                result = addGaussianNoise(img, mean, sigma);
            }
            else if (operation == "median_filter") {
                int k = body["kernel"];
                result = applyLowPassFilter(img, "Median", k);
            }
            else if (operation == "sobel") {
                Mat gray; 
                cvtColor(img, gray, COLOR_BGR2GRAY);
                result = edge::applySobel(gray);
            }
            else if (operation == "canny") {
                Mat gray; 
                cvtColor(img, gray, COLOR_BGR2GRAY);
                int th1 = body["th1"];
                int th2 = body["th2"];
                result = edge::applyCanny(gray, th1, th2);
            }
            // Add other operations here if needed

            json res_json;
            res_json["result"] = matToBase64(result);
            return crow::response{res_json.dump()};
        } catch(const std::exception& e) {
            json res_json;
            res_json["error"] = e.what();
            return crow::response{400, res_json.dump()};
        }
    });

    cout << "Server running at http://127.0.0.1:18080" << endl;
    app.port(18080).multithreaded().run();
}