#include "warningitem.h"

WarningItem::WarningItem( qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(0, BulletType::Linear, pixmapPath, parent)
{
    this->speed = 0;
    this->angle = initialAngle;
    this->removable = false;

    setRotation(initialAngle);
    qDebug() << "warningitem ANGLE :"<< initialAngle;
}
