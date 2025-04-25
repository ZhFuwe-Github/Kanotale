#ifndef HOMINGBULLETITEM_H
#define HOMINGBULLETITEM_H

#include "bulletitem.h"
#include <QPointer>

class PlayerHeartItem; // 前向声明

class HomingBulletItem : public BulletItem
{
    Q_OBJECT

public:

    explicit HomingBulletItem(int damage, qreal initialSpeed, PlayerHeartItem *target, const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    void advance(int phase) override;

private:
    QPointer<PlayerHeartItem> targetHeart; // 持有目标指针
    qreal turnRate = 5.0; // 转向速度
    qreal currentMovementAngle; // 当前移动角度
};

#endif // HOMINGBULLETITEM_H
