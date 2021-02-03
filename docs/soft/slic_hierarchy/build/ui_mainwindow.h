/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_file;
    QWidget *centralWidget;
    QWidget *widgetImages;
    QGridLayout *gridLayoutMainWindow;
    QHBoxLayout *layoutImages;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *selectionButton;
    QPushButton *contoursButton;
    QWidget *widgetSliders;
    QGridLayout *gridLayoutSliders;
    QLabel *valueWeightSlider;
    QLabel *weightLabel;
    QSlider *weightSlider;
    QSlider *nbSpxSlider;
    QLabel *nbSpxLabel;
    QLabel *valueNbSpxSlider;
    QLabel *selectedColorLabel;
    QPushButton *resetSelectionButton;
    QPushButton *saveSelectionButton;
    QWidget *widgetStatusBar;
    QHBoxLayout *horizontalLayout;
    QLabel *pixelValuesLabel;
    QLabel *pixelColorLabel;
    QLabel *pixelSpxLabel;
    QWidget *widgetCursor;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *cursor2;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1217, 801);
        actionOpen_file = new QAction(MainWindow);
        actionOpen_file->setObjectName(QStringLiteral("actionOpen_file"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widgetImages = new QWidget(centralWidget);
        widgetImages->setObjectName(QStringLiteral("widgetImages"));
        widgetImages->setGeometry(QRect(20, 120, 911, 511));
        gridLayoutMainWindow = new QGridLayout(widgetImages);
        gridLayoutMainWindow->setSpacing(6);
        gridLayoutMainWindow->setContentsMargins(11, 11, 11, 11);
        gridLayoutMainWindow->setObjectName(QStringLiteral("gridLayoutMainWindow"));
        gridLayoutMainWindow->setContentsMargins(0, 0, 0, 0);
        layoutImages = new QHBoxLayout();
        layoutImages->setSpacing(6);
        layoutImages->setObjectName(QStringLiteral("layoutImages"));

        gridLayoutMainWindow->addLayout(layoutImages, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        selectionButton = new QPushButton(widgetImages);
        selectionButton->setObjectName(QStringLiteral("selectionButton"));
        selectionButton->setMinimumSize(QSize(0, 23));
        selectionButton->setMaximumSize(QSize(16777215, 16777215));
        selectionButton->setCheckable(false);
        selectionButton->setChecked(false);

        horizontalLayout_3->addWidget(selectionButton);

        contoursButton = new QPushButton(widgetImages);
        contoursButton->setObjectName(QStringLiteral("contoursButton"));
        contoursButton->setMinimumSize(QSize(0, 23));
        contoursButton->setMaximumSize(QSize(16777215, 16777215));
        contoursButton->setCheckable(false);
        contoursButton->setChecked(false);

        horizontalLayout_3->addWidget(contoursButton);


        gridLayoutMainWindow->addLayout(horizontalLayout_3, 2, 0, 1, 1);

        widgetSliders = new QWidget(centralWidget);
        widgetSliders->setObjectName(QStringLiteral("widgetSliders"));
        widgetSliders->setGeometry(QRect(30, 20, 901, 80));
        gridLayoutSliders = new QGridLayout(widgetSliders);
        gridLayoutSliders->setSpacing(6);
        gridLayoutSliders->setContentsMargins(11, 11, 11, 11);
        gridLayoutSliders->setObjectName(QStringLiteral("gridLayoutSliders"));
        gridLayoutSliders->setContentsMargins(0, 0, 0, 0);
        valueWeightSlider = new QLabel(widgetSliders);
        valueWeightSlider->setObjectName(QStringLiteral("valueWeightSlider"));

        gridLayoutSliders->addWidget(valueWeightSlider, 2, 2, 1, 1);

        weightLabel = new QLabel(widgetSliders);
        weightLabel->setObjectName(QStringLiteral("weightLabel"));

        gridLayoutSliders->addWidget(weightLabel, 2, 0, 1, 1);

        weightSlider = new QSlider(widgetSliders);
        weightSlider->setObjectName(QStringLiteral("weightSlider"));
        weightSlider->setMinimum(5);
        weightSlider->setMaximum(200);
        weightSlider->setValue(20);
        weightSlider->setOrientation(Qt::Horizontal);

        gridLayoutSliders->addWidget(weightSlider, 2, 1, 1, 1);

        nbSpxSlider = new QSlider(widgetSliders);
        nbSpxSlider->setObjectName(QStringLiteral("nbSpxSlider"));
        nbSpxSlider->setMinimum(1);
        nbSpxSlider->setMaximum(1000);
        nbSpxSlider->setValue(300);
        nbSpxSlider->setOrientation(Qt::Horizontal);

        gridLayoutSliders->addWidget(nbSpxSlider, 0, 1, 1, 1);

        nbSpxLabel = new QLabel(widgetSliders);
        nbSpxLabel->setObjectName(QStringLiteral("nbSpxLabel"));

        gridLayoutSliders->addWidget(nbSpxLabel, 0, 0, 1, 1);

        valueNbSpxSlider = new QLabel(widgetSliders);
        valueNbSpxSlider->setObjectName(QStringLiteral("valueNbSpxSlider"));
        valueNbSpxSlider->setMinimumSize(QSize(100, 0));
        valueNbSpxSlider->setBaseSize(QSize(0, 0));

        gridLayoutSliders->addWidget(valueNbSpxSlider, 0, 2, 1, 1);

        selectedColorLabel = new QLabel(widgetSliders);
        selectedColorLabel->setObjectName(QStringLiteral("selectedColorLabel"));
        selectedColorLabel->setMinimumSize(QSize(15, 15));
        selectedColorLabel->setAutoFillBackground(true);

        gridLayoutSliders->addWidget(selectedColorLabel, 0, 5, 1, 1);

        resetSelectionButton = new QPushButton(widgetSliders);
        resetSelectionButton->setObjectName(QStringLiteral("resetSelectionButton"));

        gridLayoutSliders->addWidget(resetSelectionButton, 2, 4, 1, 1);

        saveSelectionButton = new QPushButton(widgetSliders);
        saveSelectionButton->setObjectName(QStringLiteral("saveSelectionButton"));

        gridLayoutSliders->addWidget(saveSelectionButton, 0, 4, 1, 1);

        widgetStatusBar = new QWidget(centralWidget);
        widgetStatusBar->setObjectName(QStringLiteral("widgetStatusBar"));
        widgetStatusBar->setGeometry(QRect(20, 650, 97, 19));
        horizontalLayout = new QHBoxLayout(widgetStatusBar);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pixelValuesLabel = new QLabel(widgetStatusBar);
        pixelValuesLabel->setObjectName(QStringLiteral("pixelValuesLabel"));

        horizontalLayout->addWidget(pixelValuesLabel);

        pixelColorLabel = new QLabel(widgetStatusBar);
        pixelColorLabel->setObjectName(QStringLiteral("pixelColorLabel"));
        pixelColorLabel->setMinimumSize(QSize(15, 10));
        pixelColorLabel->setAutoFillBackground(true);

        horizontalLayout->addWidget(pixelColorLabel);

        pixelSpxLabel = new QLabel(widgetStatusBar);
        pixelSpxLabel->setObjectName(QStringLiteral("pixelSpxLabel"));

        horizontalLayout->addWidget(pixelSpxLabel);

        widgetCursor = new QWidget(centralWidget);
        widgetCursor->setObjectName(QStringLiteral("widgetCursor"));
        widgetCursor->setGeometry(QRect(940, 120, 21, 23));
        horizontalLayout_2 = new QHBoxLayout(widgetCursor);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        cursor2 = new QRadioButton(widgetCursor);
        cursor2->setObjectName(QStringLiteral("cursor2"));
        cursor2->setEnabled(false);
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
        cursor2->setPalette(palette);
        cursor2->setAutoFillBackground(false);
        cursor2->setCheckable(true);
        cursor2->setChecked(true);

        horizontalLayout_2->addWidget(cursor2);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1217, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_file);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionOpen_file->setText(QApplication::translate("MainWindow", "&Open file", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionOpen_file->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        selectionButton->setText(QApplication::translate("MainWindow", "S\303\251lection", Q_NULLPTR));
        contoursButton->setText(QApplication::translate("MainWindow", "Objet + contours", Q_NULLPTR));
        valueWeightSlider->setText(QApplication::translate("MainWindow", "20", Q_NULLPTR));
        weightLabel->setText(QApplication::translate("MainWindow", "Weight", Q_NULLPTR));
        nbSpxLabel->setText(QApplication::translate("MainWindow", "Number of superpixels", Q_NULLPTR));
        valueNbSpxSlider->setText(QApplication::translate("MainWindow", "300", Q_NULLPTR));
        selectedColorLabel->setText(QString());
        resetSelectionButton->setText(QApplication::translate("MainWindow", "Reset Selection", Q_NULLPTR));
        saveSelectionButton->setText(QApplication::translate("MainWindow", "Save Selection", Q_NULLPTR));
        pixelValuesLabel->setText(QString());
        pixelColorLabel->setText(QString());
        pixelSpxLabel->setText(QString());
        cursor2->setText(QString());
        menuFile->setTitle(QApplication::translate("MainWindow", "Fi&le", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
