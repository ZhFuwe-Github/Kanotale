#include "bulletitem.h"
#include <QGraphicsScene>
#include <QtMath>
#include <QPainter>
#include <QDebug>

BulletItem::BulletItem(int damageValue, BulletType bulletType, const QString &pixmapPath, QGraphicsItem *parent)
    : QGraphicsObject(parent), damage(damageValue), m_bulletType(bulletType),pixmap(pixmapPath)
{
    if (!pixmapPath.isEmpty()) {
        pixmap.load(pixmapPath);
        if (pixmap.isNull()) {
            pixmap = QPixmap(10, 10);
            pixmap.fill(Qt::white);
        }
    } else {
        pixmap = QPixmap(10, 10);
        pixmap.fill(Qt::cyan);
    }
}

// 实现 paint 方法
void BulletItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); // 如果没用到 option 参数，标记为未使用
    Q_UNUSED(widget);

    // 在 boundingRect 左上角 (0,0) 绘制 pixmap
    painter->drawPixmap(boundingRect().topLeft(), pixmap);
}

// 通过路径设置图标
void BulletItem::setPixmap(const QString &pixmapPath)
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

QRectF BulletItem::boundingRect() const
{
    // 返回包围图像的矩形
    //return pixmap.rect();
    //return QRectF(0, 0, pixmap.width(), pixmap.height());
    QRectF rect = QRectF(QPointF(0, 0), pixmap.size()); // 基于 pixmap 计算
    return rect;
}

QPainterPath BulletItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void BulletItem::advance(int phase)
{
    if (phase == 0) return; // 只在移动阶段处理

    // 直线移动
    qreal rad = qDegreesToRadians(angle);
    qreal dx = speed * qCos(rad);
    qreal dy = speed * qSin(rad);

    setPos(pos() + QPointF(dx, dy));

    // 检查并处理越界
    //if (checkOutOfBounds()) {
    //    if (scene()) {
    //        qDebug() << "Bullet out of bounds, removing.";
    //        scene()->removeItem(this);
    //        delete this; // 销毁
    //    }
    //}
}

bool BulletItem::checkOutOfBounds() {
    if (!scene()) return false;

    QRectF sceneRect = scene()->sceneRect();
    QRectF itemRect = mapToScene(boundingRect()).boundingRect();

    qreal margin = 10; // 允许离开边界的距离
    return !sceneRect.adjusted(-margin, -margin, margin, margin).intersects(itemRect);
}

