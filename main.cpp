#include "widget.h" // Include your custom widget header
#include "settings.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFile> // For resource system

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // --- 强制设置深色主题 ---
    //设置 Fusion 样式
    a.setStyle(QStyleFactory::create("Fusion"));
    //创建并设置深色调色板
    QPalette darkPalette;
        // 窗口和控件背景
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53)); // 整体窗口背景 - 深灰
    darkPalette.setColor(QPalette::WindowText, Qt::white);     // 窗口文字 - 白色
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));      // 输入框、列表等背景 - 更深的灰色
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53)); // 交替行背景 (例如表格)
    darkPalette.setColor(QPalette::ToolTipBase, Qt::black);     // 工具提示背景
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);    // 工具提示文字
        // 文本和按钮
    darkPalette.setColor(QPalette::Text, Qt::white);           // 主要文本颜色
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53)); // 按钮背景
    darkPalette.setColor(QPalette::ButtonText, Qt::white);     // 按钮文字
        // 高亮和链接
    darkPalette.setColor(QPalette::BrightText, Qt::red);        // 需要特别突出的文字 (例如错误提示，或根据你的设计调整)
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218)); // 链接颜色
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218)); // 选中项背景 (例如列表、文本框)
    darkPalette.setColor(QPalette::HighlightedText, Qt::black); // 选中项文字 (需要与高亮背景对比)
        // 禁用状态的文字和按钮
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, Qt::darkGray); // 禁用状态下的高亮
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::lightGray);
    //应用全局调色板
    a.setPalette(darkPalette);


    //全局按钮样式
    a.setStyleSheet(
        "QPushButton {"
        "    background-color: rgb(0, 0, 0);"
        "    color: white;"
        "    border: 1px solid rgb(0, 0, 0);"
        "    outline: none;"
        "}"

        "QPushButton:hover {"
        "    border: 1px solid rgb(120, 120, 120);"
        "}"

        "QPushButton:focus {"
        "}"

        "QPushButton:pressed {"
        "    background-color: rgb(50, 50, 50);"
        "    border-color: rgb(42, 130, 218);"
        "}"

        "QPushButton:disabled {"
        "    background-color: rgb(60, 60, 60);"
        "    color: rgb(120, 120, 120);"
        "    border: 1px solid rgb(0, 0, 0);"
        "}"
        );

    BattleWidget w;
    w.setWindowTitle("Kanotale");
    w.resize(800,800);
    w.setMinimumSize(QSize(800,800));
    w.setMaximumSize(QSize(800,800));
    w.show();

    return a.exec();
}
