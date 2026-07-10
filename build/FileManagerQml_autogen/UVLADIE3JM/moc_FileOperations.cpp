/****************************************************************************
** Meta object code from reading C++ file 'FileOperations.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/FileOperations.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileOperations.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14FileOperationsE_t {};
} // unnamed namespace

template <> constexpr inline auto FileOperations::qt_create_metaobjectdata<qt_meta_tag_ZN14FileOperationsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileOperations",
        "QML.Element",
        "auto",
        "mkdir",
        "",
        "parentPath",
        "name",
        "remove",
        "path",
        "isDir",
        "rename",
        "oldPath",
        "newName",
        "urlToPath",
        "QUrl",
        "url",
        "pathToUrl",
        "homePath",
        "documentsPath",
        "desktopPath"
    };

    QtMocHelpers::UintData qt_methods {
        // Method 'mkdir'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(3, 4, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
        }}),
        // Method 'remove'
        QtMocHelpers::MethodData<bool(const QString &, bool)>(7, 4, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { QMetaType::Bool, 9 },
        }}),
        // Method 'rename'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(10, 4, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 11 }, { QMetaType::QString, 12 },
        }}),
        // Method 'urlToPath'
        QtMocHelpers::MethodData<QString(const QUrl &) const>(13, 4, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Method 'pathToUrl'
        QtMocHelpers::MethodData<QUrl(const QString &) const>(16, 4, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::QString, 8 },
        }}),
        // Method 'homePath'
        QtMocHelpers::MethodData<QString() const>(17, 4, QMC::AccessPublic, QMetaType::QString),
        // Method 'documentsPath'
        QtMocHelpers::MethodData<QString() const>(18, 4, QMC::AccessPublic, QMetaType::QString),
        // Method 'desktopPath'
        QtMocHelpers::MethodData<QString() const>(19, 4, QMC::AccessPublic, QMetaType::QString),
        // Method 'parentPath'
        QtMocHelpers::MethodData<QString(const QString &) const>(5, 4, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<FileOperations, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject FileOperations::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileOperationsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileOperationsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14FileOperationsE_t>.metaTypes,
    nullptr
} };

void FileOperations::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileOperations *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = _t->mkdir((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->remove((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->rename((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->urlToPath((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QUrl _r = _t->pathToUrl((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QUrl*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->homePath();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { QString _r = _t->documentsPath();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->desktopPath();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { QString _r = _t->parentPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *FileOperations::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileOperations::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileOperationsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FileOperations::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
