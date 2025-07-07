#include "ui/pages/main_page_setting_main.h"

#include "config/config.h"
#include "ui/main_router.h"
#include "ui/components/common_components.h"


using namespace std;

void setSettingMainItem(QToolButton *button, const string &name, const string &address) {
    if (nullptr == button) {
        return;
    }
    button->setText(QString::fromStdString(name));
    button->setIcon(QIcon(QString::fromStdString(address)));

#ifdef WIN32
    button->setIconSize(QSize(50, 50));
    button->setFixedSize(115, 115);
    button->setStyleSheet(R"(
        QToolButton {
            margin-top: 10px;
            padding-top: 20px;
            background-color: rgb(31, 31, 31);
            color: rgb(150, 150, 150);
            border-radius: 8px;
            font-size: 14px;
        }
    )");
#else
    button->setIconSize(QSize(100, 100));
    button->setFixedSize(230, 230);
    button->setStyleSheet(R"(
        QToolButton {
            margin-top: 20px;
            padding-top: 40px;
            background-color: #2b2b2b;
            color: rgb(150, 150, 150);
            border-radius: 16px;
            font-size: 28px;
        }
    )");
#endif
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

MainSettingMain::MainSettingMain(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "主菜单");


    settingBody = new QWidget(this);
    settingMainLayout = new QGridLayout(settingBody);
    settingMainLayout->setContentsMargins(0, 10, 0, 0);

    settingInfo = new QToolButton(settingBody);
    setSettingMainItem(settingInfo, "设备信息", g_appWorkDir + "static/images/setting-info.svg");
    connect(settingInfo, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_INFO);
            });

    settingNetwork = new QToolButton(settingBody);
    setSettingMainItem(settingNetwork, "网络设置", g_appWorkDir + "static/images/setting-network.svg");
    connect(settingNetwork, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_NETWORK);
            });

    settingLog = new QToolButton(settingBody);
    setSettingMainItem(settingLog, "识别记录", g_appWorkDir + "static/images/setting-log.svg");
    connect(settingLog, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_LOG);
            });

    settingFace = new QToolButton(settingBody);
    setSettingMainItem(settingFace, "人脸设置", g_appWorkDir + "static/images/setting-face.svg");
    connect(settingFace, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_FACE);
            });
    settingSystem = new QToolButton(settingBody);
    setSettingMainItem(settingSystem, "系统参数设置", g_appWorkDir + "static/images/setting-system.svg");
    connect(settingSystem, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_SYSTEM);
            });

    settingServer = new QToolButton(settingBody);
    setSettingMainItem(settingServer, "服务器设置", g_appWorkDir + "static/images/setting-server.svg");
    connect(settingServer, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_SERVER);
            });

    settingDoor = new QToolButton(settingBody);
    setSettingMainItem(settingDoor, "门禁参数设置", g_appWorkDir + "static/images/setting-door.svg");
    connect(settingDoor, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_DOOR);
            });

    settingMaintain = new QToolButton(settingBody);
    setSettingMainItem(settingMaintain, "设备维护", g_appWorkDir + "static/images/setting-maintain.png");
    connect(settingMaintain, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_MAINTAIN);
            });

    settingMainLayout->addWidget(settingInfo, 0, 0);
    settingMainLayout->addWidget(settingNetwork, 0, 1);
    settingMainLayout->addWidget(settingLog, 0, 2);

    settingMainLayout->addWidget(settingFace, 1, 0);
    settingMainLayout->addWidget(settingSystem, 1, 1);
    settingMainLayout->addWidget(settingServer, 1, 2);

    settingMainLayout->addWidget(settingDoor, 2, 0);
    settingMainLayout->addWidget(settingMaintain, 2, 1);


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(settingBody);
    mainLayout->addStretch();
}


MainSettingMain::~MainSettingMain() = default;
