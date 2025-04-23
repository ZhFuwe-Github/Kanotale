#ifndef PLAYERHEARTITEM_H
#define PLAYERHEARTITEM_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QKeyEvent>
#include <QPainterPath>
#include <QPainter>

class PlayerHeartItem : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit PlayerHeartItem(const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override; // 声明包围盒
    QPainterPath shape() const override; // 声明精确碰撞形状
    // paint 方法
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    // 驱动场景的更新
    void advance(int phase) override;
    //void setPixmap(const QPixmap &newPixmap);
    void setPixmap(const QString &pixmapPath);

protected:
    // 键盘事件
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QPixmap pixmap; // 存储图像
    qreal speed = 3.0; // 移动速度

    // 记录按键状态
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;

    void updatePosition(); // 更新位置
};

#endif
