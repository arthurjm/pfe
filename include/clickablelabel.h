#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include "superpixelhierarchy.h"
#include "slic.h"
#include "rangeimage.h"

// QT
#include <QLabel>
#include <QMouseEvent>
#include <QtDebug>
#include <QPainter>

// OpenCV
#include <opencv2/opencv.hpp>

class MainWindow;

#define ZOOM_MAX 8

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent = nullptr);
    ~ClickableLabel();
    QImage cvToQt(cv::Mat &pSrc);
    cv::Mat qtToCv(const QImage &pSrc);
    void setImgRef(cv::Mat pImg);
    void initSuperpixels(int pNbSpx, int pWeight);
    // void updateSuperpixels(int pNbSpx, int pWeight, bool buildScribbleLevels);
    void updateSuperpixels(int pNbSpx);
    //void generateSuperpixelsZoom(int pNbSpx, int pWeight);
    void saveSelection(string filename);

    void updateDisplay();
    void updateClusters();
    void deleteSelection();
    void clear();
    int nbSpx();

    void setScribble(bool modeScribble);
    void clearScribble();

    void setContours(bool showContours);
    void setMaximumLevel(int pMaxLevel) { _maxLevel = pMaxLevel; }

    /**
    * set the default range image 
    * 
    * @param ri reference on an RangeImage
    * */
    void setRangeImage(RangeImage &ri);

    /**
     * Generate a display matrice according to type
     * @param type must belongs to 0 to 6
     * @param  interpolation boolean to activate interpolation
     * @param closing boolean to activate closing morphologie
     * */
    cv::Mat getDisplayMat(int type, bool isGray, bool interpolate, bool closing, bool equalHist);

    /**
     * In scribble mode, get object marker's color according to label;
     * */
    QColor getObjMarkerColor();
    void setCurrentLabel(int label);

    void setMetrics(int metric);

signals:
    void mousePos(int pX, int pY);
    void pixelValue(QPoint pPos, QColor pCol, int pLabelSpx);
    void handleGenerate(bool pEnable);
    void updateSlider(int treeLevel);

private:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
    void zoomIn(int pX, int pY);
    void zoomOut(int pX, int pY);
    cv::Mat applyZoom(cv::Mat pImg);
    int getGlobalCoord(int pCoord, int pRoi);
    void drawPointTo(const QPoint &pPoint, QColor pColor);
    void drawLineTo(const QPoint &endPoint, QColor pColor);

    // SuperpixelHierarchy *_sh;
    Slic *_slic;

    int _maxLevel;

    cv::Mat _imgRef, _imgContours;
    cv::Mat _leftImgContours;
    cv::Mat _leftImgClusters;
    cv::Mat _rightImgContours;
    cv::Mat _rightImgClusters;
    cv::Mat _zoomLeftImg;
    cv::Mat _coloredImg;
    cv::Mat _rightImgNoContours;

    RangeImage _rangeImage;

    QPoint lastPoint;

    int _xRoi = 0;
    int _yRoi = 0;
    int _zoomValueLeft = 1;
    int _zoomValueRight = 1;

    QPainter painter;
    bool _isScribble;
    bool _showContours;
    int labelisationMode;
    int penWidth = 5;

    // Indicate which label will be associated to the next selected superpixels
    int _currentLabel = SLIC_LABEL_GROUND; // Default : Ground
    bool _metrics[4];
};

#endif // CLICKABLELABEL_H
