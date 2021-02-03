/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[22];
    char stringdata0[280];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 9), // "openImage"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 22), // "updateSuperpixelsLevel"
QT_MOC_LITERAL(4, 45, 20), // "initSuperpixelsLevel"
QT_MOC_LITERAL(5, 66, 23), // "updateSuperpixelsWeight"
QT_MOC_LITERAL(6, 90, 18), // "updateSliderValues"
QT_MOC_LITERAL(7, 109, 18), // "updateMaxSpxSlider"
QT_MOC_LITERAL(8, 128, 21), // "updateMaxWeightSlider"
QT_MOC_LITERAL(9, 150, 14), // "resetSelection"
QT_MOC_LITERAL(10, 165, 4), // "save"
QT_MOC_LITERAL(11, 170, 18), // "displayPixelValues"
QT_MOC_LITERAL(12, 189, 4), // "pPos"
QT_MOC_LITERAL(13, 194, 4), // "pCol"
QT_MOC_LITERAL(14, 199, 9), // "pLabelSpx"
QT_MOC_LITERAL(15, 209, 13), // "displayCursor"
QT_MOC_LITERAL(16, 223, 2), // "pX"
QT_MOC_LITERAL(17, 226, 2), // "pY"
QT_MOC_LITERAL(18, 229, 14), // "setNbSpxSlider"
QT_MOC_LITERAL(19, 244, 9), // "treeLevel"
QT_MOC_LITERAL(20, 254, 10), // "switchMode"
QT_MOC_LITERAL(21, 265, 14) // "switchContours"

    },
    "MainWindow\0openImage\0\0updateSuperpixelsLevel\0"
    "initSuperpixelsLevel\0updateSuperpixelsWeight\0"
    "updateSliderValues\0updateMaxSpxSlider\0"
    "updateMaxWeightSlider\0resetSelection\0"
    "save\0displayPixelValues\0pPos\0pCol\0"
    "pLabelSpx\0displayCursor\0pX\0pY\0"
    "setNbSpxSlider\0treeLevel\0switchMode\0"
    "switchContours"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    0,   90,    2, 0x0a /* Public */,
       9,    0,   91,    2, 0x0a /* Public */,
      10,    0,   92,    2, 0x0a /* Public */,
      11,    3,   93,    2, 0x0a /* Public */,
      15,    2,  100,    2, 0x0a /* Public */,
      18,    1,  105,    2, 0x0a /* Public */,
      20,    0,  108,    2, 0x0a /* Public */,
      21,    0,  109,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QColor, QMetaType::Int,   12,   13,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   16,   17,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openImage(); break;
        case 1: _t->updateSuperpixelsLevel(); break;
        case 2: _t->initSuperpixelsLevel(); break;
        case 3: _t->updateSuperpixelsWeight(); break;
        case 4: _t->updateSliderValues(); break;
        case 5: _t->updateMaxSpxSlider(); break;
        case 6: _t->updateMaxWeightSlider(); break;
        case 7: _t->resetSelection(); break;
        case 8: _t->save(); break;
        case 9: _t->displayPixelValues((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 10: _t->displayCursor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->setNbSpxSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->switchMode(); break;
        case 13: _t->switchContours(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
