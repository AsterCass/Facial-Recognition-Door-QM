#ifndef MAIN_PAGE_SETTING_LOGIN_H
#define MAIN_PAGE_SETTING_LOGIN_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class QLineEditPro;

class MainSettingLogin final : public QWidget {
public:
    explicit MainSettingLogin(QWidget *parent = nullptr);

    ~MainSettingLogin() override;

private:
    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;


    QLabel *passwdLabel = nullptr;
    QLineEditPro *passwd = nullptr;
    QPushButton *loginBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
};


#endif //MAIN_PAGE_SETTING_LOGIN_H
