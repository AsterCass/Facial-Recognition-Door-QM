#include "ui/pages/main_page_setting_login.h"

#include "config/config.h"
#include "ui/main_router.h"
#include "ui/components/common_components.h"


using namespace std;

MainSettingLogin::MainSettingLogin(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "登录");

    passwdLabel = new QLabel("管理密码", this);
#ifdef WIN32
    passwdLabel->setStyleSheet("color: white; font-size: 18px; margin: 10px");
#else
    passwdLabel->setStyleSheet("color: white; font-size: 36px; margin: 20px");
#endif


    passwd = new QLineEditPro(this);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setPlaceholderText("请输入管理员密码");
#ifdef WIN32
    passwd->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 16px; border-radius: 8px; "
        "padding-left: 8px; padding-right: 8px; color: white; margin: 10px");
    passwd->setFixedHeight(75);
#else
    passwd->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 32px; border-radius: 8px; "
        "padding-left: 16px; padding-right: 16px; color: white; margin: 20px");
    passwd->setFixedHeight(150);
#endif


    btnWidget = new QWidget(this);
    btnLayout = new QHBoxLayout(btnWidget);
    loginBtn = new QPushButton("登录", btnWidget);
    cancelBtn = new QPushButton("取消", btnWidget);
#ifdef WIN32
    loginBtn->setStyleSheet(
        "background-color: rgb(13, 133, 255); color: white; border-radius: 8px;box-shadow: none;"
        " height: 35px; font-size: 20px");
    cancelBtn->setStyleSheet(
        "background-color: rgb(101, 101, 101); color: white; border-radius: 8px;box-shadow: none;"
        " height: 35px; font-size: 20px");
#else
    loginBtn->setStyleSheet(
        "background-color: rgb(13, 133, 255); color: white; border-radius: 16px;box-shadow: none;"
        " height: 70px; font-size: 40px");
    cancelBtn->setStyleSheet(
        "background-color: rgb(101, 101, 101); color: white; border-radius: 16px;box-shadow: none;"
        " height: 70px; font-size: 40px");
#endif

    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backPage();
                passwd->setText("");
            });

    connect(loginBtn, &QPushButton::clicked, this,
            [=] {
                if (passwd->text().toStdString() == g_managementPassword) {
                    MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_MAIN);
                } else if (passwd->text().toStdString() == g_managementPassword + "_dev") {
                    MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_TMP);
                }
                passwd->setText("");
            });
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(loginBtn);


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(passwdLabel);
    mainLayout->addWidget(passwd);
    mainLayout->addWidget(btnWidget);
    mainLayout->addStretch();
}


MainSettingLogin::~MainSettingLogin() = default;

void MainSettingLogin::hideEvent(QHideEvent *) {
    passwd->setText("");
}
