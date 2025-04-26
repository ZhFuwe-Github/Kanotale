#include "playerheartitem.h"
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem> // 使用 option
#include <QWidget>              // 使用 widget
#include <QDebug>
#include "widget.h"

PlayerHeartItem::PlayerHeartItem(const QString &pixmapPath, QGraphicsItem *parent)
    : QGraphicsObject(parent), pixmap(pixmapPath)
{
    if (pixmap.isNull()) {
        qWarning() << "Failed to load player heart pixmap:" << pixmapPath;
        pixmap = QPixmap(20, 20);
        pixmap.fill(Qt::red);
    }
}

// 实现 paint 方法
void PlayerHeartItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); // 如果没用到 option 参数，标记为未使用
    Q_UNUSED(widget);

    // 在 boundingRect 左上角 (0,0) 绘制 pixmap
    painter->drawPixmap(boundingRect().topLeft(), pixmap);
}

// 通过路径设置图标
void PlayerHeartItem::setPixmap(const QString &pixmapPath)
{
    QPixmap newPixmap(pixmapPath);
    //setPixmap(newPixmap);
    if (pixmap.cacheKey() == newPixmap.cacheKey()) {
        // 如果新旧 Pixmap 相同，无需操作
        return;
    }

    // 在改变 boundingRect 之前，通知场景将要发生变化
    prepareGeometryChange();
    // 更新存储的 pixmap
    pixmap = newPixmap;

    // update() 会标记 boundingRect() 区域为无效，并安排重绘
    update();
}


QRectF PlayerHeartItem::boundingRect() const
{
    return QRectF(0, 0, pixmap.width(), pixmap.height());
}

QPainterPath PlayerHeartItem::shape() const
{
    // 定义碰撞形状
    QPainterPath path;
    qreal padding = 3; // 缩小一些碰撞体积
    path.addRect(boundingRect().adjusted(padding, padding, -padding, -padding));
    return path;
}

void PlayerHeartItem::advance(int phase)
{
    //****************************************************
    //  QGraphicsScene::advance() 会调用两次 advance()
    //  phase = 0: 准备阶段
    //  phase = 1: 移动阶段
    //****************************************************
    if (phase == 0) return; // 只在移动阶段处理
    updatePosition(); // 更新位置
}

void PlayerHeartItem::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        moveUp = true;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        moveDown = true;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        moveLeft = true;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        moveRight = true;
        break;
    default:
        // 其他按键
        QGraphicsObject::keyPressEvent(event);
    }
}

void PlayerHeartItem::keyReleaseEvent(QKeyEvent *event)
{
    /* QT 机制：
     * 在长按某个键不释放的情况下，keyPressEvent和keyReleaseEvent事件会不断被触发
     */

    //检查是否是 auto repeat release
    if (event->isAutoRepeat()) {
        QGraphicsObject::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        moveUp = false;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        moveDown = false;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        moveLeft = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        moveRight = false;
        break;
    default:
        QGraphicsObject::keyReleaseEvent(event);
    }
}

void PlayerHeartItem::updatePosition()
{
    qreal dx = 0;
    qreal dy = 0;

    if (moveUp) dy -= speed;
    if (moveDown) dy += speed;
    if (moveLeft) dx -= speed;
    if (moveRight) dx += speed;

    // --- 边界检测 ---
    if (dx != 0 || dy != 0) {
        QPointF currentPos = pos();
        QPointF nextPos = currentPos + QPointF(dx, dy);

        // 获取 BattleWidget 实例 (需要一种方法访问它)
        // 通过 scene()->parent() 然后 cast (scene 父对象是 BattleWidget)
        BattleWidget* battleWidget = qobject_cast<BattleWidget*>(scene() ? scene()->parent() : nullptr);

        if (battleWidget && scene()) { // 确保能访问 BattleWidget 和 scene
            // 使用 battleRectInScene 检测边界
            QRectF allowedRect = battleWidget->getBattleRectInScene();
            QRectF itemRect = mapToScene(boundingRect()).boundingRect(); // 获取项在场景中的实际矩形

            // 检查水平边界
            if (nextPos.x() < allowedRect.left()) {
                nextPos.setX(allowedRect.left());
            } else if (nextPos.x() + itemRect.width() > allowedRect.right()) {
                nextPos.setX(allowedRect.right() - itemRect.width());
            }

            // 检查垂直边界
            if (nextPos.y() < allowedRect.top()) {
                nextPos.setY(allowedRect.top());
            } else if (nextPos.y() + itemRect.height() > allowedRect.bottom()) {
                nextPos.setY(allowedRect.bottom() - itemRect.height());
            }
        }
            setPos(nextPos);
        }


}
