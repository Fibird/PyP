#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QKeyEvent>
#include <QColorDialog>
#include <QKeySequence>
#include <QIcon>
#include <QDir>
#include <QStringList>
#include <QString>
#include <QLocale>
#include "commands/commands.h"
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loadedImage(),
    savedImage()
{
    ui->setupUi(this);
    fileInfo = QFileInfo();
    undoStack = new QUndoStack(this);
    createActions();
    createMenus();
    // install translators
    qApp->installTranslator(&appTranslator);
    qApp->installTranslator(&qtTranslator);
    createLanguageMenu();
    ui->retranslateUi(this);
    retranslateUi();
}

void MainWindow::createActions()
{
    // undo and redo actions
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setToolTip(tr("undo"));
    undoAction->setStatusTip(tr("undo"));
    undoAction->setIcon(QIcon(tr(":/menu_icon/undo-icon")));

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setToolTip(tr("redo"));
    redoAction->setStatusTip(tr("redo"));
    redoAction->setIcon(QIcon(tr(":/menu_icon/redo-icon")));
}

void MainWindow::createMenus()
{
    ui->mainToolBar->addAction(undoAction);
    ui->menuFile->addAction(undoAction);
    ui->mainToolBar->addAction(redoAction);
    ui->menuFile->addAction(redoAction);
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
        loadedImage = cv::imread(fileFullPath.toLocal8Bit().toStdString());
        displayMat(loadedImage);
        // clear undoStack every loading a new image
        undoStack->clear();
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
    //savedImage = cartoonifier.getLastResult();
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
    QMessageBox::about(this, tr("About this application"), tr("Copyright(c)2017 Chaoyang Liu.\nThis software is under GPLv3.0 license."));
}

void MainWindow::on_action_Painting_triggered()
{
    if (!loadedImage.empty())
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

void MainWindow::updateImage(cv::Mat img)
{
    savedImage = img;
    displayMat(img);
}

void MainWindow::on_action_Sketch_triggered()
{
    if (!loadedImage.empty())
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
    if (!loadedImage.empty())
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
    this->close();
}

void MainWindow::createLanguageMenu()
{
    languageActionGroup = new QActionGroup(ui->menulanguage);
    languageActionGroup->setExclusive(true);
    connect(languageActionGroup, &QActionGroup::triggered, this, &MainWindow::switchLanguage);
    langPath = QApplication::applicationDirPath();
    langPath.append("/languages");
    QDir qmDir(langPath);
    QStringList fileNames = qmDir.entryList(QStringList("pyp_*.qm"));

    // format systems languages
    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    for (int i = 0; i < fileNames.size(); ++i)
    {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i];

        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);

        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        //To check the translation of the string "English" in the "MainWindow" context.
        QString lang = translator.translate("MainWindow", "English");
        QAction *action = new QAction(tr("&%1 %2").arg(i + 1).arg(lang), this);
        action->setCheckable(true);
        action->setData(locale);

        ui->menulanguage->addAction(action);
        languageActionGroup->addAction(action);

        // set default translations and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
            switchLanguage(action);
        }
    }
}

void MainWindow::switchLanguage(QAction *action)
{
    QString locale = action->data().toString();
    QString qmDir(langPath);
    appTranslator.load("pyp_" + locale, qmDir);
    qtTranslator.load("qt_" + locale, qmDir);
    ui->retranslateUi(this);
    retranslateUi();
}

void MainWindow::retranslateUi()
{
    undoAction->setText(tr("&Undo"));
    undoAction->setToolTip(tr("undo"));
    undoAction->setStatusTip(tr("undo"));
    redoAction->setText(tr("&Redo"));
    redoAction->setToolTip(tr("redo"));
    redoAction->setStatusTip(tr("redo"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!undoStack->isClean())
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Save"), tr("Save the changes?"),
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (btn == QMessageBox::Yes)
        {
            on_action_Save_triggered();
            event->accept();
        }
        else if (btn == QMessageBox::No)
        {
            event->accept();
        }
        else
            event->ignore();
    }
    else
        event->accept();
}
