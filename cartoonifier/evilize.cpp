#include "evilize.h"
#include <opencv2/imgproc.hpp>

using namespace cv;

Evilize::Evilize()
{

}

Mat Evilize::process(const Mat &image)
{
    Mat imageGray;
    cvtColor(image, imageGray, CV_BGR2GRAY);

    // remove pixel noise with a good median filter
    medianBlur(imageGray, imageGray, 7);
    Size size = image.size();
    Mat edges = Mat(size, CV_8U);
    Mat edges2;
    // make image horrible using scharr filter
    Scharr(imageGray, edges, CV_8U, 1, 0);
    Scharr(imageGray, edges2, CV_8U, 1, 0, -1);
    edges += edges2;
    threshold(edges, result, 12, 255, THRESH_BINARY_INV);
    medianBlur(result, result, 3);

    return result;
}
