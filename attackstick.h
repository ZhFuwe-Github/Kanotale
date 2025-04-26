#ifndef ATTACKSTICK_H
#define ATTACKSTICK_H

#include "bulletitem.h"
#include <QElapsedTimer>
#include <QKeyEvent>

class AttackStick : public BulletItem
{
    Q_OBJECT

public:
    explicit AttackStick(int damage, qreal initialSpeed, qreal initialAngle, const QString &pixmapPath, QGraphicsItem *parent = nullptr);

    // 启动计时
    void startMovingTimer();
    void advance(int phase) override;
    bool getMoving();

signals:
    // 在停止时发出经过的时间
    void stopped(qint64 elapsedTime);

protected:
    // 键盘事件处理
    void keyPressEvent(QKeyEvent *event) override;

private:
    QElapsedTimer movementTimer; // 内部计时器
    bool isMoving = true; // 是否在移动
};

#endif // ATTACKSTICK_H
