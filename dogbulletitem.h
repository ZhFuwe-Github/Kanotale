#ifndef DOGBULLETITEM_H
#define DOGBULLETITEM_H

#include "linearbulletitem.h"
#include <QPixmap>

class DogBulletItem : public LinearBulletItem
{
    Q_OBJECT

public:
    explicit DogBulletItem(int damage, qreal initialSpeed, qreal initialAngle,
                           const QString &pixmapPath1, const QString &pixmapPath2,
                           QGraphicsItem *parent = nullptr);
    // 重写碰撞形状
    QPainterPath shape() const override;
    // 重写 advance 处理动画切换
    void advance(int phase) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;


private:
    QPixmap pixmap1;
    QPixmap pixmap2;
    QPixmap *currentPixmap; // 指向当前要显示的 pixmap
    int frameCounter = 0;    // 帧计数器
    int switchInterval = 15; // 每隔多少帧切换一次图像 (可以调整)
};

#endif // DOGBULLETITEM_H
