#include <opencv2/opencv.hpp>

int predict_color(int h, int s, int v, int g, bool bw);
int get_color_ID(cv::Mat image, bool bw = true);
float* get_color_percent(cv::Mat image, bool bw = true);
cv::Mat get_color_per_pixel_ID(cv::Mat image, bool bw = true);
bool is_in_range(cv::Mat image, int h, int s, int v, int h_delta = 5, int s_delta = 30, int v_delta = 30);
cv::Mat in_range_mask(cv::Mat image, int h, int s, int v, int h_delta = 5, int s_delta = 30, int v_delta = 30);
