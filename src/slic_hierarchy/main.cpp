#include "mainwindow.h"
#include <QApplication>

#include "rangeimage.h"

int main(int argc, char *argv[])
{
    RangeImage ri(argv[1]);
    return 1;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
