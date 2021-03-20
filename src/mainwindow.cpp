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

    _ui->valueNbSpxSlider->setNum(INITIAL_NB_SPX);
    _ui->nbSpxSlider->setValue(INITIAL_NB_SPX);

    // Menu bar actions
    connect(_ui->openFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui->openLabels, SIGNAL(triggered()), this, SLOT(openLabels()));
    connect(_ui->saveLabels, SIGNAL(triggered()), this, SLOT(save()));

    connect(_ui->nbSpxSlider, SIGNAL(sliderReleased()), this, SLOT(updateSuperpixelsLevel()));
    connect(_ui->nbSpxSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValues()));
    connect(_ui->resetSelectionButton, SIGNAL(released()), this, SLOT(resetSelection()));
    connect(_ui->selectionButton, SIGNAL(released()), this, SLOT(switchSelection()));
    connect(_ui->contoursButton, SIGNAL(released()), this, SLOT(switchContours()));
    // bind clear maker button
    connect(_ui->clearMarkerButton, &QPushButton::released, this, [this]() { updateDisplay(_currentDisplayType); });

    connect(_ui->spinBoxMaxSpx, SIGNAL(editingFinished()), this, SLOT(updateMaxSpxSlider()));

    connect(_ui->clWidget, SIGNAL(pixelValue(QPoint, QColor, int)), this, SLOT(displayPixelValues(QPoint, QColor, int)));
    connect(_ui->clWidget, SIGNAL(mousePos(int, int)), this, SLOT(displayCursor(int, int)));
    connect(_ui->clWidget, SIGNAL(updateSlider(int)), this, SLOT(setNbSpxSlider(int)));

    // Pointcloud buttons
    connect(_ui->whitePointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(Color::White); });
    connect(_ui->projectionPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(Color::Projection); });
    connect(_ui->groundTruthPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(Color::GroundTruth); });
    connect(_ui->segmentationPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(Color::Segmentation); });
    connect(_ui->propagationPointcloudButton, &QRadioButton::clicked, this, [this]() { updateColor(Color::Propagation); });

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
    _pc = nullptr;
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
    _ui->spinBoxMaxSpx->setValue(MAX_LEVEL);

    _ui->clWidget->initSuperpixels(INITIAL_NB_SPX);
    updateSuperpixelsLevel();

    _brushCursor.setColor(QColor(0, 0, 0));
    _brushCursor.setStyle(Qt::SolidPattern);
    _palCursor.setBrush(QPalette::Active, QPalette::Window, _brushCursor);
    _ui->widgetCursor->setPalette(_palCursor);
}

MainWindow::~MainWindow()
{
    if (_pc != nullptr)
        delete _pc;
    if (_ui != nullptr)
        delete _ui;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open a point cloud", QString("../data/velodyne"), "Binary file (*.bin)");
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

    _ui->clWidget->setImgRef(_img);
    _ui->clWidget->setRangeImage(ri);

    _ui->clWidget->initSuperpixels(INITIAL_NB_SPX);
    updateSuperpixelsLevel();
    _ui->clWidget->clear();

    _ui->nbSpxSlider->setMaximum(MAX_LEVEL);
    _ui->spinBoxMaxSpx->setValue(MAX_LEVEL);

    _ui->valueNbSpxSlider->setNum(INITIAL_NB_SPX);
    _ui->nbSpxSlider->setValue(INITIAL_NB_SPX);

    _ui->statusBar->addWidget(_ui->pixelValuesLabel);
    _ui->statusBar->addWidget(_ui->pixelColorLabel);
}

void MainWindow::openLabels()
{
    if (_pc == nullptr)
    {
        cerr << "need to load pointcloud file" << endl;
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, "Open labels", QString("../data/label"), "Label file (*.label)");
    if (fileName == nullptr)
        return;
    _pc->openLabels(fileName.toStdString());
}

void MainWindow::save()
{
    if (_pc == nullptr)
    {
        cerr << "need to load pointcloud file" << endl;
        return;
    }
    QString QfileName = QFileDialog::getSaveFileName(this, "Save labels", QString("../data"), "Label file (*.label)");
    if (QfileName == nullptr)
        return;
    string fileName = QfileName.toStdString() + ".label";
    _pc->saveLabels(fileName);
}

void MainWindow::initSuperpixelsLevel()
{
    _ui->clWidget->initSuperpixels(_ui->nbSpxSlider->maximum());
}

void MainWindow::updateSuperpixelsLevel()
{
    _ui->clWidget->updateSuperpixels(_ui->nbSpxSlider->value());
}

void MainWindow::updateSliderValues()
{
    _ui->valueNbSpxSlider->setNum(_ui->nbSpxSlider->value());
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

void MainWindow::setNbSpxSlider(int treeLevel)
{
    _ui->valueNbSpxSlider->setNum(treeLevel);
    _ui->nbSpxSlider->setValue(treeLevel);
}

void MainWindow::resetSelection()
{
    _ui->clWidget->deleteSelection();
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

void MainWindow::switchSelection()
{
    if (_isScribble)
    {
        _isScribble = false;
        _ui->clWidget->setScribble(false);
        _ui->selectionButton->setText("Scribble");
        this->setCursor(Qt::ArrowCursor);
        _ui->nbSpxSlider->setVisible(true);
    }
    else
    {
        _isScribble = true;
        _ui->clWidget->setScribble(true);
        _ui->selectionButton->setText("Selection");
        this->setCursor(Qt::PointingHandCursor);
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
    _ui->clWidget->deleteSelection();
    _ui->clWidget->setMetrics(metric);
    _ui->clWidget->initSuperpixels(_ui->nbSpxSlider->maximum());
    updateSuperpixelsLevel();
}

void MainWindow::updateColor(Color colorMode)
{
    if (_pc == nullptr)
    {
        cerr << "need to load pointcloud file " << endl;
        return;
    }
    _pc->ChangeColor(colorMode);
    _pclVisualizer->updatePointCloud(_pc->getPointCloud(), "point_cloud");
    _ui->vtkWidget->update();
    if (colorMode == Color::Segmentation)
    {
        if (!_segmentation)
        {
            _segmentation = !_segmentation;
            _ui->segmentationPointcloudButton->setText("Refresh selection");
        }
    }
    else if (_segmentation)
    {
        _segmentation = !_segmentation;
        _ui->segmentationPointcloudButton->setText("Selected labels");
    }
}