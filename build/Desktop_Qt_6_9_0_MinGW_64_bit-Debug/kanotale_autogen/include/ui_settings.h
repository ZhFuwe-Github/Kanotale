/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QLabel *label;
    QPushButton *pushButton;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QFrame *frame;

    void setupUi(QWidget *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName("Settings");
        Settings->resize(274, 320);
        Settings->setStyleSheet(QString::fromUtf8(""));
        label = new QLabel(Settings);
        label->setObjectName("label");
        label->setGeometry(QRect(60, 50, 191, 61));
        QFont font;
        font.setFamilies({QString::fromUtf8("Fusion Pixel 8px Monospaced latin")});
        font.setPointSize(36);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("color:rgb(255, 255, 255)"));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        pushButton = new QPushButton(Settings);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(130, 240, 41, 21));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Fusion Pixel 12px Monospaced latin")});
        font1.setPointSize(10);
        pushButton->setFont(font1);
        checkBox = new QCheckBox(Settings);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(80, 140, 161, 31));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(checkBox->sizePolicy().hasHeightForWidth());
        checkBox->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Fusion Pixel 12px Monospaced latin")});
        font2.setPointSize(14);
        checkBox->setFont(font2);
        checkBox->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        checkBox->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        checkBox->setStyleSheet(QString::fromUtf8("QCheckBox::indicator {border: 1px solid white;}\n"
"QCheckBox {spacing: 15px;}\n"
"QCheckBox::indicator:checked { background-color: rgb(223, 209, 10);}"));
        checkBox->setLocale(QLocale(QLocale::Chinese, QLocale::China));
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh));
        checkBox->setIcon(icon);
        checkBox->setIconSize(QSize(24, 24));
        checkBox_2 = new QCheckBox(Settings);
        checkBox_2->setObjectName("checkBox_2");
        checkBox_2->setGeometry(QRect(80, 190, 161, 31));
        sizePolicy.setHeightForWidth(checkBox_2->sizePolicy().hasHeightForWidth());
        checkBox_2->setSizePolicy(sizePolicy);
        checkBox_2->setFont(font2);
        checkBox_2->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        checkBox_2->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        checkBox_2->setStyleSheet(QString::fromUtf8("QCheckBox::indicator {border: 1px solid white;}\n"
"QCheckBox {spacing: 15px;}\n"
"QCheckBox::indicator:checked { background-color: rgb(223, 209, 10);}"));
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        checkBox_2->setIcon(icon1);
        checkBox_2->setIconSize(QSize(24, 24));
        frame = new QFrame(Settings);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(40, 40, 221, 241));
        frame->setStyleSheet(QString::fromUtf8("background-color:rgb(0, 0, 0);\n"
"border: 2px solid white;"));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        frame->setLineWidth(2);
        frame->raise();
        label->raise();
        pushButton->raise();
        checkBox->raise();
        checkBox_2->raise();

        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QWidget *Settings)
    {
        Settings->setWindowTitle(QCoreApplication::translate("Settings", "\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("Settings", "Kanotale", nullptr));
        pushButton->setText(QCoreApplication::translate("Settings", "\347\241\256\345\256\232", nullptr));
        checkBox->setText(QCoreApplication::translate("Settings", " \345\274\200\345\220\257\351\271\277\351\270\243", nullptr));
        checkBox_2->setText(QCoreApplication::translate("Settings", " \350\241\200\351\207\217\351\224\201\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
