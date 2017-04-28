#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>

// OpenCV libray headers
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loadedImage(),
    savedImage()
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
    savedImage = loadedImage = cv::imread(fileName.toUtf8().toStdString());
    displayMat(loadedImage);
}

void MainWindow::displayMat(cv::Mat displayedImage)
{
    QImage transfromedImg;
    cv::Mat tempRgb;
    // Convert Mat BGR to QImage RGB
    if (displayedImage.channels() == 3)
    {
        cv::cvtColor(displayedImage, tempRgb, CV_BGR2RGB);
        transfromedImg = QImage((const unsigned char*)(tempRgb.data),
                    tempRgb.cols, tempRgb.rows,
                    tempRgb.cols * tempRgb.channels(),
                    QImage::Format_RGB888);
    }
    else
    {
        transfromedImg = QImage((const unsigned char*)(tempRgb.data),
                    tempRgb.cols, tempRgb.rows,
                    tempRgb.cols * tempRgb.channels(),
                    QImage::Format_RGB888);
    }
    scene = new QGraphicsScene(this);
    QPixmap showedPixImg = QPixmap::fromImage(transfromedImg);

    scene->addPixmap(showedPixImg);
    scene->setSceneRect(showedPixImg.rect());
    ui->ImageGraphicsView->setScene(scene);
}

void MainWindow::on_action_Save_triggered()
{
    if (savedImage.empty())
    {
        QMessageBox::information(this, "Can't save null image!", "There is no image to be saved! Please open a new file and process it!");
    }
    else
    {
        QString imagePath = QFileDialog::getSaveFileName(this,
                                                         tr("Save File"),
                                                         ".",
                                                         tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
        if (!imagePath.isEmpty())
        {
            cv::imwrite(imagePath.toStdString(), savedImage);
        }
    }
}
