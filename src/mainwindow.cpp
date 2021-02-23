#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QColorDialog>
#include <QDebug>
using namespace cv;

const QString displayNbOnConstSpace(int num)
{
    if (num < 10)
    {
        return "   " + QString::number(num);
    }
    else if (num < 100)
    {
        return "  " + QString::number(num);
    }
    else
    {
        return QString::number(num);
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    _cl = new ClickableLabel(nullptr);

    _ui->layoutImages->addWidget(_cl, 0, 0);

    _ui->valueWeightSlider->setNum(INITIAL_WEIGHT);
    _ui->weightSlider->setValue(INITIAL_WEIGHT);

    connect(_ui->actionOpen_file, SIGNAL(triggered()), this, SLOT(openRangeImage()));
    connect(_ui->nbSpxSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsLevel()));
    connect(_ui->nbSpxSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->weightSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsWeight()));
    connect(_ui->weightSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->resetSelectionButton, SIGNAL(released()), this, SLOT(resetSelection()));
    connect(_ui->saveSelectionButton, SIGNAL(released()), this, SLOT(save()));
    connect(_ui->selectionButton, SIGNAL(released()), this, SLOT(switchMode()));
    connect(_ui->contoursButton, SIGNAL(released()), this, SLOT(switchContours()));

    connect(_ui->spinBoxMaxSpx, SIGNAL(valueChanged(int)), this, SLOT(updateMaxSpxSlider()));
    connect(_ui->spinBoxMaxWeight, SIGNAL(valueChanged(int)), this, SLOT(updateMaxWeightSlider()));

    connect(_cl, SIGNAL(pixelValue(QPoint, QColor, int)), this, SLOT(displayPixelValues(QPoint, QColor, int)));
    connect(_cl, SIGNAL(mousePos(int, int)), this, SLOT(displayCursor(int, int)));
    connect(_cl, SIGNAL(updateSlider(int)), this, SLOT(setNbSpxSlider(int)));

    //_img = imread("../../data/images/banana1.bmp");
    //if(_img.cols==0) _img = imread("../../data/range_images/000045.bin");

    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../data/range_image"), "Binary file (*.bin)");
    //string fileName = "../../../data/range_image/000045.bin";

    RangeImage ri(fileName.toStdString());
    //_img = ri.createImageFromXYZ();
    _img = ri.createBGRFromColorMap(1, true);

    float scale = MAX_WIDTH / (_img.cols);
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);
    scale = MAX_HEIGHT / (2 * _img.rows);
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);

    _cl->setImgRef(_img);
    _cl->setRangeImage(ri);
    _cl->setMaximumLevel(MAX_LEVEL);
    _ui->nbSpxSlider->setMaximum(MAX_LEVEL);
    _ui->spinBoxMaxSpx->setValue(MAX_LEVEL);
    _ui->weightSlider->setMaximum(MAX_WEIGHT);
    _ui->spinBoxMaxWeight->setValue(MAX_WEIGHT);

    _ui->spinBoxMaxWeight->setVisible(false); //remove to enable changing max values of weight

    initSuperpixelsLevel();

    int w_width = min(_img.cols, (int)MAX_WIDTH);
    int w_height = min(2 * _img.rows, (int)MAX_HEIGHT);

    // width : 20 = left padding (10) + layout spacing(10)
    // height : 20 = image height (64) + selection button (25) + slider (70) + 100?
    this->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 170);

    _ui->horizontalWidget->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 120);
    _ui->widgetSliders->resize(w_width, 70);
    _ui->widgetImages->resize(w_width, w_height + _ui->selectionButton->height());
    _ui->selectionButton->setMaximumWidth((w_width - 10) / 2.0);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
    _ui->statusBar->addWidget(_ui->pixelSpxLabel);

    _brushCursor.setColor(QColor(0, 0, 0));
    _brushCursor.setStyle(Qt::SolidPattern);
    _palCursor.setBrush(QPalette::Active, QPalette::Window, _brushCursor);
    _ui->widgetCursor->setPalette(_palCursor);
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _cl;
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open an image", QString(), "Images (*.bmp *.jpeg *.jpg *.png *.tif *.gif)");
    if (fileName == nullptr)
        return;

    _img = imread(fileName.toStdString());

    float scale = min(MAX_WIDTH / (2 * _img.cols), MAX_HEIGHT / _img.rows);
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);

    _cl->clear();
    _cl->setImgRef(_img);
    initSuperpixelsLevel();

    int w_width = min(2 * _img.cols, (int)MAX_WIDTH);
    int w_height = min(_img.rows, (int)MAX_HEIGHT);

    this->resize(w_width + 50, w_height + 2.0 * _ui->widgetSliders->height() + _ui->selectionButton->height());

    _ui->widgetSliders->resize(w_width, _ui->widgetSliders->height());
    _ui->widgetImages->resize(w_width, w_height + _ui->selectionButton->height());
    _ui->selectionButton->setMaximumWidth(w_width / 2.0);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}

void MainWindow::openRangeImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../../../data/range_image"), "Binary file (*.bin)");
    if (fileName == nullptr)
        return;

    RangeImage ri(fileName.toStdString());
    // _img = ri.createImageFromXYZ();
    _img = ri.createBGRFromColorMap(1, true);

    float scale = min(MAX_WIDTH / (_img.cols), MAX_HEIGHT / (2 * _img.rows));
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);

    _cl->clear();
    _cl->setImgRef(_img);
    _cl->setRangeImage(ri);
    initSuperpixelsLevel();

    int w_width = min(_img.cols, (int)MAX_WIDTH);
    int w_height = min(2 * _img.rows, (int)MAX_HEIGHT);

    // width : 20 = left padding (10) + layout spacing(10)
    // height : 20 = image height (64) + selection button (25) + slider (70) + 100?
    this->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 170);

    _ui->horizontalWidget->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 120);
    _ui->widgetSliders->resize(w_width, 70);
    _ui->widgetImages->resize(w_width, w_height + _ui->selectionButton->height());
    _ui->selectionButton->setMaximumWidth((w_width - 10) / 2.0);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}

void MainWindow::initSuperpixelsLevel()
{
    _cl->updateSuperpixels(_ui->nbSpxSlider->value(), _ui->weightSlider->value(), true);
}

void MainWindow::updateSuperpixelsLevel()
{
    _cl->updateSuperpixels(_ui->nbSpxSlider->value(), _ui->weightSlider->value(), false);
}

void MainWindow::updateSuperpixelsWeight()
{
    initSuperpixelsLevel();
    _cl->clear();
}

void MainWindow::updateSliderValues()
{
    _ui->valueNbSpxSlider->setNum(_ui->nbSpxSlider->value());
    // if (_ui->weightSlider->value() == _ui->weightSlider->maximum()){
    //     _ui->valueWeightSlider->setText(trUtf8("\u221e")); // Symbol infinite
    // } else {
    _ui->valueWeightSlider->setNum(_ui->weightSlider->value());
    // }
}

void MainWindow::updateMaxSpxSlider()
{
    _cl->setMaximumLevel(_ui->spinBoxMaxSpx->value());
    _ui->nbSpxSlider->setMaximum(_ui->spinBoxMaxSpx->value());
}

void MainWindow::updateMaxWeightSlider()
{
    _ui->weightSlider->setMaximum(_ui->spinBoxMaxWeight->value());
}

void MainWindow::setNbSpxSlider(int treeLevel)
{
    _ui->valueNbSpxSlider->setNum(treeLevel);
    _ui->nbSpxSlider->setValue(treeLevel);
}

void MainWindow::resetSelection()
{
    _cl->deleteSelection();
}

void MainWindow::save()
{
    _cl->saveSelection();
}

void MainWindow::displayPixelValues(QPoint pos, QColor col, int label_spx)
{
    QString x_value = displayNbOnConstSpace(pos.x());
    QString y_value = displayNbOnConstSpace(pos.y());
    QString r_value = displayNbOnConstSpace(col.red());
    QString g_value = displayNbOnConstSpace(col.green());
    QString b_value = displayNbOnConstSpace(col.blue());
    QString status = "(x:" + x_value + "    y:" + y_value + ")    ";
    status += "(r:" + r_value + "    g:" +
              g_value + "    b:" +
              b_value + ")    ";
    status += "color:";
    _ui->pixelValuesLabel->setText(status);
    _brushColorPixel.setColor(col);
    _brushColorPixel.setStyle(Qt::SolidPattern);
    _palColorPixel.setBrush(QPalette::Active, QPalette::Window, _brushColorPixel);
    _ui->pixelColorLabel->setPalette(_palColorPixel);
    _ui->pixelSpxLabel->setText("    spx:" + QString::number(label_spx));
}

void MainWindow::displayCursor(int pX, int pY)
{
    _ui->widgetCursor->move(pX + 4, pY + _img.rows + 104);
}

void MainWindow::switchMode()
{
    if (_isScribble)
    {
        _isScribble = false;
        _cl->setScribble(false);
        _ui->selectionButton->setText("Sélection");
        this->setCursor(Qt::ArrowCursor);
        _ui->nbSpxSlider->setVisible(true);
    }
    else
    {
        _isScribble = true;
        _cl->setScribble(true);
        _ui->selectionButton->setText("Scribble");
        this->setCursor(Qt::PointingHandCursor);

        //set nb spx on max level and disable it :
        // _ui->valueNbSpxSlider->setNum(_cl->nbSpx());
        // _ui->nbSpxSlider->setValue(_cl->nbSpx());
        // _cl->updateSuperpixels(_ui->nbSpxSlider->value());
        // _ui->nbSpxSlider->setVisible(false);
    }
}

void MainWindow::switchContours()
{
    if (_showContours)
    {
        _showContours = false;
        _cl->setContours(false);
        _ui->contoursButton->setText("Objet");
    }
    else
    {
        _showContours = true;
        _cl->setContours(true);
        _ui->contoursButton->setText("Objet + contours");
    }
}
