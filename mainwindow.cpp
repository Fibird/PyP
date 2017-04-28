#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                    ".", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    loadedImage = cv::imread(fileName.toUtf8().toStdString());
    displayMat(loadedImage);
}

void MainWindow::displayMat(cv::Mat displayedImage)
{

    QImage transfromedImg;
    transfromedImg = mat2QImage(displayedImage);
    scene = new QGraphicsScene(this);
    QPixmap showedPixImg = QPixmap::fromImage(transfromedImg);

    scene->addPixmap(showedPixImg);
    scene->setSceneRect(showedPixImg.rect());
    ui->ImageGraphicsView->setScene(scene);
}

QImage MainWindow::mat2QImage(cv::Mat m)
{
    cv::Mat tempRgb;
    QImage q;
    if (m.channels() == 3)
    {
        // Convert Mat BGR to QImage RGB
        cv::cvtColor(m, tempRgb, CV_BGR2RGB);
        q = QImage((const unsigned char*)(tempRgb.data),
                                tempRgb.cols, tempRgb.rows,
                                tempRgb.cols * tempRgb.channels(),
                                QImage::Format_RGB888);
    }
    else
    {
        q = QImage((const unsigned char*)(tempRgb.data),
                                tempRgb.cols, tempRgb.rows,
                                tempRgb.cols * tempRgb.channels(),
                                QImage::Format_RGB888);
    }
    return q;
}

void MainWindow::qImage2Mat(QImage &q, cv::Mat &m)
{

}

void MainWindow::on_action_Save_triggered()
{

}
