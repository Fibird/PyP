#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <QGraphicsScene>
#include "cartoonifier/cartoonifier.h"
#include <QWheelEvent>
#include <QFileInfo>
#include <QUndoCommand>
#include <QUndoStack>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void wheelEvent(QWheelEvent *event);
private slots:
    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_About_triggered();

    void on_action_Painting_triggered();

    void on_action_Sketch_triggered();

    void on_action_Evil_triggered();

    void on_action_undo_triggered();

    void on_actionSave_as_triggered();

    void on_action_Exit_triggered();

private:
    Ui::MainWindow *ui;
    cv::Mat loadedImage;
    QGraphicsScene *scene;
    cv::Mat savedImage;
    Cartoonifier cartoonifier;
    QFileInfo fileInfo;
    QUndoStack *undoStack;
    QAction *undoAction;
    QAction *redoAction;
private:
    void displayMat(Mat &displayedImage);
    void updateDisplay();
    void createActions();
    void createMenus();
};

#endif // MAINWINDOW_H
