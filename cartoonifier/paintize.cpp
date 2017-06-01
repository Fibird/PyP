#include "paintize.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

Paintize::Paintize()
{

}

Mat Paintize::process(const Mat &image)
{
    Size size = image.size();
    Mat result = image.clone();

    Size smallSize;
    smallSize.width = size.width / 2;
    smallSize.height = size.height / 2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    // scale image 1/4 smaller
    resize(result, smallImg, smallSize, 0,0, INTER_LINEAR);

    // do the bilateral filtering at a shrunken scale to speed it up
    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;        // Repetitions for strong cartoon effect.
    for (int i=0; i<repetitions; i++)
    {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 15;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }

    // Go back to the original scale.
    resize(smallImg, result, size, 0,0, INTER_LINEAR);
    return result;
}
