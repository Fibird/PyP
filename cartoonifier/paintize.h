#ifndef PAINTIZE_H
#define PAINTIZE_H
#include <opencv2/highgui/highgui.hpp>

class Paintize
{
private:
    cv::Mat result;
public:
    Paintize();
    cv::Mat process(const cv::Mat &image);
};

#endif // PAINTIZE_H
