#ifndef CARTOONIFIER_H
#define CARTOONIFIER_H
#include <iostream>
#include <opencv2/highgui.hpp>
#include "imageUtils/imageutils.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace std;
using namespace cv;

class Cartoonifier
{
public:
    Cartoonifier();
    bool setInputImage(std::string filename);
    void setInputImage(Mat img);
    const Mat getInputMat() const;
    const Mat getLastResult() const;
    void resetResult();
    Mat getEvil();
    Mat getSketch();
    void cartoonifyImage(bool sketchMode, bool alienMode, bool evilMode, int debugType);
    void paintingProcess();
    void alienProcess();
    void sketchProcess();
    void evilProcess();
    ~Cartoonifier();
private:
    Mat image;
    Mat result;
    ImageUtils imageUtils;
private:
    void changeFacialSkinColor(Mat smallImgBGR, Mat bigEdges, Point *p);
    void removePepperNoise(Mat &mask);
    void detectAndDraw(Mat& img, Mat &rst, CascadeClassifier& cascade,
        double scale, bool tryflip, Point *p);
};

#endif // CARTOONIFIER_H
