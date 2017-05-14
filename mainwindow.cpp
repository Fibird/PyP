#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDebug>
#include <QColorDialog>

// OpenCV libray headers
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loadedImage(),
    savedImage(),
    cartoonifier()
{
    ui->setupUi(this);
    fileInfo = QFileInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString fileFullPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                    ".", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    this->fileInfo = QFileInfo(fileFullPath);

    if (!fileFullPath.isEmpty())
    {
        loadedImage = cv::imread(fileFullPath.toUtf8().toStdString());

        cartoonifier.setInputImage(fileFullPath.toStdString());
        displayMat(cartoonifier.getInputMat());
    }
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
    savedImage = cartoonifier.getLastResult();
    if (savedImage.empty())
    {
        QMessageBox::information(this, "Can't save null image!", "There is no image to be saved! Please open a new file and process it!");
    }
    else
    {
        cv::imwrite(fileInfo.filePath().toStdString(), savedImage);
    }
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About this application"), tr("Copyright(c)2017 Chaoyang Liu, Yue Shi.\nThis software is under GPLv3.0 license."));
}

void MainWindow::on_action_Painting_triggered()
{
    if (!cartoonifier.getInputMat().empty())
    {
        //cartoonifier.cartoonifyImage(false, false, false, false);
        cartoonifier.paintingProcess();
        updateDisplay();
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
}

void MainWindow::updateDisplay()
{
    displayMat(cartoonifier.getLastResult());
}

void MainWindow::on_action_Sketch_triggered()
{
    if (!cartoonifier.getInputMat().empty())
    {
        //cartoonifier.cartoonifyImage(true, false, false, false);
        cartoonifier.sketchProcess();
        updateDisplay();
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
}

void MainWindow::on_action_Evil_triggered()
{
    if (!cartoonifier.getInputMat().empty())
    {
        //cartoonifier.cartoonifyImage(false, false, true, false);
        cartoonifier.evilProcess();
        updateDisplay();
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
}

void MainWindow::on_action_Alian_triggered()
{
    if (!cartoonifier.getInputMat().empty())
    {
        //cartoonifier.cartoonifyImage(false, true, false, false);
        QColor color = QColorDialog::getColor(Qt::green, this, "Set face skin color");
        if (!color.isValid())
            color = Qt::green;
        cartoonifier.alienProcess(color.blue(), color.green(), color.red());
        updateDisplay();
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
}

void MainWindow::on_action_undo_triggered()
{
    //cartoonifier.setInputImage(loadedImage);
    cartoonifier.resetResult();
    updateDisplay();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        if(event->delta() > 0)
        {
            ui->ImageGraphicsView->scale(1.1, 1.1);
        }
        else
        {
            ui->ImageGraphicsView->scale(0.9, 0.9);
        }
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    savedImage = cartoonifier.getLastResult();
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

void MainWindow::on_action_Exit_triggered()
{
    if (!(QMessageBox::information(this, tr("exit"), tr("Do you really want to exit?"), tr("Yes"), tr("No"))))
    {
        this->close();
    }
}
