#include "widget.h"
#include "settings.h"
#include "playerheartitem.h"
#include "bulletitem.h"
#include "linearbulletitem.h"

#include "qrandom.h"

#include <QPixmap>
#include <QFont>
#include <QDebug>
#include <QKeyEvent>
#include <QObject>
#include <QResizeEvent>
#include <QPointer>
#include <qapplication.h>

// --- 设定 ---
const QString PLAYER_NAME = "小鹿包";
const int PLAYER_LV = 1;
const int INITIAL_PLAYER_MAX_HP = 20;
const int INITIAL_PLAYER_CURRENT_HP = 20;
const QString ENEMY_SPRITE_PATH = "./ktresources/images/kano/k01.png";
const QString FONT_PATH = "./ktresources/fonts/fusion-pixel-12px-monospaced-latin.ttf";
const QString SMALL_FONT_PATH = "./ktresources/fonts/fusion-pixel-8px-monospaced-latin.ttf";
const int FONT_SIZE = 16; // 默认字体大小
const int INITIAL_ENEMY_MAX_HP = 50;
const int INITIAL_ENEMY_CURRENT_HP = 50;


BattleWidget::BattleWidget(QWidget *parent)
    : QWidget(parent)
{
    // --- 初始化玩家状态 ---
    playerMaxHp = INITIAL_PLAYER_MAX_HP;
    playerCurrentHp = INITIAL_PLAYER_CURRENT_HP;
    // --- 初始化敌人状态 ---
    enemyMaxHp = INITIAL_ENEMY_MAX_HP;
    enemyCurrentHp = INITIAL_ENEMY_CURRENT_HP;
    // --- 初始化UI ---
    loadCustomFont();
    loadSmallCustomFont();
    setupUi();
    setupStyles();

    // --- 初始化打字机效果定时器 ---
    dialogueTimer = new QTimer(this);
    dialogueTimer->setInterval(CHARACTER_INTERVAL_MS); // 设置间隔
    connect(dialogueTimer, &QTimer::timeout, this, &BattleWidget::updateDialogueText); // 连接信号槽
    edialogueTimer = new QTimer(this);
    edialogueTimer->setInterval(CHARACTER_INTERVAL_MS);
    connect(edialogueTimer, &QTimer::timeout, this, &BattleWidget::updateEDialogueText);

    // --- 初始对话 ---
    startDialogue("* 鹿乃出现在你面前.");

    // --- 初始化玩家 HP 显示  ---
    playerNameLabel->setText(PLAYER_NAME); // 名字
    playerLevelLabel->setText(QString("LV %1").arg(PLAYER_LV)); // 等级
    hpProgressBar->setMaximum(playerMaxHp); // 最大HP
    updateHpDisplay(); // 更新HP显示

    // --- 初始化敌人 HP 显示 ---
    enemyHpProgressBar->setMaximum(enemyMaxHp);
    updateEnemyHpDisplay();

    //初始化敌人图片
    loadAndSetPixmap(enemySpriteLabel, ENEMY_SPRITE_PATH, ENEMY_SPRITE_TARGET_SIZE);
    enemySpriteLabel->setAlignment(Qt::AlignCenter); // 对齐

    //链接按钮信号与槽
    connect(fightButton, &QPushButton::clicked, this, &BattleWidget::onFightClicked);
    connect(actButton, &QPushButton::clicked, this, &BattleWidget::onActClicked);
    connect(itemButton, &QPushButton::clicked, this, &BattleWidget::onItemClicked);
    connect(mercyButton, &QPushButton::clicked, this, &BattleWidget::onMercyClicked);
}

BattleWidget::~BattleWidget()
{

}

//加载自定义字体，如果失败，使用默认字体
void BattleWidget::loadCustomFont()
{
    int fontId = QFontDatabase::addApplicationFont(FONT_PATH);
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            pixelFont.setFamily(fontFamilies.at(0));
            pixelFont.setPointSize(FONT_SIZE);
            qInfo() << "Successfully loaded font:" << fontFamilies.at(0);
        } else {
            qWarning() << "Warning: Could not retrieve font family name after loading" << FONT_PATH;
            pixelFont = QFont("Monospace", FONT_SIZE);
        }
    } else {
        qWarning() << "Warning: Failed to load font:" << FONT_PATH << ". Using fallback.";
        pixelFont = QFont("Monospace", FONT_SIZE);
        pixelFont.setStyleHint(QFont::TypeWriter);
    }
}
void BattleWidget::loadSmallCustomFont()
{
    int smallfontId = QFontDatabase::addApplicationFont(SMALL_FONT_PATH);
    if (smallfontId != -1) {
        QStringList smallfontFamilies = QFontDatabase::applicationFontFamilies(smallfontId);
        if (!smallfontFamilies.isEmpty()) {
            smallpixelFont.setFamily(smallfontFamilies.at(0));
            smallpixelFont.setPointSize(FONT_SIZE);
            qInfo() << "Successfully loaded font:" << smallfontFamilies.at(0);
        } else {
            qWarning() << "Warning: Could not retrieve font family name after loading" << FONT_PATH;
            smallpixelFont = QFont("Monospace", FONT_SIZE); // Fallback
        }
    } else {
        qWarning() << "Warning: Failed to load font:" << SMALL_FONT_PATH << ". Using fallback.";
        smallpixelFont = QFont("Monospace", FONT_SIZE); // Fallback font
        smallpixelFont.setStyleHint(QFont::TypeWriter);
    }
}


// 设置按钮Enter键确认和自动聚焦
bool BattleWidget::eventFilter(QObject *watched, QEvent *event)
{
    // 尝试将被观察对象转换为 QPushButton
    QPushButton *button = qobject_cast<QPushButton*>(watched);
    // 检查是否转换成功，并且事件是鼠标进入
    if (button && event->type() == QEvent::Enter)
    {
        //设置焦点
        button->setFocus();
    }
    else if (event->type() == QEvent::KeyPress)
    {
        // 将通用事件指针转换为键盘事件指针
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        // 检查按下的键是否是 Enter
        if (keyEvent->key() == Qt::Key_Enter)
        {
            button->animateClick();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

//设置ESC键返回
void BattleWidget::keyPressEvent(QKeyEvent *event)
{
    // 检查按下的键是否是 Esc
    if (event->key() == Qt::Key_Escape)
    {
        // 获取当前正在显示的页面
        QWidget *currentPage = actionStackedWidget->currentWidget();
        // 检查当前页面是否是需要按 ESC 返回的页面
        if (currentPage == actMenuPage ||currentPage == itemMenuPage ||currentPage == mercyMenuPage)
        {
            actionStackedWidget->setCurrentWidget(dialoguePage);
            event->accept();
            return;
        }
    }
    // 则调用基类的 keyPressEvent 来处理其他默认按键行为
    QWidget::keyPressEvent(event);
}

// 定位 gameView
void BattleWidget::positionGameView() {
    if (gameView && isVisible()) { // 只在主窗口可见时定位
        // 获取主窗口当前在全局屏幕上的位置
        QPoint battleWidgetGlobalPos = mapToGlobal(QPoint(0, 0));

        int targetX = battleWidgetGlobalPos.x(); // 与主窗口左侧对齐
        int targetY = battleWidgetGlobalPos.y() + height() -550; // 主窗口底部再向上 600px

        gameView->move(targetX, targetY);
    }
}

// 处理窗口操作
void BattleWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event); // 基类实现
    positionGameView();      // 定位 gameView
}
void BattleWidget::moveEvent(QMoveEvent *event) {
    QWidget::moveEvent(event); // 基类实现
    positionGameView();    // 定位 gameView
}
void BattleWidget::closeEvent(QCloseEvent *event) {
    // 先关闭 gameView
    if (gameView) {
        gameView->close();
    }
    // 接受关闭事件，允许窗口关闭
    event->accept();
    // 阻止关闭 event->ignore();
}

// --- 启动对话的方法 ---
void BattleWidget::startDialogue(const QString& text)
{
    if (dialogueTimer->isActive()) {
        dialogueTimer->stop(); // 如果上一个还没说完，先停掉
    }
    fullDialogueText = text;
    currentCharIndex = 0;
    dialogueLabel->setText("");
    if (!fullDialogueText.isEmpty()) { // 只在文本非空时启动定时器
        dialogueTimer->start(); // 启动定时器开始逐字显示
        updateDialogueText(); // 立刻显示第一个字符
    }
}

// --- 实现对话框定时器触发的槽函数 ---
void BattleWidget::updateDialogueText()
{
    if (currentCharIndex < fullDialogueText.length()) {
        // 截取子串
        dialogueLabel->setText(fullDialogueText.left(currentCharIndex + 1));
        currentCharIndex++;
    } else {
        dialogueTimer->stop();
    }
}

// --- 启动敌人对话的方法 ---
void BattleWidget::startEDialogue(const QString& text)
{
    if (edialogueTimer->isActive()) {
        edialogueTimer->stop(); // 如果上一个还没说完，先停掉
    }
    fullEDialogueText = text;
    currentECharIndex = 0;
    enemyDialogueLabel->setText(""); // 清空标签内容
    if (!fullEDialogueText.isEmpty()) { // 只在文本非空时启动定时器
        edialogueTimer->start(); // 启动定时器开始逐字显示
        updateEDialogueText(); // 立刻显示第一个字符
    }
}

// --- 实现敌人对话框定时器触发的槽函数 ---
void BattleWidget::updateEDialogueText()
{
    if (currentECharIndex < fullEDialogueText.length()) {
        // 截取子串
        enemyDialogueLabel->setText(fullEDialogueText.left(currentECharIndex + 1));
        currentECharIndex++;
        // 播放音效
        // QSoundEffect effect;
        // effect.setSource(QUrl::fromLocalFile(":/sounds/typing_sound.wav"));
        // effect.play();
    } else {
        edialogueTimer->stop(); // 显示完毕停止定时
    }
    QTimer::singleShot(1000, this, [this](){ // 1000ms 后自动清空
        enemyDialogueLabel->setText("");
    });
}

// --- 加载和设置图片 ---
void BattleWidget::loadAndSetPixmap(QLabel* label, const QString& path, const QSize& targetSize)
{
    QPixmap pixmap(path);
    label->setPixmap(pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation));
}

// --- 修改敌人图片 ---
void BattleWidget::setEnemySprite(const QString& imagePath)
{
    loadAndSetPixmap(enemySpriteLabel, imagePath, ENEMY_SPRITE_TARGET_SIZE);
}
// --- 修改敌人特效图片 ---
void BattleWidget::setEnemyEffect(const QString& imagePath)
{
    loadAndSetPixmap(enemyEffectLabel, imagePath, ENEMY_SPRITE_TARGET_SIZE);
}

// --- 修改玩家 HP 的方法 ---
void BattleWidget::modifyHp(int amount)
{
    if(invincible > 0) return;

    int previousHp = playerCurrentHp;
    int newHp = playerCurrentHp + amount;
    newHp = qBound(0, newHp, playerMaxHp);  //检查边界
    playerCurrentHp = newHp;

    updateHpDisplay();

    if (amount < 0) {
        // 扣血效果
    }
    if (playerCurrentHp <= 0 && previousHp > 0) { // 检查是否死亡
        handlePlayerDeath();
    } else if (amount > 0) {
        // 治疗效果
    }

    //设置无敌帧
    invincible = 30;
}

// --- 更新 HP 显示的辅助函数 ---
void BattleWidget::updateHpDisplay()
{
    if (hpProgressBar) {
        hpProgressBar->setValue(playerCurrentHp);
    }
    if (hpValueLabel) {
        hpValueLabel->setText(QString("%1 / %2").arg(playerCurrentHp).arg(playerMaxHp));
    }
}

// --- 处理玩家死亡 ---
void BattleWidget::handlePlayerDeath()
{
    actionStackedWidget->setCurrentWidget(dialoguePage);
    dialogueLabel->setAlignment(Qt::AlignCenter);
    startDialogue("* G A M E   O V E R !");

    fightButton->setEnabled(false);
    actButton->setEnabled(false);
    itemButton->setEnabled(false);
    mercyButton->setEnabled(false);
    fightButton->hide();
    actButton->hide();
    itemButton->hide();
    mercyButton->hide();

    if (enemySpriteLabel) enemySpriteLabel->hide();
    if (enemyHpProgressBar) enemyHpProgressBar->hide();
    if (enemyEffectLabel) enemyEffectLabel->hide();
    if (enemyDialogueLabel) enemyDialogueLabel->hide();
}

// --- 修改敌人 HP ---
void BattleWidget::modifyEnemyHp(int amount)
{
    int previousHp = enemyCurrentHp;
    int newHp = enemyCurrentHp + amount;
    newHp = qBound(0, newHp, enemyMaxHp);

    // --- 更新内部状态 ---
    enemyCurrentHp = newHp;
    updateEnemyHpDisplay();

    if (amount < 0) {
        setEnemySprite("./ktresources/images/kano/k04.png");    //受伤效果
        QTimer::singleShot(500, this, [this](){ // 500ms 后恢复
            setEnemySprite("./ktresources/images/kano/k01.png");
        });
    }

    if (enemyCurrentHp <= 0 && previousHp > 0) { // 检查是否刚刚被打败
        handleEnemyDefeat();
    }
}

// --- 更新敌人 HP 显示 ---
void BattleWidget::updateEnemyHpDisplay()
{
    if (enemyHpProgressBar) {
        enemyHpProgressBar->setValue(enemyCurrentHp);
    }
}

// --- 处理敌人死亡 ---
void BattleWidget::handleEnemyDefeat()
{
    //startDialogue("* 敌人被打败了！\n* 你赢了！");

    if (enemySpriteLabel) enemySpriteLabel->hide();
    if (enemyHpProgressBar) enemyHpProgressBar->hide();

    fightButton->setEnabled(false);
    actButton->setEnabled(false);
    itemButton->setEnabled(false);
    mercyButton->setEnabled(false);

}

//获取边界值用于 PlayerHeart 检测
QRectF BattleWidget::getBattleRectInScene() const
{
    return battleRectInScene;
}

void BattleWidget::setupGameScene() {
    if (!gameView) return;

    gameScene = new QGraphicsScene(this);

    // 设置大范围的场景矩形，原点仍在左上角 (0,0)
    gameScene->setSceneRect(0, 0, FULL_SCENE_WIDTH, FULL_SCENE_HEIGHT);
    gameView->setScene(gameScene);

    // 战斗区域在场景中居中
    qreal battleX = (FULL_SCENE_WIDTH - BATTLE_BOX_WIDTH) / 2.0;
    qreal battleY = (FULL_SCENE_HEIGHT - BATTLE_BOX_HEIGHT) / 2.0; // 或者根据你的布局调整Y值
    battleRectInScene = QRectF(battleX, battleY, BATTLE_BOX_WIDTH, BATTLE_BOX_HEIGHT);

    playerHeart = new PlayerHeartItem("./ktresources/images/icon/heart.png");
    // 使用 battleRectInScene 的中心来定位
    //playerHeart->setPos(battleRectInScene.center());
    qDebug()<<battleRectInScene.center();
    playerHeart->setPos(346,228);
    playerHeart->setZValue(1);
    playerHeart->setFlag(QGraphicsItem::ItemIsFocusable);
    gameScene->addItem(playerHeart);

    // 创建边框
    QGraphicsRectItem *borderItem = new QGraphicsRectItem(battleRectInScene);
    QPen borderPen(Qt::white, 2);
    borderItem->setPen(borderPen);
    borderItem->setBrush(Qt::NoBrush);
    borderItem->setZValue(2);
    gameScene->addItem(borderItem);
    playerHeart->setFocus(); // 放在 addItem 之后

    // 创建游戏循环定时器
    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &BattleWidget::updateGame);
}

// --- 控制游戏循环 ---
void BattleWidget::startGameLoop() {
    if (gameLoopTimer && !gameLoopTimer->isActive()) {
        playerHeart->setFocus(); // 确保红心有焦点
        gameLoopTimer->start(GAME_UPDATE_INTERVAL);
        qDebug() << "Game loop started.";
        fightButton->setEnabled(false);
        actButton->setEnabled(false);
        itemButton->setEnabled(false);
        mercyButton->setEnabled(false);
    }
}

void BattleWidget::stopGameLoop() {
    if (gameLoopTimer && gameLoopTimer->isActive()) {
        gameLoopTimer->stop();
        qDebug() << "Game loop stopped.";
        fightButton->setEnabled(true);
        actButton->setEnabled(true);
        itemButton->setEnabled(true);
        mercyButton->setEnabled(true);
    }
    if (gameView) {
        gameView->hide();
    }
    // 移除场景中的所有弹幕项
    QList<QGraphicsItem*> items = gameScene->items();
    for(QGraphicsItem *item : items) {
        if (qgraphicsitem_cast<BulletItem*>(item)) {
            gameScene->removeItem(item);
            delete item;
        }
    }
}

// --- 游戏更新逻辑 ---
void BattleWidget::updateGame() {
    if (!gameScene || !playerHeart) return;
    //qDebug() <<"running updateGame()";
    // 驱动场景中所有 Item
    gameScene->advance();
    // 调用 PlayerHeart 的移动逻辑
    playerHeart->advance(1);

    // 碰撞检测
    QList<QGraphicsItem*> colliding_items = playerHeart->collidingItems();
    for (QGraphicsItem *item : colliding_items) {
        // 尝试将碰撞项转换为 BulletItem 指针
        BulletItem *bullet = qgraphicsitem_cast<BulletItem*>(item);
        if (bullet) {
            // 玩家扣血
            modifyHp(-bullet->getDamage());
            // 移除弹幕
            gameScene->removeItem(bullet);
            delete bullet; // 删除弹幕对象
            // 播放音效

        }
    }

    // 无敌帧动画
    if(invincible > 0){
        invincible-=1;
        if(invincible%8>=4){playerHeart->setPixmap("./ktresources/images/icon/empty.png");}else{
            playerHeart->setPixmap("./ktresources/images/icon/heart.png");
        }
    }
    playerHeart->setFocus();

    // 定期生成弹幕
}

// 单个弹幕测试
void BattleWidget::spawnBullet() {
    if (!gameScene) {
        qWarning() << "ERROR: gameScene is NULL in spawnBullet!";
        return; // 如果场景无效，直接返回
    }
    //qreal startX = QRandomGenerator::global()->bounded(gameScene->sceneRect().width());
    //qreal startY = gameScene->sceneRect().top(); // 在顶部生成
    qreal angle = 60.0;
    qreal speed = 1.0;
    int damage = 1;
    LinearBulletItem *bullet = new LinearBulletItem(damage, speed, angle, "./ktresources/images/icon/dot.png");
    bullet->setPos(100,100);
    bullet->setZValue(100);
    gameScene->addItem(bullet);
}

void BattleWidget::setupUi()
{
    // --- 主窗口层 ---
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20); // 在周围添加填充

    // --- BattleBox Frame ---
    battleBoxFrame = new QFrame(this);

    battleBoxLayout = new QVBoxLayout(battleBoxFrame);
    battleBoxLayout->setSpacing(10);
    battleBoxLayout->setContentsMargins(15, 15, 15, 15);

    // --- 顶部布局 ---
    enemySpriteLabel = new QLabel(battleBoxFrame);
    enemySpriteLabel->setMinimumSize(300, 370);
    enemySpriteLabel->setAlignment(Qt::AlignTop);

    enemyDialogueLabel = new QLabel(battleBoxFrame);
    enemyDialogueLabel->setWordWrap(true);
    enemyDialogueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    enemyDialogueLabel->setMinimumSize(200, 160);

    enemyEffectLabel = new QLabel(battleBoxFrame);
    enemyEffectLabel->setMinimumSize(200, 370);
    enemyEffectLabel->setAlignment(Qt::AlignTop);

    enemyHpProgressBar = new QProgressBar(battleBoxFrame); //敌人血条
    enemyHpProgressBar->setFormat("%p/100");
    enemyHpProgressBar->setTextVisible(true);
    enemyHpProgressBar->setFixedHeight(20);
    enemyHpProgressBar->setMaximumWidth(200);

    //对话窗口
    dialogueLabel = new QLabel(battleBoxFrame);
    dialogueLabel->setWordWrap(true);       // Allow text to wrap
    dialogueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    dialogueLabel->setMinimumHeight(140);

    // 创建 QStackedWidget
    actionStackedWidget = new QStackedWidget(battleBoxFrame);

    // 创建各个页面及其布局和内容
    actMenuPage = new QWidget(actionStackedWidget);
    QVBoxLayout *actMenuLayout = new QVBoxLayout(actMenuPage);
    QHBoxLayout *actMenuLayout1 = new QHBoxLayout(actMenuPage);
    QHBoxLayout *actMenuLayout2 = new QHBoxLayout(actMenuPage);
    QHBoxLayout *actMenuLayout3 = new QHBoxLayout(actMenuPage);
    actMenuPage->setLayout(actMenuLayout);
    QLabel* actLabel = new QLabel("行动选项:", actMenuPage);
    QPushButton* checkButton = new QPushButton(" * 查看", actMenuPage);
    QPushButton* talkButton = new QPushButton(" * 交谈", actMenuPage);
    QPushButton* askButton = new QPushButton(" * 询问", actMenuPage);
    QPushButton* fondleButton = new QPushButton(" * 抚摸", actMenuPage);
    QPushButton* praiseButton = new QPushButton(" * 赞扬", actMenuPage);
    QPushButton* emptyButton = new QPushButton("", actMenuPage);
    //QPushButton* actBackButton = new QPushButton("返回", actMenuPage);
    actMenuLayout->addWidget(actLabel);
    actMenuLayout->addLayout(actMenuLayout1);
    actMenuLayout->addLayout(actMenuLayout2);
    actMenuLayout->addLayout(actMenuLayout3);
    actMenuLayout1->addWidget(checkButton);
    actMenuLayout1->addWidget(talkButton);
    actMenuLayout2->addWidget(askButton);
    actMenuLayout2->addWidget(fondleButton);
    actMenuLayout3->addWidget(praiseButton);
    actMenuLayout3->addWidget(emptyButton);
    emptyButton->setEnabled(false);
    actMenuLayout->addStretch();
    // --- 应用悬停聚焦功能 ---
    checkButton->installEventFilter(this);   // 安装过滤器
    talkButton->installEventFilter(this);
    askButton->installEventFilter(this);
    fondleButton->installEventFilter(this);
    praiseButton->installEventFilter(this);
    checkButton->setFocusPolicy(Qt::StrongFocus);   // 设置焦点策略
    talkButton->setFocusPolicy(Qt::StrongFocus);
    askButton->setFocusPolicy(Qt::StrongFocus);
    fondleButton->setFocusPolicy(Qt::StrongFocus);
    praiseButton->setFocusPolicy(Qt::StrongFocus);
    //actMenuLayout->addWidget(actBackButton);

    itemMenuPage = new QWidget(actionStackedWidget);
    QVBoxLayout *itemMenuLayout = new QVBoxLayout(itemMenuPage);
    QLabel *itemLabel =new QLabel("物品菜单:", itemMenuPage);
    QHBoxLayout *itemMenuLayout1=new QHBoxLayout(itemMenuPage);
    QVBoxLayout *itemMenuLayoutL = new QVBoxLayout(itemMenuPage);
    QPushButton* itemButton1 = new QPushButton(" * 小鹿包", itemMenuPage);
    QPushButton* itemButton2 = new QPushButton(" * 小鹿包", itemMenuPage);
    QPushButton* itemButton3 = new QPushButton(" * 小鹿包", itemMenuPage);
    QVBoxLayout *itemMenuLayoutR = new QVBoxLayout(itemMenuPage);
    QLabel *infoLabel =new QLabel("* 鲜嫩可口，恢复 10 HP", itemMenuPage);
    QPushButton* confirmButton = new QPushButton("使用", itemMenuPage);
    //confirmButton->setEnabled(false);
    itemMenuLayout->addWidget(itemLabel);
    itemMenuLayout->addLayout(itemMenuLayout1);
    itemMenuLayout1->addLayout(itemMenuLayoutL);
    itemMenuLayout1->addLayout(itemMenuLayoutR);
    itemMenuLayoutL->addWidget(itemButton1);
    itemMenuLayoutL->addWidget(itemButton2);
    itemMenuLayoutL->addWidget(itemButton3);
    itemMenuLayoutR->addWidget(infoLabel);
    itemMenuLayoutR->addWidget(confirmButton);
    itemButton1->installEventFilter(this);
    itemButton2->installEventFilter(this);
    itemButton3->installEventFilter(this);
    itemButton1->setFocusPolicy(Qt::StrongFocus);
    itemButton2->setFocusPolicy(Qt::StrongFocus);
    itemButton3->setFocusPolicy(Qt::StrongFocus);

    mercyMenuPage = new QWidget(actionStackedWidget);
    QLabel *enemyName =new QLabel("鹿乃：",mercyMenuPage);
    QVBoxLayout *mercyMenuLayout = new QVBoxLayout(mercyMenuPage);
    QPushButton* mercyButton1 = new QPushButton(" * 仁慈", mercyMenuPage);
    QPushButton* escapeButton = new QPushButton(" * 逃跑", mercyMenuPage);
    mercyMenuLayout->addWidget(enemyName);
    mercyMenuLayout->addWidget(mercyButton1);
    mercyMenuLayout->addWidget(escapeButton);
    mercyMenuLayout->addStretch();
    mercyButton1->installEventFilter(this);
    escapeButton->installEventFilter(this);
    mercyButton1->setFocusPolicy(Qt::StrongFocus);
    escapeButton->setFocusPolicy(Qt::StrongFocus);

    battlePage = new QWidget(actionStackedWidget);  // 不再使用
    // 清空 battlePage 的布局
    //QLayout *oldLayout = battlePage->layout();
    //if (oldLayout) {
    //    QLayoutItem *item;
    //    while ((item = oldLayout->takeAt(0)) != nullptr) {
    //        delete item->widget(); // 删除控件
    //        delete item;          // 删除布局项
    //    }
    //    delete oldLayout;         // 删除旧布局
    //}

    // 创建 gameView，设置父窗口为 BattleWidget
    gameView = new QGraphicsView(this); // BattleWidget 作为父窗口

    gameView->setObjectName("gameView_TopLevel");
    gameView->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    gameView->setAttribute(Qt::WA_TranslucentBackground);
    gameView->setStyleSheet("background: transparent; border: 2px solid cyan;");
    gameView->setFixedSize(820, 600); // 设置固定大小
    gameView->hide();

    // 设置游戏场景
    setupGameScene();



    attackPage = new QWidget(actionStackedWidget);
    QLabel* attackPlaceholder = new QLabel("攻击将在这里进行", attackPage);
    QVBoxLayout* attacklayout = new QVBoxLayout(attackPage);
    attacklayout->addWidget(attackPlaceholder, 0, Qt::AlignCenter);

    dialoguePage = new QWidget(actionStackedWidget);
    QVBoxLayout *dialogueLayout = new QVBoxLayout(dialoguePage); // 页面内部布局
    dialoguePage->setLayout(dialogueLayout);
    dialogueLayout->addWidget(dialogueLabel);


    // 将页面添加到 QStackedWidget
    actionStackedWidget->addWidget(actMenuPage);      // index 0
    actionStackedWidget->addWidget(itemMenuPage);     // index 1
    actionStackedWidget->addWidget(mercyMenuPage);    // index 2
    actionStackedWidget->addWidget(battlePage);       // index 3
    actionStackedWidget->addWidget(attackPage);       // index 4
    actionStackedWidget->addWidget(dialoguePage);     // index 5

    // 设置初始状态
    actionStackedWidget->setCurrentWidget(dialoguePage);

    battleBoxLayout->addWidget(enemySpriteLabel, 1);
    //dialoguePage->addWidget(dialogueLabel, 0);


    QVBoxLayout* topSectionLayout = new QVBoxLayout();
    topSectionLayout->addWidget(enemyHpProgressBar, 0, Qt::AlignCenter);
    //topSectionLayout->addWidget(enemySpriteLabel, 1, Qt::AlignCenter); // 图片居中
        QHBoxLayout *enemyAreaLayout = new QHBoxLayout();
        enemyAreaLayout->addWidget(enemyEffectLabel,1);
        enemyAreaLayout->addWidget(enemySpriteLabel, 1, Qt::AlignCenter);
        enemyAreaLayout->addWidget(enemyDialogueLabel, 1);
        enemyAreaLayout->setSpacing(10);
        topSectionLayout->addLayout(enemyAreaLayout);
    //topSectionLayout->addWidget(dialogueLabel, 0);
    topSectionLayout->addWidget(actionStackedWidget, 0);

    topSectionLayout->setSpacing(10); // 调整间距
    battleBoxLayout->addLayout(topSectionLayout);

    QVBoxLayout *bottomAreaLayout = new QVBoxLayout(); // 改为 QVBoxLayout
    bottomAreaLayout->setSpacing(10);

    // --- 状态组件 ---
    QHBoxLayout *statsRowLayout = new QHBoxLayout(); // 水平布局放状态
    statsRowLayout->setSpacing(10); // 设置状态项之间的间距

    // 创建状态组件
    playerNameLabel = new QLabel(battleBoxFrame);
    playerLevelLabel = new QLabel(battleBoxFrame);
    hpTextLabel = new QLabel("HP", battleBoxFrame);
    hpProgressBar = new QProgressBar(battleBoxFrame);
    hpProgressBar->setTextVisible(false);
    hpValueLabel = new QLabel(battleBoxFrame);

        // 将状态组件添加到水平布局中
    statsRowLayout->addWidget(playerNameLabel);
    statsRowLayout->addWidget(playerLevelLabel);
    statsRowLayout->addStretch(1);
    statsRowLayout->addWidget(hpTextLabel);
    statsRowLayout->addWidget(hpProgressBar, 2);
    statsRowLayout->addWidget(hpValueLabel);

    // --- 按钮组件 ---
    QHBoxLayout *buttonRowLayout = new QHBoxLayout();
    buttonRowLayout->setSpacing(15); // 按钮间距

        // 创建按钮组件
    fightButton = new QPushButton("战斗", battleBoxFrame);
    actButton = new QPushButton("行动", battleBoxFrame);
    itemButton = new QPushButton("物品", battleBoxFrame);
    mercyButton = new QPushButton("仁慈", battleBoxFrame);

        // 添加事件过滤器
    fightButton->installEventFilter(this);
    actButton->installEventFilter(this);
    itemButton->installEventFilter(this);
    mercyButton->installEventFilter(this);
    fightButton->setFocusPolicy(Qt::StrongFocus);
    actButton->setFocusPolicy(Qt::StrongFocus);
    itemButton->setFocusPolicy(Qt::StrongFocus);
    mercyButton->setFocusPolicy(Qt::StrongFocus);

        // 设置按钮大小保持一致
    QSize buttonSize(160, 60);
    fightButton->setFixedSize(buttonSize);
    actButton->setFixedSize(buttonSize);
    itemButton->setFixedSize(buttonSize);
    mercyButton->setFixedSize(buttonSize);

        // 将按钮添加到水平布局
    buttonRowLayout->addStretch(1);
    buttonRowLayout->addWidget(fightButton);
    buttonRowLayout->addWidget(actButton);
    buttonRowLayout->addWidget(itemButton);
    buttonRowLayout->addWidget(mercyButton);
    buttonRowLayout->addStretch(1);

        //设置按钮图标
    fightButton->setIconSize(QSize(32, 32));
    actButton->setIconSize(QSize(32, 32));
    itemButton->setIconSize(QSize(32, 32));
    mercyButton->setIconSize(QSize(32, 32));
    fightButton->setIcon(QIcon("./ktresources/images/icon/fight.png"));
    actButton->setIcon(QIcon("./ktresources/images/icon/act.png"));
    itemButton->setIcon(QIcon("./ktresources/images/icon/item.png"));
    mercyButton->setIcon(QIcon("./ktresources/images/icon/mercy.png"));

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    settingsButton =new QPushButton("设置",battleBoxFrame);
    connect(settingsButton, &QPushButton::clicked, this, &BattleWidget::onSettingsClicked);
    QLabel *gameInfo = new QLabel("Kanotale beta0.2",battleBoxFrame);
    bottomLayout->addWidget(gameInfo);
    bottomLayout->addStretch();
    bottomLayout->addWidget(settingsButton);

    //底部布局
    bottomAreaLayout->addLayout(statsRowLayout);
    bottomAreaLayout->addLayout(buttonRowLayout);

    battleBoxLayout->addLayout(bottomAreaLayout);
    mainLayout->addWidget(battleBoxFrame);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);


    QWidget::setFont(pixelFont);
    playerNameLabel->setFont(smallpixelFont);
    playerLevelLabel->setFont(smallpixelFont);
    hpTextLabel->setFont(smallpixelFont);
    hpValueLabel->setFont(smallpixelFont);
    fightButton->setFont(pixelFont);
    actButton->setFont(pixelFont);
    itemButton->setFont(pixelFont);
    mercyButton->setFont(pixelFont);
    enemyHpProgressBar->setFont(pixelFont);
    enemyDialogueLabel->setFont(pixelFont);
    dialogueLabel->setFont(pixelFont);
    gameInfo->setFont(pixelFont);
    settingsButton->setFont(pixelFont);
    //actMenuPage->setFont(pixelFont);

}

void BattleWidget::setupStyles()
{
    this->setStyleSheet(
        "QWidget {"
        "   background-color: black;"
        "   color: white;"
        "}"
        );

    enemyHpProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 0px solid white;"
        "    background-color: red;"
        "    height: 15px;"
        "    text-align: center;"
        "    min-width: 160px;"
        "    font-size: 16px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: green;"
        "    margin: 0px;"
        "}"
        );

    enemyDialogueLabel->setStyleSheet(QString(
    "QLabel {"
    "    font-family: \"%1\";"
    "    font-size: 24px;"
    "}"
    ).arg(pixelFont.family()));

    dialogueLabel->setStyleSheet(QString(
    "QLabel {"
    "    font-family: \"%1\";"
    "    font-size: 28px;"
    "}"
    ).arg(pixelFont.family()));

    battleBoxFrame->setStyleSheet(
        "QFrame {"
        "   background-color: black;"
        "   border: 2px solid white;"
        "}"
        "QLabel {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        );


    // --- 血条样式 ---
    hpProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid white;"
        "    background-color: black;"
        "    height: 15px;"
        "    text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: yellow;"
        "    margin: 1px;"
        "}"
        );

    // --- 按钮样式 ---
    QString buttonStyle = QString(
        "QPushButton {"
        "    font-size: 32px;"
        "    background-color: black;"
        "    border: 4px solid orange;"
        "    color: white;"
        "    padding: 5px;"
        "    min-width: 160px;"
        "}"
        "QPushButton:hover {" // 悬停指示
        "    background-color: #333333;"
        "    icon: url(./ktresources/images/icon/heart.png);"
        "}"
        "QPushButton:pressed {" // 按下指示
        "    background-color: #555555;"
        "    border: 4px solid red;"
        "}"
        "QPushButton:focus {" // 键盘焦点指示
        "    border: 4px solid yellow;"
        "    outline: none;" // 禁用默认焦点矩形
        "    icon: url(./ktresources/images/icon/heart.png);"
        "}"
        );

    // --- 菜单/内页样式 ---
    QString StyleInside = QString(
        "QWidget { font-family: \"%1\"; font-size: %2px; }"
        "QPushButton {"
        "    font-size: 24px;"
        "    background-color: #000000;"
        "    color: white;"
        "    padding: 5px;"
        "    min-width: 160px;"
        "    max-width: 320px;" // 最大宽度
        "    outline: none;"
        "}"
        "QPushButton:focus {"
        "    color: yellow;"
        "    icon: url(./ktresources/images/icon/heart.png);"
        "    outline: none;"
        "}"
        ).arg(pixelFont.family()).arg(pixelFont.pointSize());


    fightButton->setStyleSheet(buttonStyle);
    actButton->setStyleSheet(buttonStyle);
    itemButton->setStyleSheet(buttonStyle);
    mercyButton->setStyleSheet(buttonStyle);

    actMenuPage->setStyleSheet(StyleInside);
    itemMenuPage->setStyleSheet(StyleInside);
    mercyMenuPage->setStyleSheet(StyleInside);

    // --- 单独设置按钮字体大小 ---
    QFont buttonFont = fightButton->font();
    buttonFont.setPointSize(48);

    // 使按钮可通过键盘/Tab 键聚焦
    fightButton->setFocusPolicy(Qt::StrongFocus);
    actButton->setFocusPolicy(Qt::StrongFocus);
    itemButton->setFocusPolicy(Qt::StrongFocus);
    mercyButton->setFocusPolicy(Qt::StrongFocus);

}


// --- 动作按钮槽 ---
void BattleWidget::onFightClicked()
{
    startDialogue("* 你选择了 战斗。");
    //actionStackedWidget->setCurrentWidget(attackPage);
    actionStackedWidget->setCurrentWidget(battlePage);
    actionStackedWidget->show(); // 确保 StackedWidget 可见
    //modifyEnemyHp(-3);

    if (gameView) {
        positionGameView(); // 定位
        gameView->show();   // 显示
        gameView->raise();  // 提升到顶层
        gameView->activateWindow();
        QTimer::singleShot(0, this, [this](){ // 延迟 0ms 意味着在下一个事件循环设置
                playerHeart->setFocus();
        });
        startGameLoop();
        spawnBullet();
    }
}

void BattleWidget::onActClicked()
{
    startEDialogue("* 不要白费力气。");
    actionStackedWidget->setCurrentWidget(actMenuPage);
    setEnemySprite("./ktresources/images/kano/k03.png");
    actionStackedWidget->show();
    modifyHp(-2);
    // ... ...
}

void BattleWidget::onItemClicked()
{
    actionStackedWidget->setCurrentWidget(itemMenuPage);
    actionStackedWidget->show();
    setEnemySprite("./ktresources/images/kano/k02.png");
    // ... ...
}

void BattleWidget::onMercyClicked()
{
    actionStackedWidget->setCurrentWidget(mercyMenuPage);
    actionStackedWidget->show();
    // ... ...
}

void BattleWidget::onSettingsClicked()
{
    Settings *settingsWindow = new Settings(this);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    //加载当前设置
    settingsWindow->setSoundEnabled(this->sound);
    settingsWindow->setHPFixEnabled(this->hpfix);
    //链接信号与槽以应用新设置
    connect(settingsWindow, &Settings::settingsConfirmed,this, &BattleWidget::applySettings);
    settingsButton->setEnabled(false);
    settingsWindow->show();
}

void BattleWidget::applySettings(bool soundEnabled, bool hpFixEnabled)
{
    sound = soundEnabled;
    hpfix = hpFixEnabled;
    settingsButton->setEnabled(true);
}



