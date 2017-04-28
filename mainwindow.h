#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <QGraphicsScene>

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

private:
    Ui::MainWindow *ui;
    cv::Mat loadedImage;
    QGraphicsScene *scene;
    cv::Mat savedImage;
private:
    void displayMat(cv::Mat displayedImage);
    QImage mat2QImage(cv::Mat m);
    void qImage2Mat(QImage &q, cv::Mat &m);
};

#endif // MAINWINDOW_H
