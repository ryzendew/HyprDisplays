/****************************************************************************
** Meta object code from reading C++ file 'displaywidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/displaywidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'displaywidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
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
struct qt_meta_tag_ZN13DisplayWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto DisplayWidget::qt_create_metaobjectdata<qt_meta_tag_ZN13DisplayWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DisplayWidget",
        "displayChanged",
        "",
        "DisplayInfo",
        "display",
        "primaryChanged",
        "name",
        "primary",
        "enabledChanged",
        "enabled",
        "positionChanged",
        "x",
        "y",
        "settingsChanged",
        "onDisplayChanged",
        "onSettingsChanged",
        "onPrimaryToggled",
        "onEnabledToggled",
        "onResolutionChanged",
        "resolution",
        "onRefreshRateChanged",
        "refreshRate",
        "onScaleChanged",
        "scale",
        "onTransformChanged",
        "transform",
        "onPositionChanged",
        "onWorkspaceChanged",
        "workspace",
        "onMirrorChanged",
        "mirrorOf",
        "onPreviewClicked",
        "onAdvancedClicked",
        "onResetClicked",
        "onApplyClicked",
        "onDeleteClicked",
        "onDuplicateClicked",
        "onMirrorClicked",
        "onUnmirrorClicked",
        "onSetPrimaryClicked",
        "onSetSecondaryClicked",
        "onRotate90Clicked",
        "onRotate180Clicked",
        "onRotate270Clicked",
        "onFlipHorizontalClicked",
        "onFlipVerticalClicked",
        "onScale100Clicked",
        "onScale125Clicked",
        "onScale150Clicked",
        "onScale200Clicked",
        "onCustomScaleClicked",
        "selected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'displayChanged'
        QtMocHelpers::SignalData<void(const DisplayInfo &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'primaryChanged'
        QtMocHelpers::SignalData<void(const QString &, bool)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Bool, 7 },
        }}),
        // Signal 'enabledChanged'
        QtMocHelpers::SignalData<void(const QString &, bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Bool, 9 },
        }}),
        // Signal 'positionChanged'
        QtMocHelpers::SignalData<void(const QString &, int, int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
        // Signal 'settingsChanged'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onDisplayChanged'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSettingsChanged'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPrimaryToggled'
        QtMocHelpers::SlotData<void(bool)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Slot 'onEnabledToggled'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Slot 'onResolutionChanged'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
        // Slot 'onRefreshRateChanged'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'onScaleChanged'
        QtMocHelpers::SlotData<void(double)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 23 },
        }}),
        // Slot 'onTransformChanged'
        QtMocHelpers::SlotData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Slot 'onPositionChanged'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onWorkspaceChanged'
        QtMocHelpers::SlotData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 28 },
        }}),
        // Slot 'onMirrorChanged'
        QtMocHelpers::SlotData<void(const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 30 },
        }}),
        // Slot 'onPreviewClicked'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdvancedClicked'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResetClicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyClicked'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDuplicateClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMirrorClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUnmirrorClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSetPrimaryClicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSetSecondaryClicked'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRotate90Clicked'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRotate180Clicked'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRotate270Clicked'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFlipHorizontalClicked'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFlipVerticalClicked'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onScale100Clicked'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onScale125Clicked'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onScale150Clicked'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onScale200Clicked'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCustomScaleClicked'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'selected'
        QtMocHelpers::PropertyData<bool>(51, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'primary'
        QtMocHelpers::PropertyData<bool>(7, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'enabled'
        QtMocHelpers::PropertyData<bool>(9, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DisplayWidget, qt_meta_tag_ZN13DisplayWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DisplayWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13DisplayWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13DisplayWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13DisplayWidgetE_t>.metaTypes,
    nullptr
} };

void DisplayWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DisplayWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->displayChanged((*reinterpret_cast< std::add_pointer_t<DisplayInfo>>(_a[1]))); break;
        case 1: _t->primaryChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->enabledChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 3: _t->positionChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 4: _t->settingsChanged(); break;
        case 5: _t->onDisplayChanged(); break;
        case 6: _t->onSettingsChanged(); break;
        case 7: _t->onPrimaryToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->onEnabledToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->onResolutionChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onRefreshRateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->onScaleChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 12: _t->onTransformChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->onPositionChanged(); break;
        case 14: _t->onWorkspaceChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->onMirrorChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onPreviewClicked(); break;
        case 17: _t->onAdvancedClicked(); break;
        case 18: _t->onResetClicked(); break;
        case 19: _t->onApplyClicked(); break;
        case 20: _t->onDeleteClicked(); break;
        case 21: _t->onDuplicateClicked(); break;
        case 22: _t->onMirrorClicked(); break;
        case 23: _t->onUnmirrorClicked(); break;
        case 24: _t->onSetPrimaryClicked(); break;
        case 25: _t->onSetSecondaryClicked(); break;
        case 26: _t->onRotate90Clicked(); break;
        case 27: _t->onRotate180Clicked(); break;
        case 28: _t->onRotate270Clicked(); break;
        case 29: _t->onFlipHorizontalClicked(); break;
        case 30: _t->onFlipVerticalClicked(); break;
        case 31: _t->onScale100Clicked(); break;
        case 32: _t->onScale125Clicked(); break;
        case 33: _t->onScale150Clicked(); break;
        case 34: _t->onScale200Clicked(); break;
        case 35: _t->onCustomScaleClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DisplayWidget::*)(const DisplayInfo & )>(_a, &DisplayWidget::displayChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DisplayWidget::*)(const QString & , bool )>(_a, &DisplayWidget::primaryChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DisplayWidget::*)(const QString & , bool )>(_a, &DisplayWidget::enabledChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DisplayWidget::*)(const QString & , int , int )>(_a, &DisplayWidget::positionChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DisplayWidget::*)()>(_a, &DisplayWidget::settingsChanged, 4))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isSelected(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isPrimary(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->isEnabled(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setSelected(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setPrimary(*reinterpret_cast<bool*>(_v)); break;
        case 2: _t->setEnabled(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *DisplayWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DisplayWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13DisplayWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DisplayWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 36)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 36;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DisplayWidget::displayChanged(const DisplayInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DisplayWidget::primaryChanged(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DisplayWidget::enabledChanged(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void DisplayWidget::positionChanged(const QString & _t1, int _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void DisplayWidget::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
