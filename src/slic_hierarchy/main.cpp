#include <QApplication>

#include "mainwindow.h"
#include "pointcloud.h"
#include "rangeimage.h"

void test_put()
{
    auto depth = nc::random::randInt<int>(nc::Shape(1, 10), 0, 10);
    std::cout << "depth:" << std::endl;
    depth.print();
    // auto indices = nc::arange<int>(depth.shape().cols); // validé

    auto order = nc::argsort(depth, nc::Axis::COL); // validé
    order = nc::flip(order, nc::Axis::COL);         // validé
    order.print();

    // La logique
    // out[indices[i]] = depth[i]  // avec numcpp et put
    // out[i] = depth[indices[i]]  // en python dans laserscan.py
    // le tableau devrait ressembler à ça pour suivre la logique de put et numcpp
    // std::cout << "[[2, ?, 1, 0, 9, ?, ?, 8, ?, ?]]" << std::endl;

    // auto in = nc::zeros<int>(1, 10);
    auto out = nc::random::randInt<int>(nc::Shape(1, 10), 10, 20);
    // out.print();

    // order.fill(5);
    // order.print();
    depth = nc::put(depth, order, depth);

    //out.put(order, depth);
    std::cout << "Resultat" << std::endl;
    out.print();
}

void trie(nc::NdArray<int> &array, nc::NdArray<unsigned int> index)
{
    auto array2 = array.copy();
    for (uint32_t i = 0; i < array.size(); i++)
    {
        array2[i] = array[index[i]];
    }
    array = array2;
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        PointCloud cp(argv[1]);
        RangeImage ri(cp);
        // ri.loadRangeImage("../000242_jz.bin");
        // std::cout << ri._data2[0].x << " " << ri._data[0].x << std::endl;
    }

    // test_put();

    // QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    // return a.exec();
}
