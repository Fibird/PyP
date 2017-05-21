#ifndef SKETCHIZE_H
#define SKETCHIZE_H
#include <opencv2/highgui.hpp>

class Sketchize
{
private:
    int strokeWidth;
    cv::Mat result;
public:
    Sketchize();
    inline void setStrokeWidth(int w);
    inline int getStrokeWidth() const;
    cv::Mat process(const cv::Mat &image);
private:
    void removePepperNoise(Mat &mask);
};

#endif // SKETCHIZE_H
