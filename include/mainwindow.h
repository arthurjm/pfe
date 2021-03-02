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

#include "rangeimage.h"

using namespace cv;

#define MAX_WIDTH 2100.0
#define MAX_HEIGHT 800.0
#define INITIAL_NB_SPX 500
#define INITIAL_WEIGHT 20
#define MAX_LEVEL 500
#define MAX_WEIGHT 20

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void openImage();
    void openRangeImage();
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

    /**
     * Update the range image according to type
     * @param type must belongs 0 to 6
     * */
    void updateDisplay(int type);

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

    bool _isGray = false;
    bool _interpolate = false;
    bool _closing = false;
    int _currentDisplayType = RI_Y;
    bool _equalHist = false;
};

#endif // MAINWINDOW_H
