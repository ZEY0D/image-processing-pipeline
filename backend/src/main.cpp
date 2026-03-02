#include <iostream>
#include <opencv2/opencv.hpp>
#include "crow_all.h"
#include "base64.h"
#include "json.hpp"

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

// ==================== CORS Helper ====================
crow::response corsResponse(int code, const string& body = "") {
    crow::response res(code, body);
    res.add_header("Access-Control-Allow-Origin",  "*");
    res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Accept");
    res.add_header("Content-Type", "application/json");
    return res;
}

// ==================== Main REST API ====================
int main() {
    crow::SimpleApp app;

    // ------- Preflight (OPTIONS) -------
    CROW_ROUTE(app, "/process").methods("OPTIONS"_method)([]() {
        crow::response res(204);
        res.add_header("Access-Control-Allow-Origin",  "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Accept");
        return res;
    });

    // ------- Main process endpoint -------
    CROW_ROUTE(app, "/process").methods("POST"_method)([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);

            string operation = body.value("operation", "");
            if (operation.empty())
                return corsResponse(400, R"({"error":"Missing 'operation' field"})");

            string imageB64 = body.value("image", "");
            if (imageB64.empty())
                return corsResponse(400, R"({"error":"Missing 'image' field"})");

            Mat img = base64ToMat(imageB64);
            if (img.empty())
                return corsResponse(400, R"({"error":"Could not decode image — check base64 data"})");

            Mat result;

            // -------- Noise --------
            if (operation == "salt_pepper_noise") {
                float ratio = body.value("ratio", 0.05f);
                result = addSaltAndPepperNoise(img, ratio);
            }
            else if (operation == "gaussian_noise") {
                double sigma = body.value("sigma", 25.0);
                result = addGaussianNoise(img, 0.0, sigma);
            }
            // -------- Spatial Filters --------
            else if (operation == "average_filter") {
                int k = body.value("kernel", 3);
                result = applyLowPassFilter(img, "Average", k);
            }
            else if (operation == "gaussian_filter") {
                int k = body.value("kernel", 3);
                result = applyLowPassFilter(img, "Gaussian", k);
            }
            else if (operation == "median_filter") {
                int k = body.value("kernel", 3);
                result = applyLowPassFilter(img, "Median", k);
            }
            // -------- Edge Detection --------
            else if (operation == "sobel") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                result = edge::applySobel(gray);
            }
            else if (operation == "prewitt") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                result = edge::applyPrewitt(gray);
            }
            else if (operation == "roberts") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                result = edge::applyRoberts(gray);
            }
            else if (operation == "canny") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                int th1 = body.value("th1", 50);
                int th2 = body.value("th2", 150);
                result = edge::applyCanny(gray, th1, th2);
            }
            // -------- Histogram & Equalization --------
            else if (operation == "histogram") {
                // Returns a rendered histogram plot as an image
                result = hist::plotHistogramsAndCDF(img, "Histogram & CDF", "");
            }
            else if (operation == "equalize") {
                // Equalize each channel individually
                vector<Mat> channels;
                split(img, channels);
                for (auto& ch : channels)
                    equalizeHist(ch, ch);
                merge(channels, result);
            }
            // -------- Grayscale --------
            else if (operation == "grayscale") {
                Mat gray = hist::toGrayscale(img);
                // Convert back to 3-channel so the frontend can display normally
                cvtColor(gray, result, COLOR_GRAY2BGR);
            }
            // -------- Frequency Domain --------
            else if (operation == "frequency_lpf") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                float cutoff = body.value("cutoff", 30.0f);
                Mat filtered = freq::applyLowPassFilter(gray, cutoff);
                cvtColor(filtered, result, COLOR_GRAY2BGR);
            }
            else if (operation == "frequency_hpf") {
                Mat gray;
                cvtColor(img, gray, COLOR_BGR2GRAY);
                float cutoff = body.value("cutoff", 30.0f);
                Mat filtered = freq::applyHighPassFilter(gray, cutoff);
                cvtColor(filtered, result, COLOR_GRAY2BGR);
            }
            else {
                json err;
                err["error"] = "Unknown operation: " + operation;
                return corsResponse(400, err.dump());
            }

            if (result.empty())
                return corsResponse(500, R"({"error":"Processing produced an empty image"})");

            json res_json;
            res_json["result"] = matToBase64(result);
            return corsResponse(200, res_json.dump());

        } catch (const json::parse_error& e) {
            json err;
            err["error"] = string("JSON parse error: ") + e.what();
            return corsResponse(400, err.dump());
        } catch (const std::exception& e) {
            json err;
            err["error"] = string("Processing error: ") + e.what();
            return corsResponse(500, err.dump());
        }
    });

    cout << "Server running at http://127.0.0.1:18080" << endl;
    app.port(18080).multithreaded().run();
}