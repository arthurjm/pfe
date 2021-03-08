#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QColorDialog>
#include <QDebug>
using namespace cv;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          _ui(new Ui::MainWindow),
                                          _pclVisualizer(new pcl::visualization::PCLVisualizer("PCL Visualizer", false))
{
    _ui->setupUi(this);

    // _cl = new ClickableLabel(nullptr);

    // _ui->layoutImages->addWidget(_cl, 0, 0);

    // Pointcloud temporary
    // QVTKWidget *VTKwidget = new QVTKWidget();
    // _ui->pointCloudLayout->addWidget(VTKwidget);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
    _ui->statusBar->addWidget(_ui->pixelSpxLabel);

    _ui->vtkWidget->SetRenderWindow(_pclVisualizer->getRenderWindow());

    _ui->valueWeightSlider->setNum(INITIAL_WEIGHT);
    _ui->weightSlider->setValue(INITIAL_WEIGHT);

    _ui->valueNbSpxSlider->setNum(INITIAL_NB_SPX);
    _ui->nbSpxSlider->setValue(INITIAL_NB_SPX);

    connect(_ui->actionOpen_file, SIGNAL(triggered()), this, SLOT(openRangeImage()));
    connect(_ui->nbSpxSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsLevel()));
    connect(_ui->nbSpxSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->weightSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsWeight()));
    connect(_ui->weightSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->resetSelectionButton, SIGNAL(released()), this, SLOT(resetSelection()));
    connect(_ui->saveSelectionButton, SIGNAL(released()), this, SLOT(save()));
    connect(_ui->selectionButton, SIGNAL(released()), this, SLOT(switchMode()));
    connect(_ui->contoursButton, SIGNAL(released()), this, SLOT(switchContours()));

    connect(_ui->spinBoxMaxSpx, SIGNAL(editingFinished()), this, SLOT(updateMaxSpxSlider()));
    connect(_ui->spinBoxMaxWeight, SIGNAL(valueChanged(int)), this, SLOT(updateMaxWeightSlider()));

    connect(_ui->clWidget, SIGNAL(pixelValue(QPoint, QColor, int)), this, SLOT(displayPixelValues(QPoint, QColor, int)));
    connect(_ui->clWidget, SIGNAL(mousePos(int, int)), this, SLOT(displayCursor(int, int)));
    connect(_ui->clWidget, SIGNAL(updateSlider(int)), this, SLOT(setNbSpxSlider(int)));

    // Connect to range image display RadioButton
    connect(_ui->display_XYZ, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_XYZ); });
    connect(_ui->display_X, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_X); });
    connect(_ui->display_Y, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_Y); });
    connect(_ui->display_Z, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_Z); });
    connect(_ui->display_Depth, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_DEPTH); });
    connect(_ui->display_Remission, &QRadioButton::clicked, this, [this]() { updateDisplay(RI_REMISSION); });

    connect(_ui->display_Gray, &QCheckBox::clicked, this, [this]() { _isGray = !_isGray; updateDisplay(_currentDisplayType); });
    connect(_ui->display_Interpolation, &QCheckBox::clicked, this, [this]() { _interpolate = !_interpolate; updateDisplay(_currentDisplayType); });
    connect(_ui->display_Closing, &QCheckBox::clicked, this, [this]() { _closing = !_closing; updateDisplay(_currentDisplayType); });
    connect(_ui->display_Hist, &QCheckBox::clicked, this, [this]() { _equalHist = !_equalHist; updateDisplay(_currentDisplayType); });

    // Connect label RadioButton
    connect(_ui->label_Ground, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_GROUND); });
    connect(_ui->label_Structure, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_STUCTURE); });
    connect(_ui->label_Vehicle, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_VEHICLE); });
    connect(_ui->label_Nature, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_NATURE); });
    connect(_ui->label_Human, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_HUMAN); });
    connect(_ui->label_Object, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_OBJECT); });
    connect(_ui->label_Outlier, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(CL_LABEL_OUTLIER); });
    
    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../data/range_image"), "Binary file (*.bin)");
    RangeImage ri(fileName.toStdString());
    // RangeImage ri("../data/range_image/000045.bin");
    openPointCloud("../data/velodyne/000000.bin");

    _interpolate = true;
    _img = ri.createColorMat({RI_Y}, _isGray, _interpolate);

    _minSpx = (_img.rows * _img.cols) / (min(_img.rows, _img.cols) * min(_img.rows, _img.cols));

    _ui->display_Y->setChecked(true);
    _ui->display_Interpolation->setChecked(true);
    _ui->label_Ground->setChecked(true);

    float scale = MAX_WIDTH / (_img.cols);
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);
    scale = MAX_HEIGHT / (2 * _img.rows);
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);

    _ui->clWidget->setImgRef(_img);
    _ui->clWidget->setRangeImage(ri);
    _ui->clWidget->setMaximumLevel(MAX_LEVEL);
    _ui->nbSpxSlider->setMaximum(MAX_LEVEL);
    _ui->weightSlider->setMaximum(MAX_WEIGHT);
    _ui->spinBoxMaxSpx->setValue(MAX_LEVEL);
    _ui->spinBoxMaxWeight->setValue(MAX_WEIGHT);

    _ui->spinBoxMaxWeight->setVisible(false); //remove to enable changing max values of weight

    _ui->clWidget->initSuperpixels(INITIAL_NB_SPX, INITIAL_WEIGHT);
    updateSuperpixelsLevel();

    int w_width = min(_img.cols, (int)MAX_WIDTH);
    int w_height = min(2 * _img.rows, (int)MAX_HEIGHT);

    // width : 20 = left padding (10) + layout spacing(10)
    // height : 20 = image height (64) + selection button (25) + slider (70) + 100?
    this->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 170);

    // _ui->horizontalWidget->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 120 + 200);
    // _ui->widgetSliders->resize(w_width, 70);
    // _ui->widgetImages->resize(w_width, w_height + _ui->selectionButton->height());
    // _ui->selectionButton->setMaximumWidth((w_width - 10) / 2.0);

    // _ui->vtkWidget->resize(w_width, 200);

    _brushCursor.setColor(QColor(0, 0, 0));
    _brushCursor.setStyle(Qt::SolidPattern);
    _palCursor.setBrush(QPalette::Active, QPalette::Window, _brushCursor);
    _ui->widgetCursor->setPalette(_palCursor);
}

MainWindow::~MainWindow()
{
    delete _ui;
    // delete _cl;
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

    _ui->clWidget->clear();
    _ui->clWidget->setImgRef(_img);

    initSuperpixelsLevel();

    // int w_width = min(2 * _img.cols, (int)MAX_WIDTH);
    // int w_height = min(_img.rows, (int)MAX_HEIGHT);

    // // this->resize(w_width + 50, w_height + 2.0 * _ui->slidersWidget->height() + _ui->selectionButton->height());

    // _ui->slidersWidget->resize(w_width, _ui->slidersWidget->height());
    // _ui->slidersWidget->resize(w_width, w_height + _ui->selectionButton->height());
    // _ui->selectionButton->setMaximumWidth(w_width / 2.0);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}

void MainWindow::openRangeImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../data/range_image"), "Binary file (*.bin)");
    if (fileName == nullptr)
        return;

    RangeImage ri(fileName.toStdString());
    _img = ri.createColorMat({RI_Y}, true);
    _ui->display_Y->setChecked(true);
    _ui->display_Interpolation->setChecked(true);
    _interpolate = true;
    float scale = min(MAX_WIDTH / (_img.cols), MAX_HEIGHT / (2 * _img.rows));
    if (scale < 1.0)
        cv::resize(_img, _img, cv::Size(0, 0), scale, scale);

    _ui->clWidget->clear();
    _ui->clWidget->setImgRef(_img);
    _ui->clWidget->setRangeImage(ri);

    _ui->clWidget->initSuperpixels(INITIAL_NB_SPX, INITIAL_WEIGHT);
    updateSuperpixelsLevel();

    _ui->nbSpxSlider->setMaximum(MAX_LEVEL);
    _ui->spinBoxMaxSpx->setValue(MAX_LEVEL);

    _ui->valueWeightSlider->setNum(INITIAL_WEIGHT);
    _ui->weightSlider->setValue(INITIAL_WEIGHT);

    _ui->valueNbSpxSlider->setNum(INITIAL_NB_SPX);
    _ui->nbSpxSlider->setValue(INITIAL_NB_SPX);

    // int w_width = min(_img.cols, (int)MAX_WIDTH);
    // int w_height = min(2 * _img.rows, (int)MAX_HEIGHT);

    // // width : 20 = left padding (10) + layout spacing(10)
    // // height : 20 = image height (64) + selection button (25) + slider (70) + 100?
    // this->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 170);

    // _ui->horizontalWidget->resize(w_width + 30 + _ui->widgetSidebar_1->width() + _ui->widgetSidebar_2->width(), w_height + _ui->selectionButton->height() + 120);
    // _ui->widgetSliders->resize(w_width, 70);
    // _ui->widgetImages->resize(w_width, w_height + _ui->selectionButton->height());
    // _ui->selectionButton->setMaximumWidth((w_width - 10) / 2.0);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}

void MainWindow::initSuperpixelsLevel()
{
    _ui->clWidget->initSuperpixels(_ui->nbSpxSlider->maximum(), _ui->weightSlider->value());
    // _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value(), _ui->weightSlider->value(), true);
}

void MainWindow::updateSuperpixelsLevel()
{
    //_ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value(), _ui->weightSlider->value(), false);
    _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value());
}

void MainWindow::updateSuperpixelsWeight()
{
    initSuperpixelsLevel();
    _ui->clWidget->clear();
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
    int value = max(_minSpx, _ui->spinBoxMaxSpx->value());
    _ui->spinBoxMaxSpx->setValue(value);
    _ui->clWidget->setMaximumLevel(value);
    _ui->nbSpxSlider->setMaximum(value);
    _ui->nbSpxSlider->setValue(value);
    _ui->valueNbSpxSlider->setNum(value);

    initSuperpixelsLevel();
    updateSuperpixelsLevel();
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
    _ui->clWidget->deleteSelection();
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save a range image", QString("../data/range_image_labeled/save.bin"), "Binary file (*.bin)");
    _ui->clWidget->saveSelection(filename.toStdString());
}

void MainWindow::displayPixelValues(QPoint pos, QColor col, int label_spx)
{
    QString x_value = QString::number(pos.x());
    QString y_value = QString::number(pos.y());
    QString r_value = QString::number(col.red());
    QString g_value = QString::number(col.green());
    QString b_value = QString::number(col.blue());
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

void MainWindow::displayCursor(int x, int y)
{
    int offset = _ui->vtkWidget->height() + _ui->clWidget->height() / 2;
    _ui->widgetCursor->move(x, y + offset);
}

void MainWindow::switchMode()
{
    if (_isScribble)
    {
        _isScribble = false;
        _ui->clWidget->setScribble(false);
        _ui->selectionButton->setText("Sélection");
        this->setCursor(Qt::ArrowCursor);
        _ui->nbSpxSlider->setVisible(true);
    }
    else
    {
        _isScribble = true;
        _ui->clWidget->setScribble(true);
        _ui->selectionButton->setText("Scribble");
        this->setCursor(Qt::PointingHandCursor);

        //set nb spx on max level and disable it :
        // _ui->valueNbSpxSlider->setNum(_ui->clWidget->nbSpx());
        // _ui->nbSpxSlider->setValue(_ui->clWidget->nbSpx());
        // _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value());
        // _ui->nbSpxSlider->setVisible(false);
    }
}

void MainWindow::switchContours()
{
    if (_showContours)
    {
        _showContours = false;
        _ui->clWidget->setContours(false);
        _ui->contoursButton->setText("Objet");
    }
    else
    {
        _showContours = true;
        _ui->clWidget->setContours(true);
        _ui->contoursButton->setText("Objet + contours");
    }
}
void MainWindow::updateDisplay(int type)
{
    _img = _ui->clWidget->getDisplayMat(type, _isGray, _interpolate, _closing, _equalHist);
    _currentDisplayType = type;
    _ui->clWidget->clear();
    _ui->clWidget->setImgRef(_img);
    initSuperpixelsLevel();
    updateSuperpixelsLevel();
}

void MainWindow::openPointCloud(string filename)
{
    // _pclVisualizer->removeAllPointClouds();
    _pointCloud = getPointCloud(filename);
    _pclVisualizer->addPointCloud<KittiPoint>(_pointCloud, "point_cloud");
}

KittiPointCloud::Ptr MainWindow::getPointCloud(string filename)
{
    KittiPointCloud::Ptr pointcloud(new KittiPointCloud);

    fstream file(filename.c_str(), ios::in | ios::binary);

    if (file.good())
    {
        file.seekg(0, std::ios::beg);
        int i;
        for (i = 0; file.good() && !file.eof(); i++)
        {
            KittiPoint point;
            file.read((char *)&point.x, 3 * sizeof(float));
            // file.read((char *)&point.intensity, sizeof(float));
            float poubelle;
            file.read((char *)&poubelle, sizeof(float)); // Arthur
            point.r = 255;
            point.g = 255;
            point.b = 255;
            point.a = 1;

            pointcloud->push_back(point);
        }
    }
    file.close();

    return pointcloud;
}
