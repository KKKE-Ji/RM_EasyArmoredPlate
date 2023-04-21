#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
    namedWindow("Window");
    int slider_value = 0;
    createTrackbar("Slider", "Window", &slider_value, 255);
    while (true) {
        imshow("Window", Mat::zeros(500, 500, CV_8UC3) + Scalar(slider_value, slider_value, slider_value));
        if (waitKey(30) == 27) {
            break;
        }
    }
    return 0;
}
