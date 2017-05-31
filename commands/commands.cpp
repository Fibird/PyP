#include "commands/commands.h"
#include <QDebug>

/*************** sketchize command ***************/
SketchizeCMD::SketchizeCMD(cv::Mat &image, int strokeWidth, QObject *objParent, QUndoCommand *undoParent)
    : QObject(objParent),
      QUndoCommand(undoParent)
{
    oldImg = image.clone();
    skc.setStrokeWidth(strokeWidth);
}

void SketchizeCMD::undo()
{
    newImg = oldImg.clone();
    emit transpImg(newImg);
}

void SketchizeCMD::redo()
{
    newImg = skc.process(oldImg);
    emit transpImg(newImg);
}

/*************** cartoonize command ***************/

CartoonizeCMD::CartoonizeCMD(cv::Mat &image, QObject *objParent, QUndoCommand * undoParent)
    : QObject(objParent),
      QUndoCommand(undoParent)
{
    oldImg = image.clone();
    skc.setStrokeWidth(3);
}

void CartoonizeCMD::undo()
{
    newImg = oldImg.clone();
    emit transpImg(newImg);
}

void CartoonizeCMD::redo()
{
    cv::Mat skcRst = skc.process(oldImg);
    cv::Mat ptRst = pt.process(oldImg);
    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)newImg.data, 0, newImg.step * newImg.rows);
    ptRst.copyTo(newImg, skcRst);
    emit transpImg(newImg);
}

/*************** evilize command ***************/

EvilizeCMD::EvilizeCMD(cv::Mat &image, QObject *objParent, QUndoCommand *undoParent)
    : QObject(objParent),
      QUndoCommand(undoParent)
{
    oldImg = image.clone();
}

void EvilizeCMD::undo()
{
    newImg = oldImg.clone();
    emit transpImg(newImg);
}

void EvilizeCMD::redo()
{
    cv::Mat evlRst = evl.process(oldImg);
    cv::Mat ptRst = pt.process(oldImg);
    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)newImg.data, 0, newImg.step * newImg.rows);
    ptRst.copyTo(newImg, evlRst);
    emit transpImg(newImg);
}
