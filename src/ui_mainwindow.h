/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_file;
    QWidget *centralWidget;
    QWidget *horizontalWidget;
    QHBoxLayout *globalHorizontal;
    QWidget *widgetImage;
    QVBoxLayout *gloabalVerticalLeft;
    QWidget *widgetSliders;
    QGridLayout *gridLayoutSliders;
    QPushButton *saveSelectionButton;
    QPushButton *resetSelectionButton;
    QLabel *nbSpxLabel;
    QSlider *weightSlider;
    QLabel *valueWeightSlider;
    QLabel *selectedColorLabel;
    QSpinBox *spinBoxMaxSpx;
    QLabel *valueNbSpxSlider;
    QSlider *nbSpxSlider;
    QSpinBox *spinBoxMaxWeight;
    QLabel *weightLabel;
    QWidget *widgetImages;
    QGridLayout *gridImages;
    QHBoxLayout *layoutImages;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *selectionButton;
    QPushButton *contoursButton;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widgetSidebar_1;
    QVBoxLayout *globalVerticalRight;
    QCheckBox *display_Hist;
    QCheckBox *display_Interpolation;
    QCheckBox *display_Closing;
    QCheckBox *display_Gray;
    QRadioButton *display_XYZ;
    QRadioButton *display_X;
    QRadioButton *display_Y;
    QRadioButton *display_Z;
    QRadioButton *display_Depth;
    QRadioButton *display_Remission;
    QWidget *widgetSidebar_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *SHWeight_X;
    QCheckBox *SHWeight_Y;
    QCheckBox *SHWeight_Z;
    QCheckBox *SHWeight_Depth;
    QCheckBox *SHWeight_Remission;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label;
    QLabel *label_2;
    QWidget *widgetCursor;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *cursor;
    QWidget *widgetStatusBar;
    QHBoxLayout *horizontalLayout;
    QLabel *pixelValuesLabel;
    QLabel *pixelColorLabel;
    QLabel *pixelSpxLabel;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1334, 801);
        actionOpen_file = new QAction(MainWindow);
        actionOpen_file->setObjectName(QString::fromUtf8("actionOpen_file"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalWidget = new QWidget(centralWidget);
        horizontalWidget->setObjectName(QString::fromUtf8("horizontalWidget"));
        horizontalWidget->setGeometry(QRect(0, 0, 751, 661));
        globalHorizontal = new QHBoxLayout(horizontalWidget);
        globalHorizontal->setSpacing(10);
        globalHorizontal->setContentsMargins(11, 11, 11, 11);
        globalHorizontal->setObjectName(QString::fromUtf8("globalHorizontal"));
        globalHorizontal->setContentsMargins(10, 0, 0, 0);
        widgetImage = new QWidget(horizontalWidget);
        widgetImage->setObjectName(QString::fromUtf8("widgetImage"));
        widgetImage->setMinimumSize(QSize(30, 30));
        gloabalVerticalLeft = new QVBoxLayout(widgetImage);
        gloabalVerticalLeft->setSpacing(10);
        gloabalVerticalLeft->setContentsMargins(11, 11, 11, 11);
        gloabalVerticalLeft->setObjectName(QString::fromUtf8("gloabalVerticalLeft"));
        gloabalVerticalLeft->setSizeConstraint(QLayout::SetDefaultConstraint);
        gloabalVerticalLeft->setContentsMargins(0, 0, 0, 0);
        widgetSliders = new QWidget(widgetImage);
        widgetSliders->setObjectName(QString::fromUtf8("widgetSliders"));
        widgetSliders->setMinimumSize(QSize(0, 0));
        widgetSliders->setMaximumSize(QSize(16777215, 16777215));
        gridLayoutSliders = new QGridLayout(widgetSliders);
        gridLayoutSliders->setSpacing(5);
        gridLayoutSliders->setContentsMargins(11, 11, 11, 11);
        gridLayoutSliders->setObjectName(QString::fromUtf8("gridLayoutSliders"));
        gridLayoutSliders->setContentsMargins(0, 0, 0, 0);
        saveSelectionButton = new QPushButton(widgetSliders);
        saveSelectionButton->setObjectName(QString::fromUtf8("saveSelectionButton"));
        saveSelectionButton->setMaximumSize(QSize(16777215, 30));

        gridLayoutSliders->addWidget(saveSelectionButton, 0, 4, 1, 1);

        resetSelectionButton = new QPushButton(widgetSliders);
        resetSelectionButton->setObjectName(QString::fromUtf8("resetSelectionButton"));
        resetSelectionButton->setMaximumSize(QSize(16777215, 30));

        gridLayoutSliders->addWidget(resetSelectionButton, 2, 4, 1, 1);

        nbSpxLabel = new QLabel(widgetSliders);
        nbSpxLabel->setObjectName(QString::fromUtf8("nbSpxLabel"));
        nbSpxLabel->setMaximumSize(QSize(16777215, 30));

        gridLayoutSliders->addWidget(nbSpxLabel, 0, 0, 1, 1);

        weightSlider = new QSlider(widgetSliders);
        weightSlider->setObjectName(QString::fromUtf8("weightSlider"));
        weightSlider->setMaximumSize(QSize(16777215, 30));
        weightSlider->setMinimum(1);
        weightSlider->setMaximum(20);
        weightSlider->setValue(10);
        weightSlider->setOrientation(Qt::Horizontal);

        gridLayoutSliders->addWidget(weightSlider, 2, 1, 1, 1);

        valueWeightSlider = new QLabel(widgetSliders);
        valueWeightSlider->setObjectName(QString::fromUtf8("valueWeightSlider"));
        valueWeightSlider->setMaximumSize(QSize(60, 30));

        gridLayoutSliders->addWidget(valueWeightSlider, 2, 2, 1, 1);

        selectedColorLabel = new QLabel(widgetSliders);
        selectedColorLabel->setObjectName(QString::fromUtf8("selectedColorLabel"));
        selectedColorLabel->setMinimumSize(QSize(15, 15));
        selectedColorLabel->setMaximumSize(QSize(16777215, 30));
        selectedColorLabel->setAutoFillBackground(true);

        gridLayoutSliders->addWidget(selectedColorLabel, 0, 5, 1, 1);

        spinBoxMaxSpx = new QSpinBox(widgetSliders);
        spinBoxMaxSpx->setObjectName(QString::fromUtf8("spinBoxMaxSpx"));
        spinBoxMaxSpx->setMaximumSize(QSize(16777215, 30));
        spinBoxMaxSpx->setMinimum(1);
        spinBoxMaxSpx->setMaximum(10000);
        spinBoxMaxSpx->setSingleStep(100);

        gridLayoutSliders->addWidget(spinBoxMaxSpx, 0, 3, 1, 1);

        valueNbSpxSlider = new QLabel(widgetSliders);
        valueNbSpxSlider->setObjectName(QString::fromUtf8("valueNbSpxSlider"));
        valueNbSpxSlider->setMinimumSize(QSize(40, 0));
        valueNbSpxSlider->setMaximumSize(QSize(60, 30));
        valueNbSpxSlider->setBaseSize(QSize(0, 0));

        gridLayoutSliders->addWidget(valueNbSpxSlider, 0, 2, 1, 1);

        nbSpxSlider = new QSlider(widgetSliders);
        nbSpxSlider->setObjectName(QString::fromUtf8("nbSpxSlider"));
        nbSpxSlider->setMaximumSize(QSize(16777215, 30));
        nbSpxSlider->setMinimum(1);
        nbSpxSlider->setMaximum(1000);
        nbSpxSlider->setValue(300);
        nbSpxSlider->setOrientation(Qt::Horizontal);

        gridLayoutSliders->addWidget(nbSpxSlider, 0, 1, 1, 1);

        spinBoxMaxWeight = new QSpinBox(widgetSliders);
        spinBoxMaxWeight->setObjectName(QString::fromUtf8("spinBoxMaxWeight"));
        spinBoxMaxWeight->setMaximumSize(QSize(16777215, 30));
        spinBoxMaxWeight->setMinimum(1);
        spinBoxMaxWeight->setMaximum(100);
        spinBoxMaxWeight->setSingleStep(10);

        gridLayoutSliders->addWidget(spinBoxMaxWeight, 2, 3, 1, 1);

        weightLabel = new QLabel(widgetSliders);
        weightLabel->setObjectName(QString::fromUtf8("weightLabel"));
        weightLabel->setMaximumSize(QSize(16777215, 30));

        gridLayoutSliders->addWidget(weightLabel, 2, 0, 1, 1);


        gloabalVerticalLeft->addWidget(widgetSliders);

        widgetImages = new QWidget(widgetImage);
        widgetImages->setObjectName(QString::fromUtf8("widgetImages"));
        widgetImages->setMaximumSize(QSize(16777215, 16777215));
        gridImages = new QGridLayout(widgetImages);
        gridImages->setSpacing(6);
        gridImages->setContentsMargins(11, 11, 11, 11);
        gridImages->setObjectName(QString::fromUtf8("gridImages"));
        gridImages->setHorizontalSpacing(0);
        gridImages->setVerticalSpacing(10);
        gridImages->setContentsMargins(0, 0, 0, 0);
        layoutImages = new QHBoxLayout();
        layoutImages->setSpacing(6);
        layoutImages->setObjectName(QString::fromUtf8("layoutImages"));

        gridImages->addLayout(layoutImages, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(10);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        selectionButton = new QPushButton(widgetImages);
        selectionButton->setObjectName(QString::fromUtf8("selectionButton"));
        selectionButton->setMinimumSize(QSize(0, 25));
        selectionButton->setMaximumSize(QSize(16777215, 25));
        selectionButton->setCheckable(false);
        selectionButton->setChecked(false);

        horizontalLayout_3->addWidget(selectionButton);

        contoursButton = new QPushButton(widgetImages);
        contoursButton->setObjectName(QString::fromUtf8("contoursButton"));
        contoursButton->setMinimumSize(QSize(0, 25));
        contoursButton->setMaximumSize(QSize(16777215, 25));
        contoursButton->setCheckable(false);
        contoursButton->setChecked(false);

        horizontalLayout_3->addWidget(contoursButton);


        gridImages->addLayout(horizontalLayout_3, 2, 0, 1, 1);


        gloabalVerticalLeft->addWidget(widgetImages);


        globalHorizontal->addWidget(widgetImage);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        widgetSidebar_1 = new QWidget(horizontalWidget);
        widgetSidebar_1->setObjectName(QString::fromUtf8("widgetSidebar_1"));
        widgetSidebar_1->setEnabled(true);
        widgetSidebar_1->setMaximumSize(QSize(16777215, 16777215));
        globalVerticalRight = new QVBoxLayout(widgetSidebar_1);
        globalVerticalRight->setSpacing(5);
        globalVerticalRight->setContentsMargins(11, 11, 11, 11);
        globalVerticalRight->setObjectName(QString::fromUtf8("globalVerticalRight"));
        globalVerticalRight->setContentsMargins(0, 0, 0, 0);
        display_Hist = new QCheckBox(widgetSidebar_1);
        display_Hist->setObjectName(QString::fromUtf8("display_Hist"));

        globalVerticalRight->addWidget(display_Hist);

        display_Interpolation = new QCheckBox(widgetSidebar_1);
        display_Interpolation->setObjectName(QString::fromUtf8("display_Interpolation"));
        display_Interpolation->setMinimumSize(QSize(0, 0));

        globalVerticalRight->addWidget(display_Interpolation);

        display_Closing = new QCheckBox(widgetSidebar_1);
        display_Closing->setObjectName(QString::fromUtf8("display_Closing"));
        display_Closing->setMinimumSize(QSize(0, 0));

        globalVerticalRight->addWidget(display_Closing);

        display_Gray = new QCheckBox(widgetSidebar_1);
        display_Gray->setObjectName(QString::fromUtf8("display_Gray"));

        globalVerticalRight->addWidget(display_Gray);

        display_XYZ = new QRadioButton(widgetSidebar_1);
        display_XYZ->setObjectName(QString::fromUtf8("display_XYZ"));

        globalVerticalRight->addWidget(display_XYZ);

        display_X = new QRadioButton(widgetSidebar_1);
        display_X->setObjectName(QString::fromUtf8("display_X"));

        globalVerticalRight->addWidget(display_X);

        display_Y = new QRadioButton(widgetSidebar_1);
        display_Y->setObjectName(QString::fromUtf8("display_Y"));

        globalVerticalRight->addWidget(display_Y);

        display_Z = new QRadioButton(widgetSidebar_1);
        display_Z->setObjectName(QString::fromUtf8("display_Z"));

        globalVerticalRight->addWidget(display_Z);

        display_Depth = new QRadioButton(widgetSidebar_1);
        display_Depth->setObjectName(QString::fromUtf8("display_Depth"));

        globalVerticalRight->addWidget(display_Depth);

        display_Remission = new QRadioButton(widgetSidebar_1);
        display_Remission->setObjectName(QString::fromUtf8("display_Remission"));

        globalVerticalRight->addWidget(display_Remission);


        horizontalLayout_2->addWidget(widgetSidebar_1);

        widgetSidebar_2 = new QWidget(horizontalWidget);
        widgetSidebar_2->setObjectName(QString::fromUtf8("widgetSidebar_2"));
        widgetSidebar_2->setEnabled(true);
        verticalLayout = new QVBoxLayout(widgetSidebar_2);
        verticalLayout->setSpacing(5);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        SHWeight_X = new QCheckBox(widgetSidebar_2);
        SHWeight_X->setObjectName(QString::fromUtf8("SHWeight_X"));

        verticalLayout->addWidget(SHWeight_X);

        SHWeight_Y = new QCheckBox(widgetSidebar_2);
        SHWeight_Y->setObjectName(QString::fromUtf8("SHWeight_Y"));

        verticalLayout->addWidget(SHWeight_Y);

        SHWeight_Z = new QCheckBox(widgetSidebar_2);
        SHWeight_Z->setObjectName(QString::fromUtf8("SHWeight_Z"));

        verticalLayout->addWidget(SHWeight_Z);

        SHWeight_Depth = new QCheckBox(widgetSidebar_2);
        SHWeight_Depth->setObjectName(QString::fromUtf8("SHWeight_Depth"));

        verticalLayout->addWidget(SHWeight_Depth);

        SHWeight_Remission = new QCheckBox(widgetSidebar_2);
        SHWeight_Remission->setObjectName(QString::fromUtf8("SHWeight_Remission"));

        verticalLayout->addWidget(SHWeight_Remission);


        horizontalLayout_2->addWidget(widgetSidebar_2);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label = new QLabel(horizontalWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(50, 30));
        label->setMaximumSize(QSize(80, 30));
        label->setTextFormat(Qt::AutoText);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(label);

        label_2 = new QLabel(horizontalWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(50, 30));
        label_2->setMaximumSize(QSize(80, 30));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setScaledContents(false);
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(label_2);


        gridLayout->addLayout(horizontalLayout_5, 0, 0, 1, 1);


        globalHorizontal->addLayout(gridLayout);

        widgetCursor = new QWidget(centralWidget);
        widgetCursor->setObjectName(QString::fromUtf8("widgetCursor"));
        widgetCursor->setGeometry(QRect(1310, 0, 21, 23));
        horizontalLayout_4 = new QHBoxLayout(widgetCursor);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        cursor = new QRadioButton(widgetCursor);
        cursor->setObjectName(QString::fromUtf8("cursor"));
        cursor->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cursor->sizePolicy().hasHeightForWidth());
        cursor->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(138, 226, 52, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(198, 255, 143, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(168, 240, 97, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(69, 113, 26, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(92, 151, 34, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush6(QColor(255, 255, 255, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        QBrush brush7(QColor(252, 175, 62, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush7);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        QBrush brush8(QColor(196, 240, 153, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush8);
        QBrush brush9(QColor(255, 255, 220, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush9);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        QBrush brush10(QColor(0, 0, 0, 128));
        brush10.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush10);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush9);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        QBrush brush11(QColor(0, 0, 0, 128));
        brush11.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush11);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush9);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        QBrush brush12(QColor(0, 0, 0, 128));
        brush12.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush12);
#endif
        cursor->setPalette(palette);
        cursor->setAutoFillBackground(false);
        cursor->setIconSize(QSize(16, 16));
        cursor->setCheckable(true);
        cursor->setChecked(true);

        horizontalLayout_4->addWidget(cursor);

        widgetStatusBar = new QWidget(centralWidget);
        widgetStatusBar->setObjectName(QString::fromUtf8("widgetStatusBar"));
        widgetStatusBar->setGeometry(QRect(10, 650, 636, 214));
        horizontalLayout = new QHBoxLayout(widgetStatusBar);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pixelValuesLabel = new QLabel(widgetStatusBar);
        pixelValuesLabel->setObjectName(QString::fromUtf8("pixelValuesLabel"));

        horizontalLayout->addWidget(pixelValuesLabel);

        pixelColorLabel = new QLabel(widgetStatusBar);
        pixelColorLabel->setObjectName(QString::fromUtf8("pixelColorLabel"));
        pixelColorLabel->setMinimumSize(QSize(15, 10));
        pixelColorLabel->setAutoFillBackground(true);

        horizontalLayout->addWidget(pixelColorLabel);

        pixelSpxLabel = new QLabel(widgetStatusBar);
        pixelSpxLabel->setObjectName(QString::fromUtf8("pixelSpxLabel"));

        horizontalLayout->addWidget(pixelSpxLabel);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1334, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_file);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Fast Hierarchy", nullptr));
        actionOpen_file->setText(QCoreApplication::translate("MainWindow", "&Open file", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen_file->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        saveSelectionButton->setText(QCoreApplication::translate("MainWindow", "Save Selection", nullptr));
        resetSelectionButton->setText(QCoreApplication::translate("MainWindow", "Reset Selection", nullptr));
        nbSpxLabel->setText(QCoreApplication::translate("MainWindow", "Number of superpixels", nullptr));
        valueWeightSlider->setText(QCoreApplication::translate("MainWindow", "20", nullptr));
        selectedColorLabel->setText(QString());
        valueNbSpxSlider->setText(QCoreApplication::translate("MainWindow", "300", nullptr));
        weightLabel->setText(QCoreApplication::translate("MainWindow", "Weight", nullptr));
        selectionButton->setText(QCoreApplication::translate("MainWindow", "S\303\251lection", nullptr));
        contoursButton->setText(QCoreApplication::translate("MainWindow", "Objet + contours", nullptr));
        display_Hist->setText(QCoreApplication::translate("MainWindow", "EqualHist", nullptr));
        display_Interpolation->setText(QCoreApplication::translate("MainWindow", "Interpolation", nullptr));
        display_Closing->setText(QCoreApplication::translate("MainWindow", "Closing", nullptr));
        display_Gray->setText(QCoreApplication::translate("MainWindow", "Gray", nullptr));
        display_XYZ->setText(QCoreApplication::translate("MainWindow", "XYZ", nullptr));
        display_X->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        display_Y->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        display_Z->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        display_Depth->setText(QCoreApplication::translate("MainWindow", "Depth", nullptr));
        display_Remission->setText(QCoreApplication::translate("MainWindow", "Remission", nullptr));
        SHWeight_X->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        SHWeight_Y->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        SHWeight_Z->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        SHWeight_Depth->setText(QCoreApplication::translate("MainWindow", "Depth", nullptr));
        SHWeight_Remission->setText(QCoreApplication::translate("MainWindow", "Remission", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Display", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "SHWeight", nullptr));
        cursor->setText(QString());
        pixelValuesLabel->setText(QString());
        pixelColorLabel->setText(QString());
        pixelSpxLabel->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "Fi&le", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
