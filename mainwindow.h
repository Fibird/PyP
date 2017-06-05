#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/highgui/highgui.hpp>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QFileInfo>
#include <QUndoCommand>
#include <QUndoStack>
#include <QTranslator>
#include <QActionGroup>

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

    void on_actionSave_as_triggered();

    void on_action_Exit_triggered();

private:
    Ui::MainWindow *ui;
    cv::Mat loadedImage;
    QGraphicsScene *scene;
    cv::Mat savedImage;
    QFileInfo fileInfo;
    QUndoStack *undoStack;
    QAction *undoAction;
    QAction *redoAction;
    QTranslator appTranslator;
    QTranslator qtTranslator;
    QString langPath;
    QActionGroup *languageActionGroup;
private:
    void displayMat(cv::Mat displayedImage);
    void updateImage(cv::Mat img);
    void createActions();
    void createMenus();
    void createLanguageMenu();
    void switchLanguage(QAction *action);
};

#endif // MAINWINDOW_H
