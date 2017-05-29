#ifndef COMMANDS_H
#define COMMANDS_H
#include <QUndoCommand>
#include <opencv2/highgui.hpp>
#include <cartoonifier/sketchize.h>
#include <cartoonifier/paintize.h>
#include <cartoonifier/evilize.h>

class SketchizeCMD : public QUndoCommand
{
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Sketchize skc;
public:
    SketchizeCMD(cv::Mat &image, int strokeWidth = 3, QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;
};

class CartoonizeCMD : public QUndoCommand
{
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Sketchize skc;
    Paintize pt;
public:
    CartoonizeCMD(cv::Mat &image, QUndoCommand * parent = 0);
    void undo() override;
    void redo() override;
};

class EvilizeCMD : public QUndoCommand
{
private:
    cv::Mat oldImg;
    cv::Mat newImg;
    Evilize evl;
    Paintize pt;
public:
    EvilizeCMD(cv::Mat &image, QUndoCommand * parent = 0);
    void undo() override;
    void redo() override;
};

#endif // COMMANDS_H
