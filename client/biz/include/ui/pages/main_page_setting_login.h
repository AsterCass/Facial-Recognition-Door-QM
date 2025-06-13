#ifndef MAIN_PAGE_SETTING_LOGIN_H
#define MAIN_PAGE_SETTING_LOGIN_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/setting_header.h>


class QLineEditPro;

class MainSettingLogin final : public QWidget {
public:
    explicit MainSettingLogin(QWidget *parent = nullptr);

    ~MainSettingLogin() override;

private:
    void hideEvent(QHideEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;

    QWidget *btnWidget = nullptr;
    QHBoxLayout *btnLayout = nullptr;

    QLabel *passwdLabel = nullptr;
    QLineEditPro *passwd = nullptr;
    QPushButton *loginBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    SettingHeader *settingHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_LOGIN_H
