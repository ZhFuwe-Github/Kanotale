#include "attackstick.h"
#include <QDebug>

AttackStick::AttackStick(int damage, qreal initialSpeed, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent)
    : BulletItem(damage, BulletType::Linear, pixmapPath, parent)
{

    this->speed = initialSpeed;
    this->angle = initialAngle;
    this->removable = false;

    setRotation(initialAngle);
    setFlag(QGraphicsItem::ItemIsFocusable); // 设置为可获得焦点
}

bool AttackStick::getMoving(){
    return isMoving;
}

void AttackStick::startMovingTimer() {
    if (!movementTimer.isValid()) { // 只在未启动时启动
        movementTimer.start();
        isMoving = true;
        qDebug() << "AttackStick movement timer started.";
    }
}

// 覆盖基类的 advance
void AttackStick::advance(int phase)
{
    if (phase == 0 || !isMoving) return; // 如果不在移动阶段或已停止，则不移动

    // 移动逻辑
    qreal rad = qDegreesToRadians(angle);
    qreal dx = speed * qCos(rad);
    qreal dy = speed * qSin(rad);
    setPos(pos() + QPointF(dx, dy));
}


void AttackStick::keyPressEvent(QKeyEvent *event)
{
    // 处理停止
    if (event->key() == Qt::Key_Enter ||event->key() == Qt::Key_Return)
    {
        if (isMoving) { // 只有移动时响应
            qDebug() << "Enter pressed, stopping AttackStick.";
            isMoving = false; // 停止移动

            // 计算经过时间
            qint64 elapsedTime = movementTimer.isValid() ? movementTimer.elapsed() : 0;
            movementTimer.invalidate(); // 使计时器失效

            qDebug() << "AttackStick stopped. Elapsed time:" << elapsedTime << "ms.";
            emit stopped(elapsedTime); // 发出信号

            event->accept(); // 接受事件
        }
    }
    else
    {
        // 其他按键调用基类处理
        QGraphicsObject::keyPressEvent(event);
    }
}
