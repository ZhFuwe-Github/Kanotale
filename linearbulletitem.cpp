#include "linearbulletitem.h"

LinearBulletItem::LinearBulletItem(int damage, qreal initialSpeed, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(damage, BulletType::Linear, pixmapPath, parent)
{
    this->speed = initialSpeed;
    this->angle = initialAngle;
    this->removable = removable;

    setRotation(initialAngle);
    qDebug() << "linearbulletitem ANGLE :"<< initialAngle;
}

