QT       += core gui
TARGET = test
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14

SOURCES +=  test_main.cpp \
            test_pointcloud.cpp \
            test_rangeimage.cpp \
            ../src/pointcloud.cpp \
            ../src/rangeimage.cpp

HEADERS += ../include/pointcloud.h \
            ../include/rangeimage.h

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtest
    PKGCONFIG += opencv
}

INCLUDEPATH += /usr/local/include/ \
                ../include/ \

LIBS += -L  /opt/local/lib/ -lgtest -lpthread -lopencv_core
