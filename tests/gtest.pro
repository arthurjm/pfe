TARGET = test
TEMPLATE = app

include(../src/src.pro)

SOURCES +=  test_main.cpp \
            test_pointcloud.cpp \
            test_rangeimage.cpp \

unix {
    PKGCONFIG += gtest
}

LIBS += -lgtest 

DESTDIR = ../