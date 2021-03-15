#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QColorDialog>
#include <QDebug>

using namespace cv;

string getLabelFileName(QString fileName)
{
    QFileInfo fi(fileName);
    QString base = fi.baseName().append(".label");
    QString path = fi.path();
    QString labelFileName(path);
    labelFileName.append("/../labels/");
    labelFileName.append(base);
    std::cout << "label file : " << labelFileName.toStdString() << std::endl;
    return labelFileName.toStdString();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          _ui(new Ui::MainWindow),
                                          _pclVisualizer(new pcl::visualization::PCLVisualizer("PCL Visualizer", false))
{
    _ui->setupUi(this);

    _ui->cursor->hide();

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
    _ui->statusBar->addWidget(_ui->pixelSpxLabel);
    _ui->vtkWidget->SetRenderWindow(_pclVisualizer->getRenderWindow());
    _pclVisualizer->setBackgroundColor(0.2, 0.2, 0.2);
    _pclVisualizer->setShowFPS(false);

    _ui->valueWeightSlider->setNum(INITIAL_WEIGHT);
    _ui->weightSlider->setValue(INITIAL_WEIGHT);

    _ui->valueNbSpxSlider->setNum(INITIAL_NB_SPX);
    _ui->nbSpxSlider->setValue(INITIAL_NB_SPX);

    connect(_ui->actionOpen_file, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui->nbSpxSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsLevel()));
    connect(_ui->nbSpxSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->weightSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsWeight()));
    connect(_ui->weightSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->resetSelectionButton, SIGNAL(released()), this, SLOT(resetSelection()));
    connect(_ui->saveSelectionButton, SIGNAL(released()), this, SLOT(save()));
    connect(_ui->selectionButton, SIGNAL(released()), this, SLOT(switchMode()));
    connect(_ui->contoursButton, SIGNAL(released()), this, SLOT(switchContours()));
    // bind clear maker button 
    connect(_ui->clearMarkerButton, &QPushButton::released, this, [this]() {updateDisplay(_currentDisplayType); });

    connect(_ui->spinBoxMaxSpx, SIGNAL(editingFinished()), this, SLOT(updateMaxSpxSlider()));
    connect(_ui->spinBoxMaxWeight, SIGNAL(valueChanged(int)), this, SLOT(updateMaxWeightSlider()));

    connect(_ui->clWidget, SIGNAL(pixelValue(QPoint, QColor, int)), this, SLOT(displayPixelValues(QPoint, QColor, int)));
    connect(_ui->clWidget, SIGNAL(mousePos(int, int)), this, SLOT(displayCursor(int, int)));
    connect(_ui->clWidget, SIGNAL(updateSlider(int)), this, SLOT(setNbSpxSlider(int)));

    // Pointcloud buttons
    connect(_ui->whitePointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(); });
    connect(_ui->projectionPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(0); });
    connect(_ui->vtPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(1); });
    connect(_ui->greenPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(2); });

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
    connect(_ui->label_Ground, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_GROUND); });
    connect(_ui->label_Structure, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_STUCTURE); });
    connect(_ui->label_Vehicle, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_VEHICLE); });
    connect(_ui->label_Nature, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_NATURE); });
    connect(_ui->label_Human, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_HUMAN); });
    connect(_ui->label_Object, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_OBJECT); });
    connect(_ui->label_Outlier, &QRadioButton::clicked, this, [this]() { _ui->clWidget->setCurrentLabel(SLIC_LABEL_OUTLIER); });

    // Connect metric CheckBox
    connect(_ui->metric_X, &QCheckBox::clicked, this, [this]() { updateMetrics(SLIC_METRIC_X); });
    connect(_ui->metric_Y, &QCheckBox::clicked, this, [this]() { updateMetrics(SLIC_METRIC_Y); });
    connect(_ui->metric_Z, &QCheckBox::clicked, this, [this]() { updateMetrics(SLIC_METRIC_Z); });
    connect(_ui->metric_remission, &QCheckBox::clicked, this, [this]() { updateMetrics(SLIC_METRIC_REMISSION); });

    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../data/velodyne"), "Binary file (*.bin)");
    if (fileName.isEmpty() || fileName.isNull())
    {
        return;
    }

    _pc = new PointCloud(fileName.toStdString(), getLabelFileName(fileName));

    _pclVisualizer->addPointCloud<KittiPoint>(_pc->getPointCloud(), "point_cloud");
    
    RangeImage ri = _pc->generateRangeImage();
    
    _interpolate = true;
    _img = ri.createColorMat({RI_Y}, _isGray, _interpolate);

    _minSpx = (_img.rows * _img.cols) / (min(_img.rows, _img.cols) * min(_img.rows, _img.cols));

    _ui->display_Y->setChecked(true);
    _ui->display_Interpolation->setChecked(true);
    _ui->label_Ground->setChecked(true);
    _ui->whitePointcloudButton->setChecked(true);

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

    _brushCursor.setColor(QColor(0, 0, 0));
    _brushCursor.setStyle(Qt::SolidPattern);
    _palCursor.setBrush(QPalette::Active, QPalette::Window, _brushCursor);
    _ui->widgetCursor->setPalette(_palCursor);
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open a range image", QString("../data/velodyne"), "Binary file (*.bin)");
    if (fileName == nullptr)
        return;

    if (_pc != nullptr)
        delete _pc;

    _pc = new PointCloud(fileName.toStdString(), getLabelFileName(fileName));
    _pclVisualizer->removeAllPointClouds();
    _pclVisualizer->addPointCloud<KittiPoint>(_pc->getPointCloud(), "point_cloud");
    _ui->vtkWidget->update();

    RangeImage ri = _pc->generateRangeImage();

    _img = ri.createColorMat({RI_Y}, false, true);
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

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}



void MainWindow::initSuperpixelsLevel()
{
    // _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value(), _ui->weightSlider->value(), true);
    _ui->clWidget->initSuperpixels(_ui->nbSpxSlider->maximum(), _ui->weightSlider->value());
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
    QString fileName = QFileDialog::getSaveFileName(this, "Save a range image", QString("../data/range_image_labeled/save.bin"), "Binary file (*.bin)");
    _ui->clWidget->saveSelection(fileName.toStdString());
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
    _ui->cursor->show();
}

void MainWindow::switchMode()
{
    if (_isScribble)
    {
        _isScribble = false;
        _ui->clWidget->setScribble(false);
        _ui->selectionButton->setText("Selection");
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
        _ui->contoursButton->setText("Object");
    }
    else
    {
        _showContours = true;
        _ui->clWidget->setContours(true);
        _ui->contoursButton->setText("Object + contours");
    }
}
void MainWindow::updateDisplay(int type)
{
    _img = _ui->clWidget->getDisplayMat(type, _isGray, _interpolate, _closing, _equalHist);
    _currentDisplayType = type;
    _ui->clWidget->setImgRef(_img);
    _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value());
}

void MainWindow::updateMetrics(int metric)
{
    _ui->clWidget->setMetrics(metric);
    _ui->clWidget->initSuperpixels(_ui->nbSpxSlider->maximum(), _ui->weightSlider->value());
    updateSuperpixelsLevel();
}

void MainWindow::updateColor(int colorMode)
{
    _pc->ChangeColor(colorMode);
    _pclVisualizer->updatePointCloud(_pc->getPointCloud(), "point_cloud");
    _ui->vtkWidget->update();
}
