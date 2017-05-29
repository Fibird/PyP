#ifndef EVILIZE_H
#define EVILIZE_H
#include <opencv2/highgui.hpp>

class Evilize
{
private:
    cv::Mat result;
public:
    Evilize();
    cv::Mat process(const cv::Mat &image);
};

#endif // EVILIZE_H
