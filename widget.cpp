#include "widget.h"
#include "settings.h"
#include "playerheartitem.h"
#include "bulletitem.h"
#include "linearbulletitem.h"
#include "homingbulletitem.h"
#include "fixedbulletitem.h"
#include "warningitem.h"
#include "attackstick.h"
#include "dogbulletitem.h"

#include "qrandom.h"

#include <QThread>
#include <QPixmap>
#include <QFont>
#include <QDebug>
#include <QKeyEvent>
#include <QObject>
#include <QResizeEvent>
#include <QPointer>
#include <QtMath>
#include <QApplication>
#include <QSoundEffect> // 用于音效
#include <QMediaPlayer> // 用于音乐/长音频
#include <QAudioOutput>
#include <QUrl>          // 指定音乐文件路径
#include <QDebug>
#include <QDir>
#include <qapplication.h>

// --- 设定 ---
const QString PLAYER_NAME = "小鹿包";
const int PLAYER_LV = 1;
const int INITIAL_PLAYER_MAX_HP = 20;
const int INITIAL_PLAYER_CURRENT_HP = 20;
QString ENEMY_SPRITE_PATH = "./ktresources/images/kano/k01.png";
const QString FONT_PATH = "./ktresources/fonts/fusion-pixel-12px-monospaced-latin.ttf";
const QString SMALL_FONT_PATH = "./ktresources/fonts/fusion-pixel-8px-monospaced-latin.ttf";
const int FONT_SIZE = 16; // 默认字体大小
const int INITIAL_ENEMY_MAX_HP = 500;
const int INITIAL_ENEMY_CURRENT_HP = 500;


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
    // --- 设置背景音乐 ---
    setupBackgroundMusic();

    // --- 初始化打字机效果定时器 ---
    dialogueTimer = new QTimer(this);
    dialogueTimer->setInterval(CHARACTER_INTERVAL_MS); // 设置间隔
    connect(dialogueTimer, &QTimer::timeout, this, &BattleWidget::updateDialogueText); // 连接信号槽
    edialogueTimer = new QTimer(this);
    edialogueTimer->setInterval(CHARACTER_INTERVAL_MS);
    connect(edialogueTimer, &QTimer::timeout, this, &BattleWidget::updateEDialogueText);

    // --- 初始化玩家 HP 显示  ---
    playerNameLabel->setText(PLAYER_NAME); // 名字
    playerLevelLabel->setText(QString("LV %1").arg(PLAYER_LV)); // 等级
    hpProgressBar->setMaximum(playerMaxHp); // 最大HP
    updateHpDisplay(); // 更新HP显示

    // --- 初始对话 ---
    dialogueLabel->setText("* Loading......");
    // --- 初始化敌人 HP 显示 ---
    enemyHpProgressBar->setMaximum(enemyMaxHp);
    updateEnemyHpDisplay();
    enemyHpProgressBar->hide();
    //初始化敌人图片
    loadAndSetPixmap(enemySpriteLabel, "./ktresources/images/kano/k00.png", ENEMY_SPRITE_TARGET_SIZE);
    enemySpriteLabel->setAlignment(Qt::AlignCenter); // 对齐

    //链接按钮信号与槽
    connect(fightButton, &QPushButton::clicked, this, &BattleWidget::onFightClicked);
    connect(actButton, &QPushButton::clicked, this, &BattleWidget::onActClicked);
    connect(itemButton, &QPushButton::clicked, this, &BattleWidget::onItemClicked);
    connect(mercyButton, &QPushButton::clicked, this, &BattleWidget::onMercyClicked);

    //加载音乐音效
    warningSound = new QSoundEffect(this);
    warningSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/warning.wav"));
    warningSound->setVolume(1.0);
    laserSound = new QSoundEffect(this);
    laserSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/laser.wav"));
    laserSound->setVolume(1.0);
    selectSound = new QSoundEffect(this);
    selectSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/select.wav"));
    selectSound->setVolume(1.0);
    hurtSound = new QSoundEffect(this);
    hurtSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/hurt.wav"));
    hurtSound->setVolume(1.0);
    healSound = new QSoundEffect(this);
    healSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/heal.wav"));
    healSound->setVolume(1.0);
    getitemSound = new QSoundEffect(this);
    getitemSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/item.wav"));
    getitemSound->setVolume(1.0);
    gameoverSound = new QSoundEffect(this);
    gameoverSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/gameover.wav"));
    gameoverSound->setVolume(1.0);
    edialogueSound = new QSoundEffect(this);
    edialogueSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/edialogue.wav"));
    edialogueSound->setVolume(1.0);
    attackSound = new QSoundEffect(this);
    attackSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/attack.wav"));
    attackSound->setVolume(1.0);
    beatSound = new QSoundEffect(this);
    beatSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/beat.wav"));
    beatSound->setVolume(1.0);
    fistSound = new QSoundEffect(this);
    fistSound->setSource(QUrl::fromLocalFile("./ktresources/sounds/fist.wav"));
    fistSound->setVolume(1.0);

    //回合开始
    fightButton->setEnabled(false);
    actButton->setEnabled(false);
    itemButton->setEnabled(false);
    mercyButton->setEnabled(false);
    QTimer::singleShot(5000, this, &BattleWidget::startRound);
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

void BattleWidget::setupBackgroundMusic()
{
    backgroundMusicPlayer = new QMediaPlayer(this); // 创建播放器

    // Qt6需要指定音频输出
    audioOutput = new QAudioOutput(this);
    backgroundMusicPlayer->setAudioOutput(audioOutput);

    // 设置音乐文件
    QString musicPath = "./ktresources/BGM.wav";
    qDebug() << "Attempting to load background music from:" << QDir::currentPath() + "/" + musicPath;
    backgroundMusicPlayer->setSource(QUrl::fromLocalFile(musicPath));

    // 循环播放
    backgroundMusicPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(1.0); // 设置音量
}

// --- 控制播放 ---
void BattleWidget::playBackgroundMusic() {
    if (backgroundMusicPlayer && backgroundMusicPlayer->mediaStatus() != QMediaPlayer::NoMedia) {
        if (backgroundMusicPlayer->playbackState() != QMediaPlayer::PlayingState) {
            qDebug() << "Playing background music...";
            backgroundMusicPlayer->play();
        }
    } else {
        qWarning() << "Cannot play background music: Player not ready or no media loaded.";
    }
}
        //backgroundMusicPlayer->stop();
        //backgroundMusicPlayer->pause();


// 设置按钮Enter键确认和自动聚焦
bool BattleWidget::eventFilter(QObject *watched, QEvent *event)
{
    // 处理鼠标悬停自动聚焦
    QPushButton *button = qobject_cast<QPushButton*>(watched);
    if (button && event->type() == QEvent::Enter) {
        bool gameViewHasFocus = false;
        if (gameScene && gameScene->focusItem()) {
            gameViewHasFocus = true;
        }
        if (!gameViewHasFocus) {
            button->setFocus();
        }
    }
    // 处理全局 Enter
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            // 检查当前拥有焦点的 Widget
            QWidget *focused = QApplication::focusWidget();
            // 尝试将焦点控件转换为 QPushButton
            QPushButton *focusedButton = qobject_cast<QPushButton*>(focused);
            // 检查焦点按钮是否是 BattleWidget 的子控件，且不在 QGraphicsView
            bool gameViewHasFocus = gameScene && gameScene->focusItem();
            if (focusedButton && focusedButton->window() == this->window() && !gameViewHasFocus)
            {
                qDebug() << "Enter pressed, activating focused button:" << focusedButton->text();
                focusedButton->animateClick(); // 模拟点击
                return true;
            }
        }
    }
    // 其他事件基类处理
    return QWidget::eventFilter(watched, event);
}

//设置ESC键返回
void BattleWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "BattleWidget focused item:" << (gameScene ? gameScene->focusItem() : nullptr);
    // 检查按下的键是否是 Esc
    if (event->key() == Qt::Key_Escape)
    {
        // 获取当前正在显示的页面
        QWidget *currentPage = actionStackedWidget->currentWidget();
        // 检查当前页面是否是需要按 ESC 返回的页面
        if (currentPage == actMenuPage ||currentPage == itemMenuPage ||currentPage == mercyMenuPage)
        {
            actionStackedWidget->setCurrentWidget(dialoguePage);
            fightButton->setFocus();
            event->accept();
            return;
        }
    }
    // 调用基类处理其他默认行为
    QWidget::keyPressEvent(event);
}

// 定位 gameView
void BattleWidget::positionGameView() {
    if (gameView && isVisible()) { // 只在主窗口可见时定位
        // 获取主窗口当前在全局屏幕上的位置
        QPoint battleWidgetGlobalPos = mapToGlobal(QPoint(0, 0));

        int targetX = battleWidgetGlobalPos.x(); // 与主窗口左侧对齐
        int targetY = battleWidgetGlobalPos.y() + height() -600; // 主窗口底部再向上 600px

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
        edialogueSound->play();
    } else {
        edialogueTimer->stop(); // 显示完毕停止定时
    }
    QTimer::singleShot(2000, this, [this](){ // 2000ms 后自动清空
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

// --- 修改玩家 HP  ---
void BattleWidget::modifyHp(int amount)
{
    if(amount == 0)return;

    int previousHp = playerCurrentHp;
    int newHp = playerCurrentHp + amount;
    newHp = qBound(0, newHp, playerMaxHp);  //检查边界

    if(hpfix == false)playerCurrentHp = newHp;

    updateHpDisplay();

    if (amount < 0) {
        hurtSound->play();
    }
    if (playerCurrentHp <= 0 && previousHp > 0) { // 检查是否死亡
        handlePlayerDeath();
    } else if (amount > 0) {
        healSound->play();
    }

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
    playerHeart->setPixmap("./ktresources/images/icon/dheart.png");
    gameoverSound->play();
    //stopGameLoop();

    fightButton->setEnabled(false);
    actButton->setEnabled(false);
    itemButton->setEnabled(false);
    mercyButton->setEnabled(false);
    fightButton->hide();
    actButton->hide();
    itemButton->hide();
    mercyButton->hide();

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

    if (enemySpriteLabel) enemySpriteLabel->hide();
    if (enemyHpProgressBar) enemyHpProgressBar->hide();
    if (enemyEffectLabel) enemyEffectLabel->hide();
    if (enemyDialogueLabel) enemyDialogueLabel->hide();

    // 移除场景中的所有弹幕项
    QList<QGraphicsItem*> items = gameScene->items();
    for(QGraphicsItem *item : items) {
        if (qgraphicsitem_cast<BulletItem*>(item)) {
            gameScene->removeItem(item);
            delete item;
        }
    }
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
        beatSound->play();
        QTimer::singleShot(1500, this, [this](){ // 1000ms 后恢复
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

    // 移除场景中的所有弹幕项
    QList<QGraphicsItem*> items = gameScene->items();
    for(QGraphicsItem *item : items) {
        if (qgraphicsitem_cast<BulletItem*>(item)) {
            gameScene->removeItem(item);
            delete item;
        }
    }

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

    // 设置大范围的场景矩形，原点在(0,0)
    gameScene->setSceneRect(0, 0, FULL_SCENE_WIDTH, FULL_SCENE_HEIGHT);
    gameView->setScene(gameScene);

    // 战斗区域在场景中居中
    qreal battleX = (FULL_SCENE_WIDTH - BATTLE_BOX_WIDTH) / 2.0;
    qreal battleY = (FULL_SCENE_HEIGHT - BATTLE_BOX_HEIGHT + 50) / 2.0;
    battleRectInScene = QRectF(battleX, battleY, BATTLE_BOX_WIDTH, BATTLE_BOX_HEIGHT);

    playerHeart = new PlayerHeartItem("./ktresources/images/icon/heart.png");
    // playerHeart->setPos(battleRectInScene.center());
    qDebug()<<battleRectInScene.center();
    playerHeart->setPos(346,252);
    playerHeart->setZValue(1);
    playerHeart->setFlag(QGraphicsItem::ItemIsFocusable);
    gameScene->addItem(playerHeart);

    // 创建边框
    borderItem = new QGraphicsRectItem(battleRectInScene);
    QPen borderPen(Qt::white, 2);
    borderItem->setPen(borderPen);
    borderItem->setBrush(Qt::NoBrush);
    borderItem->setZValue(2);
    gameScene->addItem(borderItem);
    //playerHeart->setFocus();

    // 创建游戏循环定时器
    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &BattleWidget::updateGame);

    batchSpawnTimer = new QTimer(this);
    connect(batchSpawnTimer, &QTimer::timeout, this, &BattleWidget::handleBatchSpawn);

    batchSpawnTimerW = new QTimer(this);
    connect(batchSpawnTimerW, &QTimer::timeout, this, &BattleWidget::handleBatchSpawnW);

    batchSpawnTimerC = new QTimer(this);
    connect(batchSpawnTimerC, &QTimer::timeout, this, &BattleWidget::handleBatchSpawnC);
}

// --- 控制游戏循环 ---
void BattleWidget::startGameLoop() {
    if (gameLoopTimer && !gameLoopTimer->isActive()) {
        //playerHeart->setFocus(); // 确保红心有焦点
        gameLoopTimer->start(GAME_UPDATE_INTERVAL);
        qDebug() << "Game loop started.";
        fightButton->setEnabled(false);
        actButton->setEnabled(false);
        itemButton->setEnabled(false);
        mercyButton->setEnabled(false);
        battleBoxFrame->setStyleSheet(
            "QFrame {"
            "   background-color: black;"
            "   border: 2px solid black;"
            "}"
            );
    }
}

void BattleWidget::stopGameLoop() {

    // 停止定时器
    if (gameLoopTimer && gameLoopTimer->isActive()) {
        gameLoopTimer->stop();
    }
    if (batchSpawnTimer && batchSpawnTimer->isActive()) {
        batchSpawnTimer->stop();
    }
    if (batchSpawnTimerW && batchSpawnTimerW->isActive()) {
        batchSpawnTimerW->stop();
    }
    if (batchSpawnTimerC && batchSpawnTimerC->isActive()) {
        batchSpawnTimerC->stop();
    }

    // 恢复按钮状态
    fightButton->setEnabled(true);
    actButton->setEnabled(true);
    itemButton->setEnabled(true);
    mercyButton->setEnabled(true);

    // 隐藏并清理视图对场景的引用
    if (gameView) {
        gameView->hide();
        // 在删除场景前，将视图与场景解绑
        gameView->setScene(nullptr);
    }

    // 删除场景对象
    if (gameScene) {
        // 删除 QGraphicsScene 对象。
        // Qt 会尝试删除添加到此场景中且没有其他 QObject 父对象的 QGraphicsItems。
        delete gameScene;
        // 置空场景指针
        gameScene = nullptr;
    } else {
        qWarning() << "gameScene was already null.";
    }

    // 将所有指向场景内物体的成员变量指针置空
    // 它们所指向的对象已被删除
    if (playerHeart) {
        playerHeart = nullptr;
    }
    if (currentAttackStickPtr) {
        currentAttackStickPtr = nullptr;
    }

    // 重置游戏状态变量
    invincible = 0;

    // 恢复界面样式
    if (battleBoxFrame) {
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
    }

    qDebug() << "stopGameLoop finished executing.";
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
            // 移除弹幕
            if(invincible <= 0 && bullet->getDamage() != 0){
                modifyHp(-bullet->getDamage());
                if(bullet->getRemovable()){
                    gameScene->removeItem(bullet);
                    delete bullet; // 删除弹幕对象
                }
                //设置无敌帧
                if(playerCurrentHp>0){
                    invincible = 120;
                }
            }
        }
    }

    //qDebug() << invincible;
    // 无敌帧动画
    if(invincible > 0){
        invincible-=1;
        if(invincible%12>=6){playerHeart->setPixmap("");}else{
            playerHeart->setPixmap("./ktresources/images/icon/heart.png");
        }
    }


    if(playerHeart->isVisible()){
       playerHeart->setFocus();
    }else{
        currentAttackStickPtr->setFocus();
    }
}

void BattleWidget::startAttack(){
    if (!gameScene) {return; }

    // 清理旧指针
    QPointer<AttackStick> safeOldPtr = currentAttackStickPtr;
    if (!safeOldPtr.isNull()) {
        if(safeOldPtr->scene()) gameScene->removeItem(safeOldPtr);
        delete safeOldPtr;
    }
    currentAttackStickPtr = nullptr; // 置空

    // 创建新实例
    QPointF startPos(-10, 212);
    AttackStick *stick = new AttackStick(0, 10, 0, "./ktresources/images/attack/stick.png");
    if (!stick) return;

    // 存储普通指针
    currentAttackStickPtr = stick;

    stick->setPos(startPos);
    stick->setZValue(100);
    gameScene->addItem(stick);

    // 连接信号槽
    connect(stick, &AttackStick::stopped, this, &BattleWidget::onAttackStickStopped);

    // UI
    playerHeart->setVisible(false);
    borderItem->setVisible(false);
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
    fightButton->setEnabled(false);
    actButton->setEnabled(false);
    itemButton->setEnabled(false);
    mercyButton->setEnabled(false);

    attackSound->play();

    stick->startMovingTimer();
    stick->setFocus();
    qDebug() << "AttackStick spawned and started. Pointer:" << currentAttackStickPtr;
    QTimer::singleShot(1170, this, [this,stick](){
        if(stick->getMoving())
            stick->setVisible(false);
    });
    //QTimer::singleShot(3000, this, [this](){ stopGameLoop();startRound();});
}

void BattleWidget::onAttackStickStopped(qint64 elapsedTime) {
    QPointer<AttackStick> safePtr = currentAttackStickPtr;
    if (!safePtr.isNull()) {
        this->lastAttackDuration = elapsedTime;
    }
    int damage=-elapsedTime*(elapsedTime-1170)/10000;
    modifyEnemyHp(-damage);
}

// 弹幕雨
void BattleWidget::spawnBullet() {

    int numberOfBullets = 15; // 一次生成弹幕数

    for (int i = 0; i < numberOfBullets; ++i) {
        // 随机生成位置
        // 随机选择 X 坐标
        qreal startX = QRandomGenerator::global()->bounded(gameScene->sceneRect().width());
        // 在场景顶部选择 Y 坐标
        qreal startY = gameScene->sceneRect().top() - QRandomGenerator::global()->bounded(10.0); // 在顶部 0 到 -10 之间随机

        QPointF startPos(startX, startY);
        qDebug() << "Calculated startPos for bullet" << i << ":" << startPos;

        // 设置弹幕属性
        qreal lowestAngle = 80.0;
        qreal highestAngle = 100.0;
        double randomFactor = QRandomGenerator::global()->generateDouble();
        qreal angle = lowestAngle + randomFactor * (highestAngle - lowestAngle);//随机角度

        qreal lowestSpeed = 2.0;
        qreal highestSpeed = 5.0;
        randomFactor = QRandomGenerator::global()->generateDouble();
        qreal speed = lowestSpeed + randomFactor * (highestSpeed - lowestSpeed);//随机速度

        int damage = 1;
        QString pixmapPath = "./ktresources/images/bullet/dot.png";

        // 创建弹幕
        LinearBulletItem *bullet = new LinearBulletItem(true,damage, speed, angle, pixmapPath);

        // 设置弹幕初始状态
        bullet->setPos(startPos);
        bullet->setZValue(100);

        // 添加到场景
        gameScene->addItem(bullet);
    }
    gameView->viewport()->update();

    qDebug() << "--- spawnBullet() finished creating" << numberOfBullets << "bullets ---";
}

//圆形弹幕
void BattleWidget::circleBullet() {

    int numberOfBullets = 10; // 一次生成弹幕数
    // 随机生成中心
    double randomFactor = QRandomGenerator::global()->generateDouble();
    qreal centerX = 346-150+300*randomFactor;
    randomFactor = QRandomGenerator::global()->generateDouble();
    qreal centerY = 228-150+300*randomFactor;

    for (int i = 0; i < numberOfBullets; ++i) {

        // 计算生成位置
        qreal pi=3.1415926;
        qreal startX =centerX+350*qCos(i*pi*36/180);
        qreal startY =centerY+350*qSin(i*pi*36/180);

        QPointF startPos(startX, startY);
        qDebug() << "Calculated startPos for bullet" << i << ":" << startPos;

        // 设置弹幕属性
        qreal angle = 180+i*36;
        qreal speed = 1.5;

        int damage = 2;
        QString pixmapPath = "./ktresources/images/bullet/dot.png";

        // 创建弹幕
        LinearBulletItem *bullet = new LinearBulletItem(true,damage, speed, angle, pixmapPath);

        // 设置弹幕初始状态
        bullet->setPos(startPos);
        bullet->setZValue(100);

        // 添加到场景
        gameScene->addItem(bullet);
    }
    gameView->viewport()->update();
}

//跟踪弹幕
void BattleWidget::spawnHomingAttack() {

    // 设置弹幕属性
    int damage = 2;
    qreal speed = 0.8;
    QString pixmapPath = "./ktresources/images/bullet/scissor.png";

    for(int i=1;i>=0;i--){

        // 确定生成位置
        qreal startX = 0;
        qreal startY = 0;
        QRectF sceneRect = gameScene->sceneRect();
        if(i==0){
            startX = sceneRect.left() +50;
            startY = QRandomGenerator::global()->bounded(sceneRect.height());
        }else{
            startX = sceneRect.right() -50;
            startY = QRandomGenerator::global()->bounded(sceneRect.height());
        }
        QPointF startPos(startX, startY);

        // 创建 HomingBulletItem 实例，传递 playerHeart
        HomingBulletItem *bullet = new HomingBulletItem(damage, speed, playerHeart, pixmapPath);

        bullet->setPos(startPos);
        bullet->setZValue(100);
        gameScene->addItem(bullet);
    }
    gameView->viewport()->update();
    qDebug() << "Spawned Homing Bullet targeting player.";
}

void BattleWidget::fistBullet(int x,int y,int fistAngle){

    qreal speed = 0;
    qreal startX =x;
    qreal startY =y;

    QPointF startPos(startX, startY);

    QString warningPath = "./ktresources/images/bullet/warning.png";
    WarningItem *warningItem = new WarningItem(fistAngle,warningPath);

    warningItem->setPos(startPos);
    warningItem->setZValue(-1);
    gameScene->addItem(warningItem);
    warningSound->play();

    QPointer<WarningItem> warningPtr = warningItem;

    // Warning闪烁动画
    QString emptyPath = "";
    int flashInterval = 100; // 闪烁间隔
    int flashCount = 10;      // 闪烁次数
    int warningTotal = flashInterval * flashCount; // 总的 Warning 显示时间

    for (int i = 0; i < flashCount; ++i) {
        int delay = flashInterval * i;
        QString pathToShow = (i % 2 == 0) ? warningPath : emptyPath; // 交替显示 warning 和 empty
        QTimer::singleShot(delay, this, [warningPtr, pathToShow]() {
            warningPtr->setPixmap(pathToShow);
        });
    }

    // 生成攻击弹幕
    QTimer::singleShot(warningTotal + 50, this, [this, warningPtr, startPos, fistAngle, speed]() {
        // 移除 Warning
        warningPtr->scene()->removeItem(warningPtr);
        delete warningPtr;

        int damage = 2;
        QString pixmapPath = "./ktresources/images/bullet/fist.png";
        FixedBulletItem *bullet = new FixedBulletItem(damage, fistAngle, pixmapPath);
        bullet->setPos(startPos);
        bullet->setZValue(100);
        QPointer<FixedBulletItem> bulletPtr = bullet;
        gameScene->addItem(bullet);
        fistSound->play();

        QTimer::singleShot(2000, this, [bulletPtr, this]() { // 3000ms自动删除
            bulletPtr->scene()->removeItem(bulletPtr);
            delete bulletPtr;
        });
    });
    gameView->viewport()->update();
}

//音符弹幕
void BattleWidget::noteBullet() {
    if (batchSpawnTimer->isActive()) {
        return;
    }
    setEnemyEffect("./ktresources/images/effect/guitar.png");
    currentBatch = 0; // 重置批次计数器

    handleBatchSpawn();
}
// 定时器触发的槽函数，生成每一批弹幕
void BattleWidget::handleBatchSpawn() {
    if (currentBatch >= totalBatches) { // 检查是否所有批次都已生成
        if (batchSpawnTimer->isActive()) {
            batchSpawnTimer->stop(); // 停止定时器
        }
        return;
    }


    qreal centerX = 120;
    qreal centerY = 120;
    qreal batchAngleOffset = currentBatch * 18.0; // 计算当前批次的起始角度偏移

    for (int j = 0; j < bulletsPerBatch; ++j) {

        // 计算位置
        qreal angleDegrees = j * 24;
        qreal angleRadians = qDegreesToRadians(angleDegrees);
        qreal startX = centerX;
        qreal startY = centerY;
        QPointF startPos(startX, startY);

        // 移动角度
        qreal movementAngle = 180.0 + j * 36.0 + batchAngleOffset;
        qreal speed = 1.5;
        int damage = 2;
        QString pixmapPath = "./ktresources/images/bullet/8note.png";

        // 创建、添加弹幕
        LinearBulletItem *bullet = new LinearBulletItem(true,damage, speed, movementAngle, pixmapPath);
        if (!bullet) continue;
        bullet->setPos(startPos);
        bullet->setZValue(100);
        if (gameScene) {
            gameScene->addItem(bullet);
        } else {
            qWarning() << "gameScene is null!";
            delete bullet;
            continue;
        }
    }

    currentBatch++; // 批次计数增加
    // 启动/保持定时器以生成下一批
    if (currentBatch < totalBatches) {
        if (!batchSpawnTimer->isActive() || batchSpawnTimer->interval() != batchInterval) {
            batchSpawnTimer->start(batchInterval);
        }
    } else {
        // 最后一批，停止定时器
        if (batchSpawnTimer->isActive()) {
            batchSpawnTimer->stop();
        }
    }
    gameView->viewport()->update();
}

// 波浪音符弹幕
void BattleWidget::noteWaveAttack() {
    if (batchSpawnTimerW->isActive()) {
        return;
    }

    currentBatchW = 0; // 重置批次计数器

    handleBatchSpawnW();
}
// 定时器触发的槽函数，生成每一批弹幕
void BattleWidget::handleBatchSpawnW() {
    if (currentBatchW >= totalBatchesW) { // 检查是否所有批次都已生成
        if (batchSpawnTimerW->isActive()) {
            batchSpawnTimerW->stop(); // 停止定时器
        }
        return;
    }

    // 计算位置
    qreal startY = 200+50*qCos(3.1415927*currentBatchW/24);
    qreal startX = 800;
    QPointF startPos(startX, startY);
    QPointF startPos1(startX, startY+140);

    // 移动角度
    qreal movementAngle = 180.0 ;
    qreal speed = 22;
    int damage = 2;
    QString pixmapPath = "./ktresources/images/bullet/note.png";

    // 创建、添加弹幕
    LinearBulletItem *bullet = new LinearBulletItem(true,damage, speed, movementAngle, pixmapPath);
    LinearBulletItem *bullet1 = new LinearBulletItem(true,damage, speed, movementAngle, pixmapPath);
    bullet->setPos(startPos);
    bullet1->setPos(startPos1);
    bullet->setZValue(100);
    bullet1->setZValue(100);
    gameScene->addItem(bullet);
    gameScene->addItem(bullet1);

    currentBatchW++; // 批次计数增加
    // 启动/保持定时器以生成下一批
    if (currentBatchW < totalBatchesW) {
        if (!batchSpawnTimerW->isActive() || batchSpawnTimerW->interval() != batchIntervalW) {
            batchSpawnTimerW->start(batchIntervalW);
        }
    } else {
        // 最后一批，停止定时器
        if (batchSpawnTimerW->isActive()) {
            batchSpawnTimerW->stop();
        }
    }
    gameView->viewport()->update();
}

//十字音符弹幕
void BattleWidget::noteCrossAttack() {
    if (batchSpawnTimerC->isActive()) {
        return;
    }
    //setEnemyEffect("./ktresources/images/effect/guitar.png");
    currentBatchC = 0; // 重置批次计数器

    QString warningPath = "./ktresources/images/bullet/crosswarning.png";
    WarningItem *warningItem = new WarningItem(0,warningPath);

    warningItem->setPos(260,163);
    warningItem->setZValue(-1);
    gameScene->addItem(warningItem);
    warningSound->play();

    QPointer<WarningItem> warningPtr = warningItem;

    // Warning闪烁动画
    QString emptyPath = "";
    int flashInterval = 100; // 闪烁间隔
    int flashCount = 10;      // 闪烁次数
    int warningTotal = flashInterval * flashCount; // 总的 Warning 显示时间
    for (int i = 0; i < flashCount; ++i) {
        int delay = flashInterval * i;
        QString pathToShow = (i % 2 == 0) ? warningPath : emptyPath; // 交替显示 warning 和 empty
        QTimer::singleShot(delay, this, [warningPtr, pathToShow]() {
            warningPtr->setPixmap(pathToShow);
        });
    }

    QTimer::singleShot(1000 , this, [this,warningPtr](){
        warningPtr->scene()->removeItem(warningPtr);
        delete warningPtr;
        handleBatchSpawnC();
    });

}
// 定时器触发的槽函数，生成每一批弹幕
void BattleWidget::handleBatchSpawnC() {
    if (currentBatchC >= totalBatchesC) { // 检查是否所有批次都已生成
        if (batchSpawnTimerC->isActive()) {
            batchSpawnTimerC->stop(); // 停止定时器
        }
        return;
    }


    qreal centerX = 346;
    qreal centerY = 252;
    qreal batchAngleOffset = currentBatchC * 13.0; // 计算当前批次的起始角度偏移

    for (int j = 0; j < 4; ++j) {

        // 计算位置
        //qreal angleDegrees = j * 90 ;
        //qreal angleRadians = qDegreesToRadians(angleDegrees);
        qreal startX = centerX;
        qreal startY = centerY;
        QPointF startPos(startX, startY);

        // 移动角度
        qreal movementAngle = 180.0 + j * 90.0 + batchAngleOffset;
        qreal speed = 10;
        int damage = 2;
        QString pixmapPath = "./ktresources/images/bullet/note1.png";

        // 创建、添加弹幕
        LinearBulletItem *bullet = new LinearBulletItem(false, damage, speed, movementAngle, pixmapPath);
        if (!bullet) continue;
        bullet->setPos(startPos);
        bullet->setZValue(100);
        if (gameScene) {
            gameScene->addItem(bullet);
        } else {
            qWarning() << "gameScene is null!";
            delete bullet;
            continue;
        }
    }

    currentBatchC++; // 批次计数增加
    // 启动/保持定时器以生成下一批
    if (currentBatchC < totalBatchesC) {
        if (!batchSpawnTimerC->isActive() || batchSpawnTimerC->interval() != batchIntervalC) {
            batchSpawnTimerC->start(batchIntervalC);
        }
    } else {
        // 最后一批，停止定时器
        if (batchSpawnTimerC->isActive()) {
            batchSpawnTimerC->stop();
        }
    }
    gameView->viewport()->update();
}

void BattleWidget::crossLaserBullet(){

    //setEnemySprite("./ktresources/images/kano/k02.png");

    qreal speed = 0;
    qreal startX =346;
    qreal startY =228;

    QPointF startPos1(startX-125, startY-240);
    QPointF startPos11(startX-130, startY-45);
    QPointF startPos2(startX+255, startY-130);
    QPointF startPos22(startX+95, startY-100);

    WarningItem *warningItem1 = new WarningItem(45,"./ktresources/images/bullet/bambi1.png");
    WarningItem *warningItem2 = new WarningItem(135,"./ktresources/images/bullet/bambi2.png");

    warningItem1->setPos(startPos1);
    warningItem2->setPos(startPos2);
    warningItem1->setZValue(-1);
    warningItem2->setZValue(-1);
    gameScene->addItem(warningItem1);
    gameScene->addItem(warningItem2);

    warningSound->play();

    QPointer<WarningItem> warningPtr1 = warningItem1;
    QPointer<WarningItem> warningPtr2 = warningItem2;

    // Warning闪烁动画
    int flashInterval = 100; // 闪烁间隔
    int flashCount = 8;      // 闪烁次数
    int warningTotal = flashInterval * flashCount; // 总的 Warning 显示时间

    for (int i = 0; i < flashCount; ++i) {
        int delay = flashInterval * i;
        QTimer::singleShot(delay, this, [warningPtr1,warningPtr2, i]() {
            if(i%2==0){
                warningPtr1->setPixmap("");
                warningPtr2->setPixmap("");
            }else{
                warningPtr1->setPixmap("./ktresources/images/bullet/bambi1.png");
                warningPtr2->setPixmap("./ktresources/images/bullet/bambi2.png");
            }
        });
    }

    // 生成攻击弹幕
    QTimer::singleShot(warningTotal + 50, this, [this, warningPtr1, warningPtr2, startPos11, startPos22, speed]() {

        int damage = 3;
        QString pixmapPath = "./ktresources/images/bullet/laser.png";
        FixedBulletItem *bullet1 = new FixedBulletItem(damage, 315, pixmapPath);
        FixedBulletItem *bullet2 = new FixedBulletItem(damage, 45, pixmapPath);
        bullet1->setPos(startPos11);
        bullet2->setPos(startPos22);
        bullet1->setZValue(100);
        bullet2->setZValue(100);
        QPointer<FixedBulletItem> bulletPtr1 = bullet1;
        QPointer<FixedBulletItem> bulletPtr2 = bullet2;
        gameScene->addItem(bullet1);
        gameScene->addItem(bullet2);

        laserSound->play();

        QTimer::singleShot(1000, this, [bulletPtr1, bulletPtr2, warningPtr1, warningPtr2, this]() { //自动删除
            bulletPtr1->scene()->removeItem(bulletPtr1);
            bulletPtr2->scene()->removeItem(bulletPtr2);
            delete bulletPtr1;
            delete bulletPtr2;
            // 移除 Warning
            warningPtr1->scene()->removeItem(warningPtr1);
            warningPtr2->scene()->removeItem(warningPtr2);
            delete warningPtr1;
            delete warningPtr2;
            //setEnemySprite("./ktresources/images/kano/k01.png");
        });
    });
    gameView->viewport()->update();
}

void BattleWidget::paddleLaserBullet(){

    //setEnemySprite("./ktresources/images/kano/k02.png");

    qreal speed = 0;
    qreal startX =346;
    qreal startY =228;

    QPointF startPos1(startX-230, startY-145);
    QPointF startPos11(startX+130, startY+65);
    QPointF startPos2(startX+260, startY+150);
    QPointF startPos22(startX-100, startY+10);

    WarningItem *warningItem1 = new WarningItem(0,"./ktresources/images/bullet/bambi1.png");
    WarningItem *warningItem2 = new WarningItem(180,"./ktresources/images/bullet/bambi2.png");

    warningItem1->setPos(startPos1);
    warningItem2->setPos(startPos2);
    warningItem1->setZValue(-1);
    warningItem2->setZValue(-1);
    gameScene->addItem(warningItem1);
    gameScene->addItem(warningItem2);

    warningSound->play();

    QPointer<WarningItem> warningPtr1 = warningItem1;
    QPointer<WarningItem> warningPtr2 = warningItem2;

    // Warning闪烁动画
    int flashInterval = 100; // 闪烁间隔
    int flashCount = 8;      // 闪烁次数
    int warningTotal = flashInterval * flashCount; // 总的 Warning 显示时间

    for (int i = 0; i < flashCount; ++i) {
        int delay = flashInterval * i;
        QTimer::singleShot(delay, this, [warningPtr1,warningPtr2, i]() {
            if(i%2==0){
                warningPtr1->setPixmap("");
                warningPtr2->setPixmap("");
            }else{
                warningPtr1->setPixmap("./ktresources/images/bullet/bambi1.png");
                warningPtr2->setPixmap("./ktresources/images/bullet/bambi2.png");
            }
        });
    }

    // 生成攻击弹幕
    QTimer::singleShot(warningTotal + 50, this, [this, warningPtr1, warningPtr2, startPos11, startPos22, speed]() {

        int damage = 3;
        QString pixmapPath = "./ktresources/images/bullet/laser.png";
        FixedBulletItem *bullet1 = new FixedBulletItem(damage, 90, pixmapPath);
        FixedBulletItem *bullet2 = new FixedBulletItem(damage, 270, pixmapPath);
        bullet1->setPos(startPos11);
        bullet2->setPos(startPos22);
        bullet1->setZValue(100);
        bullet2->setZValue(100);
        QPointer<FixedBulletItem> bulletPtr1 = bullet1;
        QPointer<FixedBulletItem> bulletPtr2 = bullet2;
        gameScene->addItem(bullet1);
        gameScene->addItem(bullet2);

        laserSound->play();

        QTimer::singleShot(1000, this, [bulletPtr1, bulletPtr2, warningPtr1, warningPtr2, this]() { // 3000ms自动删除
            bulletPtr1->scene()->removeItem(bulletPtr1);
            bulletPtr2->scene()->removeItem(bulletPtr2);
            delete bulletPtr1;
            delete bulletPtr2;
            // 移除 Warning
            warningPtr1->scene()->removeItem(warningPtr1);
            warningPtr2->scene()->removeItem(warningPtr2);
            delete warningPtr1;
            delete warningPtr2;
            //setEnemySprite("./ktresources/images/kano/k01.png");
        });
    });
    gameView->viewport()->update();
}

void BattleWidget::vpaddleLaserBullet(){

    //setEnemySprite("./ktresources/images/kano/k02.png");

    qreal speed = 0;
    qreal startX =346;
    qreal startY =228;

    QPointF startPos1(startX+40, startY-190);
    QPointF startPos11(startX+130, startY+155);
    QPointF startPos2(startX+45, startY+265);
    QPointF startPos22(startX-100, startY-80);

    WarningItem *warningItem1 = new WarningItem(90,"./ktresources/images/bullet/bambi1.png");
    WarningItem *warningItem2 = new WarningItem(270,"./ktresources/images/bullet/bambi2.png");

    warningItem1->setPos(startPos1);
    warningItem2->setPos(startPos2);
    warningItem1->setZValue(-1);
    warningItem2->setZValue(-1);
    gameScene->addItem(warningItem1);
    gameScene->addItem(warningItem2);

    QPointer<WarningItem> warningPtr1 = warningItem1;
    QPointer<WarningItem> warningPtr2 = warningItem2;

    warningSound->play();

    // Warning闪烁动画
    int flashInterval = 100; // 闪烁间隔
    int flashCount = 8;      // 闪烁次数
    int warningTotal = flashInterval * flashCount; // 总的 Warning 显示时间

    for (int i = 0; i < flashCount; ++i) {
        int delay = flashInterval * i;
        QTimer::singleShot(delay, this, [warningPtr1,warningPtr2, i]() {
            if(i%2==0){
                warningPtr1->setPixmap("");
                warningPtr2->setPixmap("");
            }else{
                warningPtr1->setPixmap("./ktresources/images/bullet/bambi1.png");
                warningPtr2->setPixmap("./ktresources/images/bullet/bambi2.png");
            }
        });
    }

    // 生成攻击弹幕
    QTimer::singleShot(warningTotal + 50, this, [this, warningPtr1, warningPtr2, startPos11, startPos22, speed]() {

        int damage = 3;
        QString pixmapPath = "./ktresources/images/bullet/laser.png";
        FixedBulletItem *bullet1 = new FixedBulletItem(damage, 180, pixmapPath);
        FixedBulletItem *bullet2 = new FixedBulletItem(damage, 0, pixmapPath);
        bullet1->setPos(startPos11);
        bullet2->setPos(startPos22);
        bullet1->setZValue(100);
        bullet2->setZValue(100);
        QPointer<FixedBulletItem> bulletPtr1 = bullet1;
        QPointer<FixedBulletItem> bulletPtr2 = bullet2;
        gameScene->addItem(bullet1);
        gameScene->addItem(bullet2);

        laserSound->play();

        QTimer::singleShot(2000, this, [bulletPtr1, bulletPtr2, warningPtr1, warningPtr2, this]() { // 3000ms自动删除
            //bulletPtr1->scene()->removeItem(bulletPtr1);
            //bulletPtr2->scene()->removeItem(bulletPtr2);
            //delete bulletPtr1;
            //delete bulletPtr2;
            // 移除 Warning
            //warningPtr1->scene()->removeItem(warningPtr1);
            //warningPtr2->scene()->removeItem(warningPtr2);
            //delete warningPtr1;
            //delete warningPtr2;
            //setEnemySprite("./ktresources/images/kano/k01.png");
        });
    });
    gameView->viewport()->update();
}

void BattleWidget::crossArrowAttack(){
    // 设置弹幕属性
    int damage = 1;
    qreal speed = 1.8;
    QString pixmapPath = "./ktresources/images/bullet/star1.png";
    QPointF currentHeartPos = playerHeart->pos();
    qreal startX = 0;
    qreal startY = 0;
    qreal movementAngle = 0;
    for(int i=3;i>=0;i--){
        // 确定生成位置
        switch (i) {
        case 0:
            startX = 140;
            startY = 0;
            movementAngle = 180;
            break;
        case 1:
            startX = -140;
            startY = 0;
            movementAngle = 0;
            break;
        case 2:
            startX = 0;
            startY = 140;
            movementAngle = 270;
            break;
        case 3:
            startX = 0;
            startY = -140;
            movementAngle = 90;
            break;
        }
        QPointF startPos(startX, startY);

        LinearBulletItem *bullet = new LinearBulletItem(false, damage, speed, movementAngle, pixmapPath);

        bullet->setPos(startPos+currentHeartPos);
        bullet->setZValue(100);
        gameScene->addItem(bullet);
    }
    gameView->viewport()->update();
};

void BattleWidget::dogAttack(int position,bool right){

    int damage = 1;
    qreal speed = 2.5;
    qreal startX = 0;
    qreal startY = 0;
    QString pixmapPath1;
    QString pixmapPath2;
    qreal movementAngle = 0;

    switch (position) {
    case 1:
        startY = 100;
        break;
    case 2:
        startY = 200;
        break;
    case 3:
        startY = 300;
        break;
    }

    if(right){
        startX = 700;
        pixmapPath1 = "./ktresources/images/bullet/dog1.png";
        pixmapPath2 = "./ktresources/images/bullet/dog2.png";
        movementAngle = 180;
    }else{
        startX = -120;
        pixmapPath1 = "./ktresources/images/bullet/dog3.png";
        pixmapPath2 = "./ktresources/images/bullet/dog4.png";
        movementAngle = 0;
    }

    QPointF startPos(startX, startY);
    //LinearBulletItem *bullet = new LinearBulletItem(false, damage, speed, movementAngle, pixmapPath1);
    DogBulletItem *bullet = new DogBulletItem(damage, speed, movementAngle, pixmapPath1, pixmapPath2);
    bullet->setPos(startPos);
    bullet->setZValue(100);
    gameScene->addItem(bullet);

    gameView->viewport()->update();
};

void BattleWidget::glcefAttack(int position,bool right){

    int damage = 1;
    qreal speed = 4.5;
    qreal startX = 0;
    qreal startY = 0;
    QString pixmapPath1;
    QString pixmapPath2;
    qreal movementAngle = 0;

    switch (position) {
    case 1:
        startY = 100;
        break;
    case 2:
        startY = 200;
        break;
    case 3:
        startY = 300;
        break;
    }

    if(right){
        startX = 700;
        pixmapPath1 = "./ktresources/images/bullet/gclef1.png";
        pixmapPath2 = "./ktresources/images/bullet/gclef2.png";
        movementAngle = 180;
    }else{
        startX = -120;
        pixmapPath1 = "./ktresources/images/bullet/dog3.png";
        pixmapPath2 = "./ktresources/images/bullet/dog4.png";
        movementAngle = 0;
    }

    QPointF startPos(startX, startY);
    //LinearBulletItem *bullet = new LinearBulletItem(false, damage, speed, movementAngle, pixmapPath1);
    DogBulletItem *bullet = new DogBulletItem(damage, speed, movementAngle, pixmapPath1, pixmapPath2);
    bullet->setPos(startPos);
    bullet->setZValue(100);
    gameScene->addItem(bullet);

    gameView->viewport()->update();
};

void BattleWidget::xcrossArrowAttack(){
    // 设置弹幕属性
    int damage = 1;
    qreal speed = 1.8;
    QString pixmapPath = "./ktresources/images/bullet/star1.png";
    QPointF currentHeartPos = playerHeart->pos();
    qreal startX = 0;
    qreal startY = 0;
    qreal movementAngle = 0;
    for(int i=3;i>=0;i--){
        // 确定生成位置
        switch (i) {
        case 0:
            startX = 100;
            startY = 100;
            movementAngle = 225;
            break;
        case 1:
            startX = -100;
            startY = -100;
            movementAngle = 45;
            break;
        case 2:
            startX = -100;
            startY = 100;
            movementAngle = 315;
            break;
        case 3:
            startX = 100;
            startY = -100;
            movementAngle = 135;
            break;
        }
        QPointF startPos(startX, startY);

        LinearBulletItem *bullet = new LinearBulletItem(false, damage, speed, movementAngle, pixmapPath);

        bullet->setPos(startPos+currentHeartPos);
        bullet->setZValue(100);
        gameScene->addItem(bullet);
    }
    gameView->viewport()->update();
};

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
    //QPushButton* checkButton = new QPushButton(" * 查看", actMenuPage);
    checkButton = new QPushButton(" * 查看", actMenuPage);
    QPushButton* talkButton = new QPushButton(" * 交谈", actMenuPage);
    QPushButton* askButton = new QPushButton(" * 询问", actMenuPage);
    QPushButton* fondleButton = new QPushButton(" * 抚摸", actMenuPage);
    QPushButton* threatenButton = new QPushButton(" * 威胁", actMenuPage);
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
    actMenuLayout3->addWidget(threatenButton);
    actMenuLayout3->addWidget(emptyButton);
    emptyButton->setEnabled(false);
    actMenuLayout->addStretch();
    // --- 应用悬停聚焦功能 ---
    checkButton->installEventFilter(this);   // 安装过滤器
    talkButton->installEventFilter(this);
    askButton->installEventFilter(this);
    fondleButton->installEventFilter(this);
    threatenButton->installEventFilter(this);
    checkButton->setFocusPolicy(Qt::StrongFocus);   // 设置焦点策略
    talkButton->setFocusPolicy(Qt::StrongFocus);
    askButton->setFocusPolicy(Qt::StrongFocus);
    fondleButton->setFocusPolicy(Qt::StrongFocus);
    threatenButton->setFocusPolicy(Qt::StrongFocus);
    //actMenuLayout->addWidget(actBackButton);

    itemMenuPage = new QWidget(actionStackedWidget);
    QVBoxLayout *itemMenuLayout = new QVBoxLayout(itemMenuPage);
    QLabel *itemLabel =new QLabel("物品菜单:", itemMenuPage);
    QHBoxLayout *itemMenuLayout1=new QHBoxLayout(itemMenuPage);
    QVBoxLayout *itemMenuLayoutL = new QVBoxLayout(itemMenuPage);
    itemButton1 = new QPushButton(" * 小鹿包", itemMenuPage);
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
    mercyButton1 = new QPushButton(" * 仁慈", mercyMenuPage);
    QPushButton* escapeButton = new QPushButton(" * 逃跑", mercyMenuPage);
    mercyMenuLayout->addWidget(enemyName);
    mercyMenuLayout->addWidget(mercyButton1);
    mercyMenuLayout->addWidget(escapeButton);
    mercyMenuLayout->addStretch();
    mercyButton1->installEventFilter(this);
    escapeButton->installEventFilter(this);
    mercyButton1->setFocusPolicy(Qt::StrongFocus);
    escapeButton->setFocusPolicy(Qt::StrongFocus);


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


    battlePage = new QWidget(actionStackedWidget);  // 不再使用
    attackPage = new QWidget(actionStackedWidget);

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
    fightButton->setIcon(           QIcon("./ktresources/images/icon/fight.png"));
    actButton->setIcon(QIcon("./ktresources/images/icon/act.png"));
    itemButton->setIcon(QIcon("./ktresources/images/icon/item.png"));
    mercyButton->setIcon(QIcon("./ktresources/images/icon/mercy.png"));

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    settingsButton =new QPushButton("设置",battleBoxFrame);
    settingsButton->setFocusPolicy(Qt::NoFocus);
    connect(settingsButton, &QPushButton::clicked, this, &BattleWidget::onSettingsClicked);
    QLabel *gameInfo = new QLabel("Kanotale beta 0.5",battleBoxFrame);
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
        //"    icon: url(./ktresources/images/icon/heart.png);"
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

    attackPage->setStyleSheet(
        "    background-image: url(./ktresources/images/attack/bg.png);"
        "    background-repeat: no-repeat;"   // 不重复平铺
        "    background-position: center;"    // 中心对齐
        "    background-origin: content;"     // 绘制原点
        "}"
        );

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

// 控制回合/剧情
void BattleWidget::startRound()
{
    switch (round) {
    case 0:
        // 初始对话
        startEDialogue("* 你 \n* 还 是 来 了 。");
        actionStackedWidget->setCurrentWidget(battlePage);

        startBattlePrepare();

        QTimer::singleShot(1000 , this, [this](){ crossLaserBullet();});
        QTimer::singleShot(3000 , this, [this](){ paddleLaserBullet();});
        QTimer::singleShot(5000 , this, [this](){ crossLaserBullet();});
        QTimer::singleShot(7000 , this, [this](){ vpaddleLaserBullet();});

        QTimer::singleShot(9000 , this, [this](){
            //显示敌人
            startDialogue("* 鹿乃出现在你面前.");
            actionStackedWidget->setCurrentWidget(dialoguePage);
            enemyHpProgressBar->show();
            setEnemySprite("./ktresources/images/kano/k02.png");
            //开始播放背景音乐

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        QTimer::singleShot(9700 , this, [this](){
            setEnemySprite(ENEMY_SPRITE_PATH);
            startEDialogue("* 开始吧。");
            playBackgroundMusic();
        });
        round+=1;
        break;
    case 1:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        spawnBullet();
        QTimer::singleShot(3000 , this, [this](){ spawnBullet();spawnBullet();});
        QTimer::singleShot(6000 , this, [this](){ spawnBullet();circleBullet();});
        QTimer::singleShot(10000 , this, [this](){ circleBullet();circleBullet(); });
        QTimer::singleShot(16000 , this, [this](){

            startDialogue("* 她就站在那里.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 2:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        QTimer::singleShot(2000 , this, [this](){ dogAttack(1,false);dogAttack(2,true);dogAttack(3,false);});
        QTimer::singleShot(7000 , this, [this](){ dogAttack(1,true);dogAttack(2,false);dogAttack(3,true);});
        QTimer::singleShot(13000 , this, [this](){

            startDialogue("* 她就站在那里.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 3:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        //spawnBullet();
        QTimer::singleShot(2000 , this, [this](){ spawnHomingAttack();});
        //QTimer::singleShot(8000 , this, [this](){ paddleLaserBullet();});
        QTimer::singleShot(6000 , this, [this](){ circleBullet(); });
        QTimer::singleShot(12000 , this, [this](){

            startDialogue("* 她就站在那里.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 4:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        QTimer::singleShot(2000 , this, [this](){ crossArrowAttack();});
        QTimer::singleShot(4000 , this, [this](){ xcrossArrowAttack();});
        QTimer::singleShot(6000 , this, [this](){ crossArrowAttack(); });
        QTimer::singleShot(8000 , this, [this](){ xcrossArrowAttack(); });
        QTimer::singleShot(10000 , this, [this](){ crossArrowAttack(); });
        QTimer::singleShot(13000 , this, [this](){

            startDialogue("* 她就站在那里.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 5:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        QTimer::singleShot(1000 , this, [this](){ dogAttack(1,false);dogAttack(3,true);});
        QTimer::singleShot(4000 , this, [this](){ xcrossArrowAttack();dogAttack(2,true);});
        QTimer::singleShot(7000 , this, [this](){ dogAttack(1,true);crossArrowAttack();dogAttack(2,false); });
        QTimer::singleShot(10000 , this, [this](){ xcrossArrowAttack(); });
        QTimer::singleShot(13500 , this, [this](){ dogAttack(3,false);crossArrowAttack(); });
        QTimer::singleShot(17000 , this, [this](){

            startDialogue("* 她就站在那里.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 6:
        startEDialogue("* ...");
        actionStackedWidget->setCurrentWidget(battlePage);
        startBattlePrepare();

        QTimer::singleShot(1000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s1.png");noteBullet();});
        QTimer::singleShot(2000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s2.png");});
        QTimer::singleShot(3000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s3.png");});
        QTimer::singleShot(4000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s4.png");});
        QTimer::singleShot(5000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s5.png");});
        QTimer::singleShot(6000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s1.png");});
        QTimer::singleShot(7000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s2.png");});
        QTimer::singleShot(8000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s3.png");});
        QTimer::singleShot(9000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s4.png");});
        QTimer::singleShot(10000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s5.png");});
        QTimer::singleShot(11000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s1.png");});
        QTimer::singleShot(12000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s2.png");setEnemyEffect("");;noteWaveAttack();});
        QTimer::singleShot(13000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s3.png");});
        QTimer::singleShot(14000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s4.png");});
        QTimer::singleShot(15000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s5.png");});
        QTimer::singleShot(16000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s1.png");});
        QTimer::singleShot(17000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s2.png");});
        QTimer::singleShot(18000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s3.png");});
        QTimer::singleShot(19000 , this, [this](){ setEnemySprite("./ktresources/images/kano/s4.png");});
        QTimer::singleShot(20000 , this, [this](){

            ENEMY_SPRITE_PATH = "./ktresources/images/kano/k05.png";
            setEnemySprite(ENEMY_SPRITE_PATH);
            startDialogue("* 鹿乃看上去有些疲惫.");
            actionStackedWidget->setCurrentWidget(dialoguePage);

            //停止弹幕攻击
            stopGameLoop();
            this->activateWindow();
            fightButton->setFocus();
        });
        round+=1;
        break;
    case 7:
        ;
        round+=1;
        break;
    case 8:
        ;
        round+=1;
        break;
    case 9:
        ;
        round+=1;
        break;
    case 10:
        ;
        round+=1;
        break;
    case 11:
        ;
        round+=1;
        break;
    case 12:
        ;
        round+=1;
        break;
    case 13:
        ;
        round+=1;
        break;
    case 14:
        ;
        round+=1;
        break;
    case 15:
        ;
        round+=1;
        break;
    default:
        ;
    }
}

//
void BattleWidget::startBattlePrepare()
{
    if (gameView) {
        setupGameScene();
        positionGameView(); // 定位
        gameView->show();   // 显示
        gameView->raise();  // 提升到顶层
        gameView->activateWindow();
        QTimer::singleShot(0, this, [this](){ // 延迟 0ms 意味着在下一个事件循环设置
            playerHeart->setFocus();
        });
        startGameLoop();
    }
}

// --- 动作按钮槽 ---
void BattleWidget::onFightClicked()
{
    actionStackedWidget->setCurrentWidget(attackPage);
    startBattlePrepare();
    startAttack();
    QTimer::singleShot(2000, this, [this](){
        stopGameLoop();
        actionStackedWidget->setCurrentWidget(dialoguePage);
        this->activateWindow();
        fightButton->setFocus();
        startRound();
    });
}


void BattleWidget::onActClicked()
{
    actionStackedWidget->setCurrentWidget(actMenuPage);
    actionStackedWidget->show();
    checkButton->setFocus(Qt::MouseFocusReason);
    // ... ...
}

void BattleWidget::onItemClicked()
{
    actionStackedWidget->setCurrentWidget(itemMenuPage);
    actionStackedWidget->show();
    itemButton1->setFocus(Qt::MouseFocusReason);
    // ... ...
}

void BattleWidget::onMercyClicked()
{
    actionStackedWidget->setCurrentWidget(mercyMenuPage);
    actionStackedWidget->show();
    mercyButton1->setFocus(Qt::MouseFocusReason);
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



