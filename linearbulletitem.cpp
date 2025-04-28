#include "linearbulletitem.h"

LinearBulletItem::LinearBulletItem(bool rotation , int damage, qreal initialSpeed, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(damage, BulletType::Linear, pixmapPath, parent)
{
    this->speed = initialSpeed;
    this->angle = initialAngle;

    if(rotation)
        setRotation(initialAngle);
    qDebug() << "linearbulletitem ANGLE :"<< initialAngle;
}

