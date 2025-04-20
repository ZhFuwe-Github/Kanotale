#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    bool isSoundEnabled() const;
    void setSoundEnabled(bool enabled);
    bool isHPFixEnabled() const;
    void setHPFixEnabled(bool enabled);

signals:
    void settingsConfirmed(bool soundEnabled, bool hpFixEnabled);

private slots:
    void onConfirmButtonClicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
