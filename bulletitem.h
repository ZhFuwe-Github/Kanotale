#ifndef BULLETITEM_H
#define BULLETITEM_H

#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPainter>

// 弹幕类型枚举
enum class BulletType {
    Generic,
    Linear,
    Laser,
    Spinning
};

class BulletItem : public QGraphicsObject
{
    Q_OBJECT
    // 自定义伤害值或者其他属性
    Q_PROPERTY(int damage READ getDamage WRITE setDamage)

public:
    // 使用 Type 标识符，方便进行 cast
    enum { Type = UserType + 1 }; // 自定义 Item 类型
    int type() const override { return Type; }

    explicit BulletItem(int damageValue, BulletType bulletType = BulletType::Generic, const QString &pixmapPath = "", QGraphicsItem *parent = nullptr);

    // 公共接口
    int getDamage() const { return damage; }
    bool getRemovable() {return removable;}
    void setDamage(int d) { damage = d; }
    BulletType getBulletType() const { return m_bulletType; }

    // QGraphicsItem 设定
    QRectF boundingRect() const override;
    QPainterPath shape() const override; // 碰撞形状
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override; // 自定义绘制

    // 移动逻辑
    virtual void advance(int phase) override; // 虚函数，允许子类覆盖
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void setPixmap(const QString &pixmapPath);

protected:
    QPixmap pixmap; // 弹幕图像
    int damage;     // 伤害值
    bool removable = true;    // 碰撞后能否移除
    qreal speed = 3.0; // 移动速度
    qreal rotationSpeed = 0.0;
    qreal angle = 0.0; // 移动角度
    BulletType m_bulletType;

    bool checkOutOfBounds(); // 越界检查
};

#endif // BULLETITEM_H
