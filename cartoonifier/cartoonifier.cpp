#include "cartoonifier.h"
#include "opencv2/imgproc.hpp"

Cartoonifier::Cartoonifier()
{

}

bool Cartoonifier::setInputImage(std::string filename)
{
    image = cv::imread(filename);
    if (!image.data)
        return false;
    else
        return true;
}

const Mat Cartoonifier::getInputMat() const
{
    return image;
}

// Convert the given photo into a cartoon-like or painting-like image.
// Set sketchMode to true if you want a line drawing instead of a painting.
// Set alienMode to true if you want alien skin instead of human.
// Set evilMode to true if you want an "evil" character instead of a "good" character.
// Set debugType to 1 to show where skin color is taken from, and 2 to show the skin mask in a new window (for desktop).
void Cartoonifier::cartoonifyImage(bool sketchMode, bool alienMode, bool evilMode, int debugType)
{
    // Convert from BGR color to Grayscale
    Mat srcGray;
    Mat srcColor = image.clone();
    cvtColor(srcColor, srcGray, CV_BGR2GRAY);

    // Remove the pixel noise with a good Median filter, before we start detecting edges.
    medianBlur(srcGray, srcGray, 7);

    Size size = image.size();
    Mat mask = Mat(size, CV_8U);
    Mat edges = Mat(size, CV_8U);

    if (!evilMode) {
        // Generate a nice edge mask, similar to a pencil line drawing.
        Laplacian(srcGray, edges, CV_8U, 5);
        threshold(edges, mask, 80, 255, THRESH_BINARY_INV);
        // Mobile cameras usually have lots of noise, so remove small
        // dots of black noise from the black & white edge mask.
        removePepperNoise(mask);
    }
    else {
        // Evil mode, making everything look like a scary bad guy.
        // (Where "srcGray" is the original grayscale image plus a medianBlur of size 7x7).
        Mat edges2;
        Scharr(srcGray, edges, CV_8U, 1, 0);
        Scharr(srcGray, edges2, CV_8U, 1, 0, -1);
        edges += edges2;
        threshold(edges, mask, 12, 255, THRESH_BINARY_INV);
        medianBlur(mask, mask, 3);
    }
    //imshow("edges", edges);
    //imshow("mask", mask);

    // For sketch mode, we just need the mask!
    if (sketchMode) {
        // The output image has 3 channels, not a single channel.
        cvtColor(mask, result, CV_GRAY2BGR);
        return;
    }

    // Do the bilateral filtering at a shrunken scale, since it
    // runs so slowly but doesn't need full resolution for a good effect.
    Size smallSize;
    smallSize.width = size.width/2;
    smallSize.height = size.height/2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    resize(srcColor, smallImg, smallSize, 0,0, INTER_LINEAR);

    // Perform many iterations of weak bilateral filtering, to enhance the edges
    // while blurring the flat regions, like a cartoon.
    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;        // Repetitions for strong cartoon effect.
    for (int i=0; i<repetitions; i++) {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 9;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }
    bool tryflip = false;
    CascadeClassifier cascade, nestedCascade;
    double scale = 1;
    Point *p = new Point[3];
    string cascadeName = "haarcascade_frontalface_alt.xml";
    string nestedCascadeName = "haarcascade_eye.xml";
    if (!nestedCascade.load(nestedCascadeName))
        cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
    if (!cascade.load(cascadeName))
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        //return -1;
    }
    if (alienMode) {
        // Apply an "alien" filter, when given a shrunken image and the full-res edge mask.
        // Detects the color of the pixels in the middle of the image, then changes the color of that region to green.
        //detectAndDraw(smallImg, cascade, nestedCascade, scale, tryflip, p);
        changeFacialSkinColor(smallImg, edges, p);
    }

    // Go back to the original scale.
    resize(smallImg, srcColor, size, 0,0, INTER_LINEAR);

    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)result.data, 0, result.step * result.rows);

    // Use the blurry cartoon image, except for the strong edges that we will leave black.
    srcColor.copyTo(result, mask);
}



// Apply an "alien" filter, when given a shrunken BGR image and the full-res edge mask.
// Detects the color of the pixels in the middle of the image, then changes the color of that region to green.
void Cartoonifier::changeFacialSkinColor(Mat smallImgBGR, Mat bigEdges, Point *p)
{
        int debugType = 1;
        // Convert to Y'CrCb color-space, since it is better for skin detection and color adjustment.
        Mat yuv = Mat(smallImgBGR.size(), CV_8UC3);
        cvtColor(smallImgBGR, yuv, CV_BGR2YCrCb);

        // The floodFill mask has to be 2 pixels wider and 2 pixels taller than the small image.
        // The edge mask is the full src image size, so we will shrink it to the small size,
        // storing into the floodFill mask data.
        int sw = smallImgBGR.cols;
        int sh = smallImgBGR.rows;
        Mat maskPlusBorder = Mat::zeros(sh+2, sw+2, CV_8U);
        Mat mask = maskPlusBorder(Rect(1,1,sw,sh));  // mask is a ROI in maskPlusBorder.
        resize(bigEdges, mask, smallImgBGR.size());

        // Make the mask values just 0 or 255, to remove weak edges.
        threshold(mask, mask, 80, 255, THRESH_BINARY);
        // Connect the edges together, if there was a pixel gap between them.
        dilate(mask, mask, Mat());
        erode(mask, mask, Mat());
        //imshow("constraints for floodFill", mask);

        // YCrCb Skin detector and color changer using multiple flood fills into a mask.
        // Apply flood fill on many points around the face, to cover different shades & colors of the face.
        // Note that these values are dependent on the face outline, drawn in drawFaceStickFigure().
        int const NUM_SKIN_POINTS = 6;
        Point skinPts[NUM_SKIN_POINTS];
        double delta_horiz = sw/11;
        double delta_verti = sh / 6;

        skinPts[0] = Point(p[0].x,          p[0].y - sh/6);
        skinPts[1] = Point(p[0].x - sw/11,  p[0].y - sh/6);
        skinPts[2] = Point(p[0].x + sw/11,  p[0].y - sh/6);
        skinPts[3] = Point(p[0].x,          p[0].y + sh/16);
        skinPts[4] = Point(p[0].x - sw/9,   p[0].y + sh/16);
        skinPts[5] = Point(p[0].x + sw/9,   p[0].y + sh/16);
        // Skin might be fairly dark, or slightly less colorful.
        // Skin might be very bright, or slightly more colorful but not much more blue.
        const int LOWER_Y = 60;
        const int UPPER_Y = 80;
        const int LOWER_Cr = 25;
        const int UPPER_Cr = 15;
        const int LOWER_Cb = 20;
        const int UPPER_Cb = 15;
        Scalar lowerDiff = Scalar(LOWER_Y, LOWER_Cr, LOWER_Cb);
        Scalar upperDiff = Scalar(UPPER_Y, UPPER_Cr, UPPER_Cb);
        // Instead of drawing into the "yuv" image, just draw 1's into the "maskPlusBorder" image, so we can apply it later.
        // The "maskPlusBorder" is initialized with the edges, because floodFill() will not go across non-zero mask pixels.
        Mat edgeMask = mask.clone();    // Keep an duplicate copy of the edge mask.

        for (int i=0; i<NUM_SKIN_POINTS; i++) {
            // Use the floodFill() mode that stores to an external mask, instead of the input image.
            const int flags = 4 | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY | (128 << 8);
            floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, flags);
        }

        if (debugType >= 2)
            imshow("flood mask", mask*120); // Draw the edges as white and the skin region as grey.

        // After the flood fill, "mask" contains both edges and skin pixels, whereas
        // "edgeMask" just contains edges. So to get just the skin pixels, we can remove the edges from it.
        mask -= edgeMask;
        // "mask" now just contains 1's in the skin pixels and 0's for non-skin pixels.

        // Change the color of the skin pixels in the given BGR image.
        int Red = 0;
        int Green = 0;
        int Blue = 70;
        add(smallImgBGR, Scalar(Blue, Green, Red), smallImgBGR, mask);
}


// Remove black dots (upto 4x4 in size) of noise from a pure black & white image.
// ie: The input image should be mostly white (255) and just contains some black (0) noise
// in addition to the black (0) edges.
void Cartoonifier::removePepperNoise(Mat &mask)
{
    // For simplicity, ignore the top & bottom row border.
    for (int y=2; y<mask.rows-2; y++) {
        // Get access to each of the 5 rows near this pixel.
        uchar *pThis = mask.ptr(y);
        uchar *pUp1 = mask.ptr(y-1);
        uchar *pUp2 = mask.ptr(y-2);
        uchar *pDown1 = mask.ptr(y+1);
        uchar *pDown2 = mask.ptr(y+2);

        // For simplicity, ignore the left & right row border.
        pThis += 2;
        pUp1 += 2;
        pUp2 += 2;
        pDown1 += 2;
        pDown2 += 2;
        for (int x=2; x<mask.cols-2; x++) {
            uchar v = *pThis;   // Get the current pixel value (either 0 or 255).
            // If the current pixel is black, but all the pixels on the 2-pixel-radius-border are white
            // (ie: it is a small island of black pixels, surrounded by white), then delete that island.
            if (v == 0) {
                bool allAbove = *(pUp2 - 2) && *(pUp2 - 1) && *(pUp2) && *(pUp2 + 1) && *(pUp2 + 2);
                bool allLeft = *(pUp1 - 2) && *(pThis - 2) && *(pDown1 - 2);
                bool allBelow = *(pDown2 - 2) && *(pDown2 - 1) && *(pDown2) && *(pDown2 + 1) && *(pDown2 + 2);
                bool allRight = *(pUp1 + 2) && *(pThis + 2) && *(pDown1 + 2);
                bool surroundings = allAbove && allLeft && allBelow && allRight;
                if (surroundings == true) {
                    // Fill the whole 5x5 block as white. Since we know the 5x5 borders
                    // are already white, just need to fill the 3x3 inner region.
                    *(pUp1 - 1) = 255;
                    *(pUp1 + 0) = 255;
                    *(pUp1 + 1) = 255;
                    *(pThis - 1) = 255;
                    *(pThis + 0) = 255;
                    *(pThis + 1) = 255;
                    *(pDown1 - 1) = 255;
                    *(pDown1 + 0) = 255;
                    *(pDown1 + 1) = 255;
                }
                // Since we just covered the whole 5x5 block with white, we know the next 2 pixels
                // won't be black, so skip the next 2 pixels on the right.
                pThis += 2;
                pUp1 += 2;
                pUp2 += 2;
                pDown1 += 2;
                pDown2 += 2;
            }
            // Move to the next pixel.
            pThis++;
            pUp1++;
            pUp2++;
            pDown1++;
            pDown2++;
        }
    }
}


// Draw an anti-aliased face outline, so the user knows where to put their face.
// Note that the skin detector for "alien" mode uses points around the face based on the face
// dimensions shown by this function.
void Cartoonifier::drawFaceStickFigure(Mat dst)
{
    Size size = dst.size();
    int sw = size.width;
    int sh = size.height;

    // Draw the face onto a color image with black background.
    Mat faceOutline = Mat::zeros(size, CV_8UC3);
    Scalar color = CV_RGB(255,255,0);   // Yellow
    int thickness = 4;
    // Use 70% of the screen height as the face height.
    int faceH = sh/2 * 70/100;  // "faceH" is actually half the face height (ie: radius of the ellipse).
    // Scale the width to be the same nice shape for any screen width (based on screen height).
    int faceW = faceH * 72/100; // Use a face with an aspect ratio of 0.72
    // Draw the face outline.
    ellipse(faceOutline, Point(sw/2, sh/2), Size(faceW, faceH), 0, 0, 360, color, thickness, CV_AA);
    // Draw the eye outlines, as 2 half ellipses.
    int eyeW = faceW * 23/100;
    int eyeH = faceH * 11/100;
    int eyeX = faceW * 48/100;
    int eyeY = faceH * 13/100;
    // Set the angle and shift for the eye half ellipses.
    int eyeA = 15; // angle in degrees.
    int eyeYshift = 11;
    // Draw the top of the right eye.
    ellipse(faceOutline, Point(sw/2 - eyeX, sh/2 - eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
    // Draw the bottom of the right eye.
    ellipse(faceOutline, Point(sw/2 - eyeX, sh/2 - eyeY - eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);
    // Draw the top of the left eye.
    ellipse(faceOutline, Point(sw/2 + eyeX, sh/2 - eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
    // Draw the bottom of the left eye.
    ellipse(faceOutline, Point(sw/2 + eyeX, sh/2 - eyeY - eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);

    // Draw the bottom lip of the mouth.
    int mouthY = faceH * 53/100;
    int mouthW = faceW * 45/100;
    int mouthH = faceH * 6/100;
    ellipse(faceOutline, Point(sw/2, sh/2 + mouthY), Size(mouthW, mouthH), 0, 0, 180, color, thickness, CV_AA);

    // Draw anti-aliased text.
    int fontFace = FONT_HERSHEY_COMPLEX;
    float fontScale = 1.0f;
    int fontThickness = 2;
    putText(faceOutline, "Put your face here", Point(sw * 23/100, sh * 10/100), fontFace, fontScale, color, fontThickness, CV_AA);
    //imshow("faceOutline", faceOutline);

    // Overlay the outline with alpha blending.
    addWeighted(dst, 1.0, faceOutline, 0.7, 0, dst, CV_8UC3);
}

const Mat Cartoonifier::getLastResult() const
{
    return result;
}

void Cartoonifier::resetResult()
{
    result = image.clone();
}

Cartoonifier::~Cartoonifier()
{

}

void Cartoonifier::setInputImage(Mat img)
{
    image = img.clone();
}

void Cartoonifier::paintingProcess()
{
    Mat srcColor = image.clone();
    Size size = image.size();

    Mat mask = getSketch();
    // Do the bilateral filtering at a shrunken scale, since it
    // runs so slowly but doesn't need full resolution for a good effect.
    Size smallSize;
    smallSize.width = size.width/2;
    smallSize.height = size.height/2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    resize(srcColor, smallImg, smallSize, 0,0, INTER_LINEAR);

    // Perform many iterations of weak bilateral filtering, to enhance the edges
    // while blurring the flat regions, like a cartoon.
    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;        // Repetitions for strong cartoon effect.
    for (int i=0; i<repetitions; i++) {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 15;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }

    // Go back to the original scale.
    resize(smallImg, srcColor, size, 0,0, INTER_LINEAR);

    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)result.data, 0, result.step * result.rows);

    // Use the blurry cartoon image, except for the strong edges that we will leave black.
    srcColor.copyTo(result, mask);
}

void Cartoonifier::alienProcess()
{
    Mat srcColor = image.clone();
    Size size = image.size();
    Mat srcGray;
    cvtColor(srcColor, srcGray, COLOR_BGR2GRAY);
    Mat mask = getSketch();
    // Do the bilateral filtering at a shrunken scale, since it
    // runs so slowly but doesn't need full resolution for a good effect.
    Size smallSize;
    smallSize.width = size.width/2;
    smallSize.height = size.height/2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    resize(srcColor, smallImg, smallSize, 0,0, INTER_LINEAR);

    // Perform many iterations of weak bilateral filtering, to enhance the edges
    // while blurring the flat regions, like a cartoon.
    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;        // Repetitions for strong cartoon effect.
    for (int i=0; i<repetitions; i++) {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 15;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }

    bool tryflip = false;
    CascadeClassifier cascade, nestedCascade;
    double scale = 1;
    Point *p = new Point[3];
    string cascadeName = "haarcascade_frontalface_alt.xml";
    string nestedCascadeName = "haarcascade_eye.xml";
    if (!nestedCascade.load(nestedCascadeName))
        cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
    if (!cascade.load(cascadeName))
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        //return -1;
    }

    detectAndDraw(smallImg, tmp, cascade, nestedCascade, scale, tryflip, p);
    Mat edges = Mat(size, CV_8U);
    // Generate a nice edge mask, similar to a pencil line drawing.
    Laplacian(srcGray, edges, CV_8U, 5);
    changeFacialSkinColor(smallImg, edges, p);

    // Go back to the original scale.
    resize(smallImg, srcColor, size, 0,0, INTER_LINEAR);

    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)result.data, 0, result.step * result.rows);

    // Use the blurry cartoon image, except for the strong edges that we will leave black.
    srcColor.copyTo(result, mask);
}

void Cartoonifier::sketchProcess()
{
    // For sketch mode, we just need the mask!
    Mat mask = getSketch();
    // The output image has 3 channels, not a single channel.
    cvtColor(mask, result, CV_GRAY2BGR);
    // Merge pencil background and result;
    Mat pencil_bg = imread("img/pencil_bg.jpg");
    double alpha = 0.6;
    double beta = 1 - alpha;
    resize(pencil_bg, pencil_bg, result.size());
    addWeighted(result, alpha, pencil_bg, beta, 0.0, result);
}

Mat Cartoonifier::getSketch()
{
    // Convert from BGR color to Grayscale
    Mat srcGray;
    Mat srcColor = image.clone();
    cvtColor(srcColor, srcGray, CV_BGR2GRAY);

    // Remove the pixel noise with a good Median filter, before we start detecting edges.
    medianBlur(srcGray, srcGray, 7);

    Size size = image.size();
    Mat mask = Mat(size, CV_8U);
    Mat edges = Mat(size, CV_8U);

    // Generate a nice edge mask, similar to a pencil line drawing.
    Laplacian(srcGray, edges, CV_8U, 5);
    threshold(edges, mask, 80, 255, THRESH_BINARY_INV);
    // Mobile cameras usually have lots of noise, so remove small
    // dots of black noise from the black & white edge mask.
    removePepperNoise(mask);
    return mask;
}

void Cartoonifier::evilProcess()
{
    Mat srcColor = image.clone();
    Size size = image.size();
    Mat mask = getEvil();

    // Do the bilateral filtering at a shrunken scale, since it
    // runs so slowly but doesn't need full resolution for a good effect.
    Size smallSize;
    smallSize.width = size.width/2;
    smallSize.height = size.height/2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    resize(srcColor, smallImg, smallSize, 0,0, INTER_LINEAR);

    // Perform many iterations of weak bilateral filtering, to enhance the edges
    // while blurring the flat regions, like a cartoon.
    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;        // Repetitions for strong cartoon effect.
    for (int i=0; i<repetitions; i++) {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 15;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }

    // Go back to the original scale.
    resize(smallImg, srcColor, size, 0,0, INTER_LINEAR);

    // Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    memset((char*)result.data, 0, result.step * result.rows);

    // Use the blurry cartoon image, except for the strong edges that we will leave black.
    srcColor.copyTo(result, mask);
}

Mat Cartoonifier::getEvil()
{
    // Convert from BGR color to Grayscale
    Mat srcGray;
    Mat srcColor = image.clone();
    cvtColor(srcColor, srcGray, CV_BGR2GRAY);

    // Remove the pixel noise with a good Median filter, before we start detecting edges.
    medianBlur(srcGray, srcGray, 7);

    Size size = image.size();
    Mat mask = Mat(size, CV_8U);
    Mat edges = Mat(size, CV_8U);
    // Evil mode, making everything look like a scary bad guy.
    // (Where "srcGray" is the original grayscale image plus a medianBlur of size 7x7).
    Mat edges2;
    Scharr(srcGray, edges, CV_8U, 1, 0);
    Scharr(srcGray, edges2, CV_8U, 1, 0, -1);
    edges += edges2;
    threshold(edges, mask, 12, 255, THRESH_BINARY_INV);
    medianBlur(mask, mask, 3);
    return mask;
}

void Cartoonifier::detectAndDraw(Mat &img, Mat &rst, CascadeClassifier &cascade, CascadeClassifier &nestedCascade, double scale, bool tryflip, Point *p)
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;

    cvtColor(img, gray, COLOR_BGR2GRAY);
    double fx = 1 / scale;
    resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    t = (double)cvGetTickCount();
    cascade.detectMultiScale(smallImg, faces,
        1.1, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        | CASCADE_SCALE_IMAGE,
        Size(30, 30));
    if (tryflip)
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale(smallImg, faces2,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            | CASCADE_SCALE_IMAGE,
            Size(30, 30));
        for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)cvGetTickCount() - t;
    printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i % 8];
        int radius;

        double aspect_ratio = (double)r.width / r.height;
        if (0.75 < aspect_ratio && aspect_ratio < 1.3)
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            //circle(img, center, radius, color, 3, 8, 0);
            //cout << "face:" << center.x << "," << center.y << endl;
            p[0].x = center.x; p[0].y = center.y;
        }
        else
            rectangle(img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
                color, 3, 8, 0);
        if (nestedCascade.empty())
            continue;
        smallImgROI = smallImg(r);
        nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            | CASCADE_SCALE_IMAGE,
            Size(30, 30));
        for (size_t j = 0; j < nestedObjects.size(); j++)
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            //circle(img, center, radius, color, 3, 8, 0);
            //cout << "eye" << j << ":" << center.x << "," << center.y << endl;
            p[j + 1].x = center.x; p[j + 1].y = center.y;
        }
    }
    rst = img;
}

