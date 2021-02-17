QT       += core gui
TARGET = test
TEMPLATE = app

INCLUDEPATH += ../slic_hierarchy/

SOURCES +=  test_main.cpp \
            test_pointcloud.cpp \
            test_rangeimage.cpp \
            ../slic_hierarchy/pointcloud.cpp \
            ../slic_hierarchy/rangeimage.cpp

HEADERS += ../slic_hierarchy/pointcloud.h \
            ../slic_hierarchy/rangeimage.h

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtest
}

INCLUDEPATH += /usr/local/include/

LIBS += -L  /opt/local/lib/ -lgtest -lpthread
