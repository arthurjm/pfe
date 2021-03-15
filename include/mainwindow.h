#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include "clickablelabel.h"
#include "rangeimage.h"
#include "pointcloud.h"

// QT
#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QFileDialog>
#include <QMouseEvent>

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

// PCL
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

// VTK
#include <vtkRenderWindow.h>

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
    void openFile();

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

    void updateColor(Color colorMode = Color::White);

    /**
     * Update the range image according to type
     * @param type must belongs 0 to 6
     * */
    void updateDisplay(int type);
    /**
     * Update the metrics and recompute semention superpixels
     * @param index indicate the index of metrics table to be update
     * */
    void updateMetrics(int index);

public:
    explicit MainWindow(QWidget *parent = 0);
    void getPointCloud(std::string fileName);
    ~MainWindow();

private:
    Ui::MainWindow *_ui;
    QPalette _palColorPixel;
    QPalette _palCursor;
    QBrush _brushColorPixel;
    QBrush _brushCursor;
    cv::Mat _img;
    bool _isScribble = false;
    bool _showContours = true;

    bool _isGray = false;
    bool _interpolate = false;
    bool _closing = false;
    int _currentDisplayType = RI_Y;
    bool _equalHist = false;

    int _minSpx;
    PointCloud *_pc;
    // Pointcloud
    pcl::visualization::PCLVisualizer::Ptr _pclVisualizer;
};

#endif // MAINWINDOW_H
