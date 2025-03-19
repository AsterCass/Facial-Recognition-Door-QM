#include "ui/pages/main_page_setting_login.h"

#include <sstream>

#include "airstrip_log.h"
#include "config/config.h"
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
                if (g_routerQueue.empty()) {
                    return;
                }
                g_routerQueue.pop_back();
                if (g_routerQueue.empty()) {
                    return;
                }
                const auto lastWidget = g_routerQueue.back();
                g_stackedWidget->setCurrentIndex(lastWidget);
                ostringstream oss;
                oss << "Current widget stack: ";
                for (const auto index: g_routerQueue) {
                    oss << index << " ";
                }
                airstrip::logPrintln(oss.str());
            });


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(passwdLabel);
    mainLayout->addWidget(passwd);
    mainLayout->addWidget(loginBtn);
    mainLayout->addWidget(cancelBtn);
    mainLayout->addStretch();


}


MainSettingLogin::~MainSettingLogin() = default;
