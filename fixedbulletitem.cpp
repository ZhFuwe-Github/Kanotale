#include "fixedbulletitem.h"

FixedBulletItem::FixedBulletItem(int damage, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(damage, BulletType::Linear, pixmapPath, parent)
{
    this->speed = 0;
    this->angle = initialAngle;
    this->removable = false;

    setRotation(initialAngle);

    qDebug() << "fixedbulletitem ANGLE :"<< initialAngle;
}

