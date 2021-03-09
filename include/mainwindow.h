#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include "clickablelabel.h"
#include "rangeimage.h"

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
// #include <QVTKWidget.h>

#define MAX_WIDTH 2100.0
#define MAX_HEIGHT 800.0
#define INITIAL_NB_SPX 500
#define INITIAL_WEIGHT 20
#define MAX_LEVEL 500
#define MAX_WEIGHT 20

typedef pcl::PointXYZRGBA KittiPoint;
// typedef pcl::PointXYZI KittiPoint;
typedef pcl::PointCloud<KittiPoint> KittiPointCloud;
typedef pcl::visualization::PointCloudColorHandlerCustom<KittiPoint> KittiPointCloudColorHandlerCustom;

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

    void openPointCloud(string fileName);
    void openPointCloud(string fileName, string labelFileName);

    void changeColor(int i = 0);

    /**
     * Update the range image according to type
     * @param type must belongs 0 to 6
     * */
    void updateDisplay(int type);

public:
    explicit MainWindow(QWidget *parent = 0);
    KittiPointCloud::Ptr getPointCloud(string fileName);
    vector<uint32_t> getLabels(string fileName);
    ~MainWindow();

private:
    Ui::MainWindow *_ui;
    // ClickableLabel *_cl;
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

    // Pointcloud
    pcl::visualization::PCLVisualizer::Ptr _pclVisualizer;
    KittiPointCloud::Ptr _pointCloud;
    std::vector<uint32_t> _labels;
};

#endif // MAINWINDOW_H
