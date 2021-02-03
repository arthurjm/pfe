#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QtDebug>
#include <QPainter>

#include <opencv2/opencv.hpp>

#include "slic.h"

using namespace cv;

class MainWindow;

#define ZOOM_MAX 8

class ClickableLabel : public QLabel{
    Q_OBJECT

public:
    ClickableLabel(QWidget* parent = nullptr);
    virtual ~ClickableLabel();
    QImage cvToQt(Mat &pSrc);
    Mat qtToCv(const QImage &pSrc);
    void setImgRef(Mat pImg);
    void initSuperpixels(int pNbSpx, int pWeight);
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

signals:
    void mousePos(int pX, int pY);
    void pixelValue(QPoint pPos, QColor pCol, int pLabelSpx);
    void handleGenerate(bool pEnable);
    void updateSlider(int treeLevel);

private:
    void mousePressEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);
    void wheelEvent(QWheelEvent *ev);
    void zoomIn(int pX, int pY);
    void zoomOut(int pX, int pY);
    Mat applyZoom(Mat pImg);
    int getGlobalCoord(int pCoord, int pRoi);
    void drawPointTo(const QPoint &pPoint, QColor pColor);
    void drawLineTo(const QPoint &endPoint, QColor pColor);


    Slic *_slic;
    Mat _imgRef;
    Mat _leftImgContours;
    Mat _leftImgClusters;
    Mat _rightImgContours;
    Mat _rightImgClusters;
    Mat _zoomLeftImg;
    Mat _coloredImg;
    Mat _rightImgNoContours;


    QPoint lastPoint;

    int _xRoi = 0;
    int _yRoi = 0;
    int _zoom = 1;

    int currentTreeLevel;

    QPainter painter;
    bool _isScribble;
    bool _showContours;
    int labelisationMode;
    int penWidth = 15;
};

#endif // CLICKABLELABEL_H
