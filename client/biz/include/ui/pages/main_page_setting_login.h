#ifndef MAIN_PAGE_SETTING_LOGIN_H
#define MAIN_PAGE_SETTING_LOGIN_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class QLineEditPro;

class MainSettingLogin final : public QWidget {
public:
    MainSettingLogin(const MainSettingLogin &) = delete;

    MainSettingLogin &operator=(const MainSettingLogin &) = delete;

    static MainSettingLogin *getInstance(QWidget *parent) {
        static MainSettingLogin instance(parent);
        return &instance;
    }

private:
    explicit MainSettingLogin(QWidget *parent = nullptr);

    ~MainSettingLogin() override;

    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;


    QLabel *passwdLabel = nullptr;
    QLineEditPro *passwd = nullptr;
    QPushButton *loginBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
};


#endif //MAIN_PAGE_SETTING_LOGIN_H
