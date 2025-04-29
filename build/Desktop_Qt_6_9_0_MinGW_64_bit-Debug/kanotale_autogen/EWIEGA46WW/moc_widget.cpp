/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../widget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
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
struct qt_meta_tag_ZN12BattleWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto BattleWidget::qt_create_metaobjectdata<qt_meta_tag_ZN12BattleWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "BattleWidget",
        "onFightClicked",
        "",
        "onActClicked",
        "onItemClicked",
        "onMercyClicked",
        "updateDialogueText",
        "updateEDialogueText",
        "onSettingsClicked",
        "applySettings",
        "soundEnabled",
        "hpFixEnabled",
        "handleBatchSpawn",
        "handleBatchSpawnW",
        "handleBatchSpawnC",
        "onAttackStickStopped",
        "elapsedTime",
        "onActCheckClicked",
        "onActTalkClicked",
        "onActAskClicked",
        "onActFondleClicked",
        "onActThreatenClicked",
        "onItemButton1Clicked",
        "onItemButton2Clicked",
        "onItemButton3Clicked",
        "onMercySpareClicked",
        "onMercyFleeClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onFightClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onItemClicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMercyClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDialogueText'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateEDialogueText'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSettingsClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'applySettings'
        QtMocHelpers::SlotData<void(bool, bool)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 10 }, { QMetaType::Bool, 11 },
        }}),
        // Slot 'handleBatchSpawn'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleBatchSpawnW'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleBatchSpawnC'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAttackStickStopped'
        QtMocHelpers::SlotData<void(qint64)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 16 },
        }}),
        // Slot 'onActCheckClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActTalkClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActAskClicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActFondleClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActThreatenClicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onItemButton1Clicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onItemButton2Clicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onItemButton3Clicked'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMercySpareClicked'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMercyFleeClicked'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<BattleWidget, qt_meta_tag_ZN12BattleWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject BattleWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12BattleWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12BattleWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12BattleWidgetE_t>.metaTypes,
    nullptr
} };

void BattleWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BattleWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onFightClicked(); break;
        case 1: _t->onActClicked(); break;
        case 2: _t->onItemClicked(); break;
        case 3: _t->onMercyClicked(); break;
        case 4: _t->updateDialogueText(); break;
        case 5: _t->updateEDialogueText(); break;
        case 6: _t->onSettingsClicked(); break;
        case 7: _t->applySettings((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 8: _t->handleBatchSpawn(); break;
        case 9: _t->handleBatchSpawnW(); break;
        case 10: _t->handleBatchSpawnC(); break;
        case 11: _t->onAttackStickStopped((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 12: _t->onActCheckClicked(); break;
        case 13: _t->onActTalkClicked(); break;
        case 14: _t->onActAskClicked(); break;
        case 15: _t->onActFondleClicked(); break;
        case 16: _t->onActThreatenClicked(); break;
        case 17: _t->onItemButton1Clicked(); break;
        case 18: _t->onItemButton2Clicked(); break;
        case 19: _t->onItemButton3Clicked(); break;
        case 20: _t->onMercySpareClicked(); break;
        case 21: _t->onMercyFleeClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *BattleWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BattleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12BattleWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BattleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }
    return _id;
}
QT_WARNING_POP
