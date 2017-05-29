#include "commands/commands.h"

/*************** sketchize command ***************/
SketchizeCMD::SketchizeCMD(cv::Mat &image, int strokeWidth, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    oldImg = image.clone();
    newImg = image;
    skc.setStrokeWidth(strokeWidth);
}

void SketchizeCMD::undo()
{
    newImg = oldImg.clone();
}

void SketchizeCMD::redo()
{
    newImg = skc.process(oldImg);
}

/*************** cartoonize command ***************/

CartoonizeCMD::CartoonizeCMD(cv::Mat &image, QUndoCommand * parent)
    : QUndoCommand(parent)
{
    oldImg = image.clone();
    newImg = image;
    skc.setStrokeWidth(3);
}

void CartoonizeCMD::undo()
{
    newImg = oldImg.clone();
}

void CartoonizeCMD::redo()
{
    cv::Mat skcRst = skc.process(oldImg);
    cv::Mat ptRst = pt.process(oldImg);
    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)newImg.data, 0, newImg.step * newImg.rows);
    ptRst.copyTo(newImg, skcRst);
}

/*************** evilize command ***************/

EvilizeCMD::EvilizeCMD(cv::Mat &image, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    oldImg = image.clone();
    newImg = image;
}

void EvilizeCMD::undo()
{

}

void EvilizeCMD::redo()
{
    cv::Mat evlRst = evl.process(oldImg);
    cv::Mat ptRst = pt.process(oldImg);
    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)newImg.data, 0, newImg.step * newImg.rows);
    ptRst.copyTo(newImg, evlRst);
}
