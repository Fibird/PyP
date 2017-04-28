#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <QGraphicsScene>
#include "cartoonifier/cartoonifier.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_About_triggered();

    void on_action_Painting_triggered();

    void on_action_Sketch_triggered();

    void on_action_Evil_triggered();

    void on_action_Alian_triggered();

private:
    Ui::MainWindow *ui;
    cv::Mat loadedImage;
    QGraphicsScene *scene;
    cv::Mat savedImage;
    Cartoonifier cartoonifier;
    void updateDisplay();
private:
    void displayMat(cv::Mat displayedImage);
};

#endif // MAINWINDOW_H
