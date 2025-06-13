#include "ui/pages/main_page_setting_login.h"

#include <ui/components/setting_header.h>

#include "config/config.h"
#include "ui/main_router.h"
#include "ui/components/common_components.h"


using namespace std;

MainSettingLogin::MainSettingLogin(QWidget *parent): QWidget(parent) {
    setObjectName("mainSettingLogin");
    setStyleSheet("#mainSettingLogin{background-color: rgb(4, 9, 12)");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    passwdLabel = new QLabel("管理密码", this);
    passwd = new QLineEditPro(this);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setPlaceholderText("请输入管理员密码");
    loginBtn = new QPushButton("登录", this);
    cancelBtn = new QPushButton("取消", this);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backPage();
                passwd->setText("");
            });

    connect(loginBtn, &QPushButton::clicked, this,
            [=] {
                if (passwd->text().toStdString() == g_managementPassword) {
                    MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_TMP);
                }
                passwd->setText("");
            });

    mainLayout->setAlignment(Qt::AlignTop);

    SettingHeader *settingHeader = new SettingHeader(this, "登录");

    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(passwd);
    mainLayout->addWidget(passwdLabel);
    mainLayout->addWidget(passwd);
    mainLayout->addWidget(loginBtn);
    mainLayout->addWidget(cancelBtn);
    mainLayout->addStretch();
}


MainSettingLogin::~MainSettingLogin() = default;
