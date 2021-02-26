#-------------------------------------------------
#
# Project created by QtCreator 2020-01-29T11:41:23
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += ../src/mainwindow.cpp \
    ../src/clickablelabel.cpp \
    ../src/superpixelhierarchy.cpp \
    ../src/rangeimage.cpp \
    ../src/pointcloud.cpp \
    ../src/shdist.cpp \

HEADERS  += ../include/mainwindow.h \
    ../include/clickablelabel.h \
    ../include/superpixelhierarchymex.hpp \
    ../include/superpixelhierarchy.h \
    ../include/rangeimage.h \
    ../include/pointcloud.h \
    ../include/shdist.h \

FORMS    += ../src/mainwindow.ui



# add open CV
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}


INCLUDEPATH += /usr/local/include/ \
                ../include/


LIBS += -L  /opt/local/lib/ -lopencv_core

OBJECTS_DIR = ../tmp