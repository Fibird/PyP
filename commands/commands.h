#ifndef COMMANDS_H
#define COMMANDS_H
#include <QUndoCommand>
#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include "cartoonifier/sketchize.h"
#include "cartoonifier/paintize.h"
#include "cartoonifier/evilize.h"

class SketchizeCMD : public QObject, public QUndoCommand
{
    Q_OBJECT
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Sketchize skc;
public:
    SketchizeCMD(cv::Mat &image, int strokeWidth = 3, QObject *objParent = 0, QUndoCommand *undoParent = 0);
    void undo() override;
    void redo() override;
signals:
    void transpImg(cv::Mat &img);
};

class CartoonizeCMD : public QObject, public QUndoCommand
{
    Q_OBJECT
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Sketchize skc;
    Paintize pt;
public:
    CartoonizeCMD(cv::Mat &image, QObject *objParent = 0, QUndoCommand *undoParent = 0);
    void undo() override;
    void redo() override;
signals:
    void transpImg(cv::Mat &img);
};

class EvilizeCMD : public QObject, public QUndoCommand
{
    Q_OBJECT
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Evilize evl;
    Paintize pt;
public:
    EvilizeCMD(cv::Mat &image, QObject *objParent = 0, QUndoCommand *undoParent = 0);
    void undo() override;
    void redo() override;
signals:
    void transpImg(cv::Mat &img);
};

#endif // COMMANDS_H
