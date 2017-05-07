#ifndef CARTOONIFIER_H
#define CARTOONIFIER_H
#include <iostream>
#include <opencv2/highgui.hpp>

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
private:
    void removePepperNoise(Mat &mask);
};

#endif // CARTOONIFIER_H
