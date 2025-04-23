#include "bulletitem.h"
#include <QGraphicsScene>
#include <QtMath>
#include <QPainter>
#include <QDebug>

BulletItem::BulletItem(int damageValue, BulletType bulletType, const QString &pixmapPath, QGraphicsItem *parent)
    : QGraphicsObject(parent), damage(damageValue), m_bulletType(bulletType)
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

void BulletItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(boundingRect().topLeft(), pixmap);
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
    if (checkOutOfBounds()) {
        if (scene()) {
            qDebug() << "Bullet out of bounds, removing.";
            scene()->removeItem(this);
            delete this; // 销毁
        }
    }
}

bool BulletItem::checkOutOfBounds() {
    if (!scene()) return false;

    QRectF sceneRect = scene()->sceneRect();
    QRectF itemRect = mapToScene(boundingRect()).boundingRect();

    qreal margin = 10; // 允许离开边界的距离
    return !sceneRect.adjusted(-margin, -margin, margin, margin).intersects(itemRect);
}

