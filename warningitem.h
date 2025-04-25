#ifndef WARNINGITEM_H
#define WARNINGITEM_H

#include "bulletitem.h"

class WarningItem : public BulletItem
{
public:

    explicit WarningItem( qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    // 自定义覆盖 advance 或 shape
    // void advance(int phase) override;
    // QPainterPath shape() const override;

};

#endif // WARNINGITEM_H
