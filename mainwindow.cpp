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
#include <QKeySequence>
#include <QIcon>
#include <commands/commands.h>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loadedImage(),
    savedImage(),
    cartoonifier()
{
    ui->setupUi(this);
    fileInfo = QFileInfo();
    undoStack = new QUndoStack(this);
    createActions();
    createMenus();
}

void MainWindow::createActions()
{
    // undo and redo actions
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(QIcon(tr(":/menu_icon/undo-icon")));

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(QIcon(tr(":/menu_icon/redo-icon")));
}

void MainWindow::createMenus()
{
    ui->mainToolBar->addAction(undoAction);
    ui->mainToolBar->addAction(redoAction);
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
        transfromedImg = QImage((const unsigned char*)(displayedImage.data),
                    displayedImage.cols, displayedImage.rows,
                    displayedImage.cols * displayedImage.channels(),
                    QImage::Format_Indexed8);
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
        CartoonizeCMD *ctCmd = new CartoonizeCMD(loadedImage);
        connect(ctCmd, &CartoonizeCMD::transpImg, this, &MainWindow::updateImage);
        undoStack->push(ctCmd);
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
}

void MainWindow::updateDisplay()
{
    //displayMat(cartoonifier.getLastResult());
}

void MainWindow::updateImage(Mat img)
{
    savedImage = img;
    displayMat(img);
}

void MainWindow::on_action_Sketch_triggered()
{
    if (!cartoonifier.getInputMat().empty())
    {
        SketchizeCMD *skcCmd = new SketchizeCMD(loadedImage);
        connect(skcCmd, &SketchizeCMD::transpImg, this, &MainWindow::updateImage);
        undoStack->push(skcCmd);
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
        EvilizeCMD *elCmd = new EvilizeCMD(loadedImage);
        connect(elCmd, &EvilizeCMD::transpImg, this, &MainWindow::updateImage);
        undoStack->push(elCmd);
    }
    else
    {
        QMessageBox::information(this, "No Image", "There is no image to be processed!");
    }
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
