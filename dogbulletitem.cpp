#include "dogbulletitem.h"
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


DogBulletItem::DogBulletItem(int damage, qreal initialSpeed, qreal initialAngle,
                             const QString &pixmapPath1, const QString &pixmapPath2,
                             QGraphicsItem *parent)
    : LinearBulletItem(true,damage, initialSpeed, initialAngle,"", parent)
{
    bool loaded1 = pixmap1.load(pixmapPath1);
    bool loaded2 = pixmap2.load(pixmapPath2);

    currentPixmap = &pixmap1; // 初始显示第一张图
    setRotation(0);
    this->removable = false;
}

QRectF DogBulletItem::boundingRect() const
{
    return QRectF(QPointF(0,0), currentPixmap->size());
}

void DogBulletItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (currentPixmap) {
        painter->drawPixmap(boundingRect().topLeft(), *currentPixmap);
    }
}

QPainterPath DogBulletItem::shape() const
{
    QPainterPath path;
    // 获取包围盒矩形
    QRectF rect = boundingRect();

    // 添加一个正好内切于包围盒的椭圆
    path.addEllipse(rect);
    // 调整椭圆大小
    qreal paddingX = rect.width() * 0.40; // 水平缩小 35%
    qreal paddingY = rect.height() * 0.5; // 垂直缩小 45%
    QRectF smallerRect = rect.adjusted(paddingX, paddingY, -paddingX, -paddingY);
    path.addEllipse(smallerRect);

    return path; // 返回包含椭圆的路径
}

void DogBulletItem::advance(int phase)
{
    if (phase == 0) return;

    // 调用基类的 advance 处理移动
    LinearBulletItem::advance(phase);

    // 处理动画切换
    frameCounter++;
    if (frameCounter >= switchInterval) {
        frameCounter = 0; // 重置计数器
        // 切换指向的 pixmap
        if (currentPixmap == &pixmap1) {
            currentPixmap = &pixmap2;
        } else {
            currentPixmap = &pixmap1;
        }
        // 请求重绘
        update();
    }
}
