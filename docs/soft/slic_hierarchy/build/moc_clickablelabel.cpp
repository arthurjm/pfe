/****************************************************************************
** Meta object code from reading C++ file 'clickablelabel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../clickablelabel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clickablelabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClickableLabel_t {
    QByteArrayData data[13];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClickableLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClickableLabel_t qt_meta_stringdata_ClickableLabel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ClickableLabel"
QT_MOC_LITERAL(1, 15, 8), // "mousePos"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 2), // "pX"
QT_MOC_LITERAL(4, 28, 2), // "pY"
QT_MOC_LITERAL(5, 31, 10), // "pixelValue"
QT_MOC_LITERAL(6, 42, 4), // "pPos"
QT_MOC_LITERAL(7, 47, 4), // "pCol"
QT_MOC_LITERAL(8, 52, 9), // "pLabelSpx"
QT_MOC_LITERAL(9, 62, 14), // "handleGenerate"
QT_MOC_LITERAL(10, 77, 7), // "pEnable"
QT_MOC_LITERAL(11, 85, 12), // "updateSlider"
QT_MOC_LITERAL(12, 98, 9) // "treeLevel"

    },
    "ClickableLabel\0mousePos\0\0pX\0pY\0"
    "pixelValue\0pPos\0pCol\0pLabelSpx\0"
    "handleGenerate\0pEnable\0updateSlider\0"
    "treeLevel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClickableLabel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    3,   39,    2, 0x06 /* Public */,
       9,    1,   46,    2, 0x06 /* Public */,
      11,    1,   49,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QColor, QMetaType::Int,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Int,   12,

       0        // eod
};

void ClickableLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClickableLabel *_t = static_cast<ClickableLabel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mousePos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->pixelValue((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->handleGenerate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updateSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ClickableLabel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClickableLabel::mousePos)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ClickableLabel::*_t)(QPoint , QColor , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClickableLabel::pixelValue)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ClickableLabel::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClickableLabel::handleGenerate)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ClickableLabel::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClickableLabel::updateSlider)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject ClickableLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_ClickableLabel.data,
      qt_meta_data_ClickableLabel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClickableLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClickableLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClickableLabel.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int ClickableLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ClickableLabel::mousePos(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClickableLabel::pixelValue(QPoint _t1, QColor _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClickableLabel::handleGenerate(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ClickableLabel::updateSlider(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
