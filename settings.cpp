#include "settings.h"
#include "ui_settings.h"
#include "widget.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &Settings::onConfirmButtonClicked);
}

Settings::~Settings()
{
    delete ui;
}

bool Settings::isSoundEnabled() const
{
    return ui->checkBox->isChecked();
}

bool Settings::isHPFixEnabled() const
{
    return ui->checkBox_2->isChecked();
}

void Settings::setSoundEnabled(bool enabled) {
    ui->checkBox->setChecked(enabled);
}
void Settings::setHPFixEnabled(bool enabled) {
    ui->checkBox_2->setChecked(enabled);
}

void Settings::onConfirmButtonClicked() {
    bool soundIsEnabled = ui->checkBox->isChecked();
    bool hpFixIsEnabled = ui->checkBox_2->isChecked();
    emit settingsConfirmed(soundIsEnabled, hpFixIsEnabled);
    this->close(); // 调用 close() 方法
}
