#include "mainwindow.h"
#include <QApplication>
#include "cartoon.h"

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    //return a.exec();
    Mat image = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
    Mat dst = image.clone();
    cartoonifyImage(image, dst, false, false, false, 0);
    namedWindow("Cartoon");
    imshow("cartoon", dst);
    waitKey(0);
    return 0;
}
