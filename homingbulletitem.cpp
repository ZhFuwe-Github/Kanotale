#include "homingbulletitem.h"
#include "playerheartitem.h"
#include "qrandom.h"
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

HomingBulletItem::HomingBulletItem(int damage, qreal initialSpeed, PlayerHeartItem *target, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(damage, BulletType::Generic, pixmapPath, parent),
    targetHeart(target)
{
    this->speed = initialSpeed;

    currentMovementAngle = QRandomGenerator::global()->bounded(360.0); // 初始方向随机
    setRotation(currentMovementAngle); //朝向初始移动方向
}

void HomingBulletItem::advance(int phase)
{
    if (phase == 0) return; // 只在移动阶段处理

    // --- 追踪逻辑 ---
    if (!targetHeart.isNull()) { // 检查目标指针是否仍然有效
        // 获取位置
        QPointF targetPos = targetHeart->pos();
        QPointF currentPos = this->pos();

        // 计算指向目标的向量和角度
        QPointF vectorToTarget = targetPos - currentPos;
        qreal angleToTarget = qRadiansToDegrees(qAtan2(vectorToTarget.y(), vectorToTarget.x()));

        // 计算角度差
        qreal angleDiff = angleToTarget - currentMovementAngle;
        while (angleDiff <= -180.0) angleDiff += 360.0;
        while (angleDiff > 180.0) angleDiff -= 360.0;

        // 根据转向速度调整当前移动角度
        qreal angleChange = qBound(-turnRate, angleDiff, turnRate); // 限制每帧最大转向角度
        currentMovementAngle += angleChange;

        // 更新图形的旋转
        setRotation(currentMovementAngle);

    }

    // 进行移动
    qreal rad = qDegreesToRadians(currentMovementAngle);
    qreal dx = speed * qCos(rad);
    qreal dy = speed * qSin(rad);
    setPos(pos() + QPointF(dx, dy));
}
