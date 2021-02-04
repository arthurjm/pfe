#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QFileDialog>
#include <QMouseEvent>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include "clickablelabel.h"

using namespace cv;

#define MAX_WIDTH 1600.0
#define MAX_HEIGHT 800.0
#define INITIAL_NB_SPX 300.0
#define INITIAL_WEIGHT 20
#define MAX_LEVEL 1000
#define MAX_WEIGHT 20


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void openImage();
    void updateSuperpixelsLevel();
    void initSuperpixelsLevel();
    void updateSuperpixelsWeight();
    void updateSliderValues();
    void updateMaxSpxSlider();
    void updateMaxWeightSlider();

    void resetSelection();
    void save();
    void displayPixelValues(QPoint pPos, QColor pCol, int pLabelSpx);
    void displayCursor(int pX, int pY);
    void setNbSpxSlider(int treeLevel);
    void switchMode();
    void switchContours();



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *_ui;
    ClickableLabel *_cl;
    QPalette _palColorPixel;
    QPalette _palCursor;
    QBrush _brushColorPixel;
    QBrush _brushCursor;
    Mat _img;
    bool _isScribble = false;
    bool _showContours = true;

};

#endif // MAINWINDOW_H