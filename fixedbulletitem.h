#ifndef FIXEDBULLETITEM_H
#define FIXEDBULLETITEM_H


#include "bulletitem.h"

class FixedBulletItem : public BulletItem
{
public:

    explicit FixedBulletItem(int damage, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    // 自定义覆盖 advance 或 shape
    // void advance(int phase) override;
    // QPainterPath shape() const override;

};


#endif // FIXEDBULLETITEM_H
