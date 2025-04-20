#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFontDatabase>
#include <QTimer>
#include <QStackedWidget>
#include <QEvent>

class BattleWidget : public QWidget
{
    Q_OBJECT

public:
    BattleWidget(QWidget *parent = nullptr);
    ~BattleWidget();

    //设置
    bool hpfix=0;
    bool sound=1;
    bool l=1;

     void startDialogue(const QString& text);
     void setEnemySprite(const QString& imagePath);
     void setEnemyEffect(const QString& imagePath);
     void startEDialogue(const QString& text);
     void modifyHp(int amount);
     void modifyEnemyHp(int amount);


protected:
     bool eventFilter(QObject *watched, QEvent *event) override;
     void keyPressEvent(QKeyEvent *event) override; //处理键盘触发

private slots:
    void onFightClicked();
    void onActClicked();
    void onItemClicked();
    void onMercyClicked();
    void updateDialogueText();
    void updateEDialogueText();
    void onSettingsClicked();
    void applySettings(bool soundEnabled, bool hpFixEnabled);

private:
    void setupUi();
    void setupStyles();
    void loadCustomFont();
    void loadSmallCustomFont();
    void loadAndSetPixmap(QLabel* label, const QString& path, const QSize& targetSize);
    void updateHpDisplay(); // 更新 HP 显示
    void handlePlayerDeath(); // 处理玩家死亡
    void updateEnemyHpDisplay();// 更新敌人 HP 显示
    void handleEnemyDefeat(); // 处理敌人死亡

    // Main Layouts
    QVBoxLayout *mainLayout;
    QFrame *battleBoxFrame;
    QVBoxLayout *battleBoxLayout;

    // 顶部
    QProgressBar *enemyHpProgressBar; //敌人血条控件
    QLabel *enemySpriteLabel;
    QLabel *enemyDialogueLabel;
    QLabel *enemyEffectLabel;
    QLabel *dialogueLabel;    // 对话窗口

    QStackedWidget *actionStackedWidget; // 放置行动、仁慈、物品菜单、攻击框、战斗框、对话窗口
    QWidget *actMenuPage;
    QWidget *itemMenuPage;
    QWidget *mercyMenuPage;
    QWidget *battlePage; // 战斗弹幕页面
    QWidget *dialoguePage;
    QWidget *attackPage;

    const QSize ENEMY_SPRITE_TARGET_SIZE = QSize(304,376);

        //用于打字机效果
    QTimer *dialogueTimer;        // 定时器
    QTimer *edialogueTimer;
    QString fullDialogueText;     // 完整对话文本
    QString fullEDialogueText;
    int currentCharIndex;         // 当前显示字符的索引
    int currentECharIndex;
    int CHARACTER_INTERVAL_MS = 35; // 每个字符出现的间隔

    // 底部
    QHBoxLayout *bottomAreaLayout;
    QVBoxLayout *playerStatsLayout;
    //QGridLayout *actionButtonsLayout;
    QLabel *playerNameLabel;
    QLabel *playerLevelLabel;
    QHBoxLayout *hpLayout;
    QLabel *hpTextLabel;
    QProgressBar *hpProgressBar;
    QLabel *hpValueLabel;

    //玩家状态
    int playerMaxHp;
    int playerCurrentHp;

    //敌人状态
    int enemyMaxHp;
    int enemyCurrentHp;

    // 行动按钮
    QPushButton *fightButton;
    QPushButton *actButton;
    QPushButton *itemButton;
    QPushButton *mercyButton;

    //字体
    QFont pixelFont;
    QFont smallpixelFont;

};

#endif // WIDGET_H
