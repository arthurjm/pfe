#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QtDebug>
#include <QPainter>

#include <opencv2/opencv.hpp>

#include "superpixelhierarchy.h"
#include "slic.h"
#include "rangeimage.h"
using namespace cv;

class MainWindow;

#define ZOOM_MAX 8



class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent = nullptr);
    virtual ~ClickableLabel();
    QImage cvToQt(Mat &pSrc);
    Mat qtToCv(const QImage &pSrc);
    void setImgRef(Mat pImg);
    void initSuperpixels(int pNbSpx, int pWeight);
    // void updateSuperpixels(int pNbSpx, int pWeight, bool buildScribbleLevels);
    void updateSuperpixels(int pNbSpx);
    void generateSuperpixelsZoom(int pNbSpx, int pWeight);
    void saveSelection();

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

    void setCurrentLabel(int label);

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
    Mat applyZoom(Mat pImg);
    int getGlobalCoord(int pCoord, int pRoi);
    void drawPointTo(const QPoint &pPoint, QColor pColor);
    void drawLineTo(const QPoint &endPoint, QColor pColor);

    // SuperpixelHierarchy *_sh;
    Slic *_slic;

    int _maxLevel;

    Mat _imgRef, _imgContours;
    Mat _leftImgContours;
    Mat _leftImgClusters;
    Mat _rightImgContours;
    Mat _rightImgClusters;
    Mat _zoomLeftImg;
    Mat _coloredImg;
    Mat _rightImgNoContours;

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
    int penWidth = 10;

    // Indicate which label will be associated to the next selected superpixels
    int _currentLabel = CL_LABEL_GROUND; // Default : Ground
};

#endif // CLICKABLELABEL_H
