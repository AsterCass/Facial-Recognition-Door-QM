#include "ui/pages/main_page_setting_login.h"

#include "config/config.h"
#include "ui/main_router.h"
#include "ui/components/common_components.h"


using namespace std;

MainSettingLogin::MainSettingLogin(QWidget *parent): QWidget(parent) {
    setObjectName("mainSettingLogin");
    setStyleSheet("#mainSettingLogin{background-color: white}");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setMargin(20);

    passwdLabel = new QLabel("管理密码", this);
    passwd = new QLineEditPro(this);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setPlaceholderText("请输入管理员密码");
    loginBtn = new QPushButton("登录", this);
    cancelBtn = new QPushButton("取消", this);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backPage();
            });

    connect(loginBtn, &QPushButton::clicked, this,
            [=] {
                if (passwd->text().toStdString() == g_managementPassword) {
                    MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_TMP);
                } else {
                    passwd->setText("");
                }
            });

    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(passwdLabel);
    mainLayout->addWidget(passwd);
    mainLayout->addWidget(loginBtn);
    mainLayout->addWidget(cancelBtn);
    mainLayout->addStretch();
}


MainSettingLogin::~MainSettingLogin() = default;
