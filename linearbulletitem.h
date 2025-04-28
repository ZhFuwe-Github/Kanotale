#ifndef LINEARBULLETITEM_H
#define LINEARBULLETITEM_H

#include "bulletitem.h"

class LinearBulletItem : public BulletItem
{
public:

    explicit LinearBulletItem(bool rotation ,int damage, qreal initialSpeed, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    // 自定义覆盖 advance 或 shape
    // void advance(int phase) override;
    // QPainterPath shape() const override;

};

#endif // LINEARBULLETITEM_H
