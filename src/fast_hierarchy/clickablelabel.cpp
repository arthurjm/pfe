#include "clickablelabel.h"
#include "mainwindow.h"

#if (CV_VERSION_MAJOR >= 4)
    #define INTER_AREA INTER_AREA
#else
    #define INTER_AREA CV_INTER_AREA
#endif

ClickableLabel::ClickableLabel(QWidget* parent)
: QLabel(parent){
    setMouseTracking(true);
    _isScribble = false;
    _showContours = true;
    labelisationMode = 2;
    _sh = new SuperpixelHierarchy();
}

/*
 * Destructor. Clear any present data.
 */
ClickableLabel::~ClickableLabel(){
    delete _sh;
}

/*
 * Convert an openCV matrix to a QImage.
 *
 * Input : Image (cv::Mat), output format (QImage::Format)
 * Output: Image (Qiamge)
 */
QImage ClickableLabel::cvToQt(cv::Mat &src){
    return QImage(src.data,src.cols,src.rows,src.step, QImage::Format_RGB888).rgbSwapped();
}

cv::Mat ClickableLabel::qtToCv(const QImage &src){
    QImage swapped = src.rgbSwapped();
    return cv::Mat( swapped.height(), swapped.width(),
                                CV_8UC3,
                                const_cast<uchar*>(swapped.bits()),
                                static_cast<size_t>(swapped.bytesPerLine())).clone();
}

void ClickableLabel::setImgRef(Mat pImg){
    _imgRef = pImg;
    _coloredImg = pImg;
    //_imgContours = imread("../../../data/images/bigcat_edge.png");
    _imgContours = pImg; // changed in the futur 
}

void ClickableLabel::setRangeImage(RangeImage& ri){
    _rangeImage = ri;
}

/*
 * Clear the selection and reset the zoom.
 *
 * Input : -
 * Output: -
 */
void ClickableLabel::clear(){
    _zoomValueLeft = 1;
    _zoomValueRight = 1;

    _xRoi = 0;
    _yRoi = 0;
    if(!_imgRef.empty()) deleteSelection();
}

void ClickableLabel::clearScribble(){
    _sh->clearScribbleClusters();
    _coloredImg = _imgRef.clone();
    updateDisplay();
}

/*
 * Input : Number of superpixels (int), weight (int)
 * Output: -
 */
 void ClickableLabel::updateSuperpixels(int pNbSpx, int pWeight, bool buildScribbleLevels){
   if(_imgRef.empty()) return;
   _leftImgContours = _sh->buildHierarchy(_imgRef.clone(), _imgContours.clone(), pNbSpx, pWeight, buildScribbleLevels, _rangeImage.getData());
   _leftImgClusters = _leftImgContours.clone();
   _zoomLeftImg = applyZoom(_leftImgContours);
   _rightImgContours =  Mat(_imgRef.rows, _imgRef.cols, CV_8UC3, cv::Scalar(255,255,255));
   _rightImgContours = _sh->displayAllSaliency(_rightImgContours);
   _rightImgClusters = _rightImgContours.clone();

   updateClusters();
   updateDisplay();
 }


/*
 *  Concatenation of the left image and the right image, conversion in QPixmap
 *  and setting the image of the ClickableLabel before display.
 *
 * Input : -
 * Output: -
 */
void ClickableLabel::updateDisplay(){
     Mat display, leftImg, rightImg;
     if(_isScribble) leftImg = applyZoom(_coloredImg.clone());
     else leftImg = _zoomLeftImg;
     if(_showContours) rightImg = applyZoom(_rightImgClusters);
     else rightImg = applyZoom(_rightImgNoContours);
     hconcat(leftImg, rightImg, display);
     QImage superpixelsMap = cvToQt(display);
     QPixmap superpixelsPixmap = QPixmap::fromImage(superpixelsMap);
     this->setPixmap(superpixelsPixmap.scaled(superpixelsMap.size(),Qt::KeepAspectRatio));
}

/*
 *  Update the selection by displaying the clusters of the
 *  selected superpixels.
 *
 * Input : -
 * Output: -
 */
void ClickableLabel::updateClusters(){
     // _leftImgClusters = _sh->displayContours(_leftImgContours);
     _leftImgClusters = _sh->displayGreySelection(_leftImgContours);

     _zoomLeftImg = applyZoom(_leftImgClusters.clone());

     _rightImgClusters = _sh->displaySelection(_rightImgContours, _leftImgContours);
     _rightImgNoContours = _sh->displaySelection( cv::Mat(_imgRef.rows, _imgRef.cols, CV_8UC3, cv::Scalar(255,255,255)), _imgRef);
}


/*
 *  Delete the selection of all spx and update the display.
 *
 * Input : -
 * Output: -
 */
void ClickableLabel::deleteSelection(){
    _sh->clearScribbleClusters();
    _sh->clearSelectedClusters();

    _leftImgClusters = _leftImgContours;
    _rightImgClusters = _rightImgContours;
    _coloredImg = _imgRef.clone();
    _rightImgNoContours = cv::Mat(_imgRef.rows, _imgRef.cols, CV_8UC3, cv::Scalar(255,255,255));
    if(_isScribble) _zoomLeftImg = applyZoom(_coloredImg.clone());
    else _zoomLeftImg = applyZoom(_leftImgContours.clone());
    updateDisplay();
}

/*
 *  Save the current selection in the /images folder.
 *
 * Input : -
 * Output: -
 */
void ClickableLabel::saveSelection(){
    cv::Mat imgToSave = _sh->displaySelection( cv::Mat(_imgRef.rows, _imgRef.cols, CV_8UC3, cv::Scalar(255,255,255)), _imgRef);
    cv::imwrite("../../images/save.png", imgToSave);
}

/*
 *  Get the global coordinate based on the current zoom, the image coordinate
 * and the top left corner coordinate.
 * Input : Coordinate x or y (int), top left corner x or y (int)
 * Output: -
 */
int ClickableLabel::getGlobalCoord(int pCoord, int pRoi){
    return pRoi + pCoord/_zoomValueLeft;
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    if(_imgRef.empty()) return;
    int x = event->x();
    int y = event->y();
    if (x<0 || x>=_imgRef.cols * 2 || y<0 || y>=_imgRef.rows){
        return;
    }
    if(x >= _imgRef.cols && x < _imgRef.cols * 2){
        x = x%_imgRef.cols;
    }
    else {
        x = getGlobalCoord(x, _xRoi);
        y = getGlobalCoord(y, _yRoi);
    }
    if (_isScribble && event->type() == QEvent::MouseButtonPress){
        lastPoint = QPoint(x,y);
        if(event->button() == Qt::LeftButton) drawPointTo(QPoint(x,y), Qt::blue);
        else if(event->button() == Qt::RightButton) drawPointTo(QPoint(x,y), Qt::red);
    }
}

/*
 * Input : Mouse event (QMouseEvent)
 * Output: -
 */
 void ClickableLabel::mouseReleaseEvent(QMouseEvent *event){
     if(_imgRef.empty()) return;
     int x = event->x();
     int y = event->y();
     if (x<0 || x>=_imgRef.cols * 2 || y<0 || y>=_imgRef.rows){
         return;
     }
     if(x >= _imgRef.cols && x < _imgRef.cols * 2){
         x = x%_imgRef.cols;
     }
     else {
         x = getGlobalCoord(x, _xRoi);
         y = getGlobalCoord(y, _yRoi);
     }
     if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::LeftButton){
         if(_isScribble){
             drawLineTo(QPoint(x,y), Qt::blue);
             _sh->addObjectCluster(cv::Point2i(x,y));
             _sh->binaryLabelisationConnected();
         }
         else{
             _sh->selectCluster(cv::Point2i(x,y));
         }
     }
     else if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton) {
         if(_isScribble){
             drawLineTo(QPoint(x,y), Qt::red);
             _sh->addBackgroundCluster(cv::Point2i(x,y));
             _sh->binaryLabelisationConnected();

         }
         else{
             _sh->deselectCluster(cv::Point2i(x,y));
         }
     }
     updateClusters();
     updateDisplay();
 }

/*
 * Input : Mouse event (QMouseEvent)
 * Output: -
 */
 void ClickableLabel::mouseMoveEvent(QMouseEvent *event){
     if(_imgRef.empty()) return;
     int x = event->x();
     int y = event->y();
     if (x<0 || x>=_imgRef.cols * 2 || y<0 || y>=_imgRef.rows){
         return;
     }
     if(x >= _imgRef.cols && x < _imgRef.cols * 2){
         x = x%_imgRef.cols;
     }
     else {
         emit mousePos(x, y); // if zoom right is disabled, must update x and y first
         x = getGlobalCoord(x, _xRoi);
         y = getGlobalCoord(y, _yRoi);
     }
     Vec3b bgrPixel = _imgRef.at<Vec3b>(y,x);
     QColor bgr(bgrPixel[2], bgrPixel[1], bgrPixel[0]);
     emit pixelValue(QPoint(x,y), bgr, _sh->labelOfPixel(cv::Point2i(x, y)));
     if(event->buttons() & Qt::LeftButton){
         if(_isScribble){
             drawLineTo(QPoint(x,y), Qt::blue);
             _sh->addObjectCluster(cv::Point2i(x,y));
             _sh->binaryLabelisationConnected();
         }
         else{
             _sh->selectCluster(Point2i(x,y));
         }
     }
     else if(event->buttons() & Qt::RightButton){
         if(_isScribble){
             drawLineTo(QPoint(x,y), Qt::red);
             _sh->addBackgroundCluster(cv::Point2i(x,y));
             _sh->binaryLabelisationConnected();

         }
         else{
             _sh->deselectCluster(Point2i(x,y));
         }
     }
     updateClusters();
     updateDisplay();
 }

/*
 * Zoom (wheel forward) or dezoom (wheel backward on the image, and update the
 * display.
 *
 * Input : Mouse event (QWheelEvent)
 * Output: -
 */
void ClickableLabel::wheelEvent(QWheelEvent *event){
    if (event->delta()>0 && event->x() < _imgRef.cols && _zoomValueLeft < ZOOM_MAX){
        if(!_isScribble){
            int newLevel = _sh->zoomInHierarchy(_maxLevel);
            updateSuperpixels(newLevel, _sh->getCurrentWeight(), false);
        }
        zoomIn(event->x(), event->y());
    }
    else if(event->delta()<0 && event->x() < _imgRef.cols && _zoomValueLeft!=1){
        if(!_isScribble){
            int newLevel = _sh->zoomOutHierarchy();
            updateSuperpixels(newLevel, _sh->getCurrentWeight(), false);
        }
        zoomOut(event->x(), event->y());
    }
    updateSlider(_sh->getCurrentLevel());
    updateDisplay();
}

/*
 *  Zoom*2 on the image based on the current mouse location. It computes the top
 * left corner, gets the ROI and resizes the new image to keep the size of the
 * main image. .
 *
 * Input : x coordinate (int), y coordinate (int)
 * Output: -
 */
 void ClickableLabel::zoomIn(int pX, int pY){
     pX = getGlobalCoord(pX, _xRoi);
     pY = getGlobalCoord(pY, _yRoi);

     if(pX + (_imgRef.cols) / (_zoomValueLeft*4) >= _imgRef.cols){
         pX = (_imgRef.cols-1) - (_imgRef.cols) / (_zoomValueLeft*4);
     }
     else if(pX - (_imgRef.cols) / (_zoomValueLeft*4) < 0){
         pX = (_imgRef.cols) / (_zoomValueLeft*4);
     }
     if(pY + (_imgRef.rows) / (_zoomValueLeft*4) >= _imgRef.rows){
         pY = (_imgRef.rows-1) - (_imgRef.rows) / (_zoomValueLeft*4);
     }
     else if(pY + (_imgRef.rows) / (_zoomValueLeft*4) < 0){
         pY = (_imgRef.rows) / (_zoomValueLeft*4);
     }

     _xRoi = max(0, pX - (_imgRef.cols) / (_zoomValueLeft*4));
     _yRoi = max(0, pY - (_imgRef.rows) / (_zoomValueLeft*4));

     Rect roi = Rect(_xRoi, _yRoi, _imgRef.cols/(_zoomValueLeft*2), _imgRef.rows/(_zoomValueLeft*2));

     Mat imgRoiClusters;
     if(_isScribble) imgRoiClusters = _coloredImg.clone()(roi);
     else imgRoiClusters = _leftImgClusters.clone()(roi);

     cv::resize(imgRoiClusters, _zoomLeftImg, Size(_imgRef.cols, _imgRef.rows), 0, 0, CV_INTER_AREA);
     _zoomValueLeft*=2;
 }

/*
 *  Zoom/2 on the image based on the current mouse location. It computes the top
 * left corner, gets the ROI and resizes the new image to keep the size of the
 * main image.
 *
 * Input : x coordinate (int), y coordinate (int)
 * Output: -
 */
 void ClickableLabel::zoomOut(int pX, int pY){
     pX = getGlobalCoord(pX, _xRoi);
     pY = getGlobalCoord(pY, _yRoi);

     if(pX + (_imgRef.cols) / _zoomValueLeft >= _imgRef.cols){
         pX = (_imgRef.cols-1) - (_imgRef.cols) / _zoomValueLeft;
     }
     if(pY + (_imgRef.rows) / _zoomValueLeft >= _imgRef.rows){
         pY = (_imgRef.rows-1) - (_imgRef.rows) / _zoomValueLeft;
     }
     if(pX - (_imgRef.cols) / _zoomValueLeft < 0){
         pX = (_imgRef.cols) / _zoomValueLeft;
     }
     if(pY + (_imgRef.rows) / _zoomValueLeft < 0){
         pY = (_imgRef.rows) / _zoomValueLeft;
     }

     _xRoi = max(0, pX - (_imgRef.cols) / _zoomValueLeft);
     _yRoi = max(0, pY - (_imgRef.rows) / _zoomValueLeft);

     Rect roi = Rect(_xRoi, _yRoi, _imgRef.cols/(_zoomValueLeft/2), _imgRef.rows/(_zoomValueLeft/2));
     Mat imgRoiClusters;
     if(_isScribble) imgRoiClusters = _coloredImg.clone()(roi);
     else imgRoiClusters = _leftImgClusters.clone()(roi);
     cv::resize(imgRoiClusters, _zoomLeftImg, Size(_imgRef.cols, _imgRef.rows), 0, 0, CV_INTER_AREA);
     _zoomValueLeft/=2;
 }

/*
 *  Apply the current level of zoom on the image.
 *
 * Input : Image (cv::Mat)
 * Output: -
 */
Mat ClickableLabel::applyZoom(Mat pImg){
    Rect roi = Rect(_xRoi, _yRoi, _imgRef.cols/_zoomValueLeft, _imgRef.rows/_zoomValueLeft);
    Mat zoomedImg = pImg(roi);
    cv::resize(zoomedImg, zoomedImg, Size(_imgRef.cols, _imgRef.rows), 0, 0, INTER_AREA);
    return zoomedImg;
}

void ClickableLabel::drawPointTo(const QPoint &pPoint, QColor pColor)
{
    QImage coloredImg = cvToQt(_coloredImg);
    painter.begin(&coloredImg);
    painter.setPen(QPen(pColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPoint(pPoint);
    painter.end();
    _coloredImg = qtToCv(coloredImg);
    updateDisplay();
}

void ClickableLabel::drawLineTo(const QPoint &pEndPoint, QColor pColor)
{
    QImage coloredImg = cvToQt(_coloredImg);
    painter.begin(&coloredImg);
    painter.setPen(QPen(pColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, pEndPoint);
    painter.end();
    _coloredImg = qtToCv(coloredImg);
    updateDisplay();
    lastPoint = pEndPoint;
}

void ClickableLabel::setScribble(bool isScribble){
    _isScribble = isScribble;
    updateDisplay();
}
void ClickableLabel::setContours(bool showContours){
    _showContours = showContours;
    updateDisplay();
}
