#include "ui/pages/main_page_setting_maintain.h"

#include <airstrip_command.h>
#include <airstrip_log.h>
#include <sstream>
#include <config/style.h>
#include <ui/main_router.h>


using namespace std;

void resetDataOperation(const bool confirm) {
    if (!confirm) {
        return;
    }
#ifdef WIN32
    logPrintln("Reset all data", airstrip::INFO, __FUNCTION__);
#else
    airstrip::execScript(g_appWorkDir + "script/linux/reset.sh");
#endif
}

MainSettingMaintain::MainSettingMaintain(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "设备维护", nullptr);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        // resetData
        resetData = new QPushButton("恢复出场设置", body);
#ifdef WIN32
        resetData->setStyleSheet(
            "text-align: left; padding-left: 20px; background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: rgb(255, 71, 60)");
        resetData->setFixedHeight(50);
#else
        resetData->setStyleSheet("text-align: left; padding-left: 20px; background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: rgb(255, 71, 60)");
        resetData->setFixedHeight(100);
#endif
        connect(resetData, &QPushButton::clicked, this,
                [=] {
                    MainRouter::getInstance()->mainNotificationShow(
                        "确定要恢复出厂设置吗？", bind(resetDataOperation, std::placeholders::_1));
                });
        bodyLayout->addWidget(resetData);


        // reboot
        reboot = new QPushButton("重启设备", body);
#ifdef WIN32
        reboot->setStyleSheet(
            "text-align: left; padding-left: 20px; background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: rgb(13, 133, 255)");
        reboot->setFixedHeight(50);
#else
        reboot->setStyleSheet("text-align: left; padding-left: 20px; background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: rgb(13, 133, 255)");
        reboot->setFixedHeight(100);
#endif
        connect(reboot, &QPushButton::clicked, this,
                [=] {
#ifdef WIN32
                    logPrintln("Reboot ...", airstrip::INFO, __FUNCTION__);
#else
                        airstrip::execScript(g_appWorkDir + "script/linux/reboot_app.sh");
#endif
                });
        bodyLayout->addWidget(reboot);

        // cronReboot
        cronReboot = new QWidget(body);
        cronRebootLayout = new QHBoxLayout(cronReboot);
        cronRebootLayout->setContentsMargins(20, 0, 0, 0);
        cronRebootLayout->setSpacing(20);
#ifdef WIN32
        cronReboot->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        cronReboot->setFixedHeight(50);
#else
        cronReboot->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        cronReboot->setFixedHeight(100);
#endif
        cronRebootLabel = new QLabel("定时重启（每天凌晨4点）", cronReboot);
        cronRebootInput = new QPushButton("⬤", cronReboot);
        cronRebootInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
        connect(cronRebootInput, &QPushButton::clicked, this,
                [=] {
                    if (cronRebootValue) {
                        cronRebootValue = 0;
                        cronRebootInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
#ifdef WIN32
                        logPrintln("Cron reboot off ...", airstrip::INFO, __FUNCTION__);
#else
                        airstrip::execScript(g_appWorkDir + "script/linux/cron_auto_reboot.sh off");
#endif
                    } else {
                        cronRebootValue = 1;
                        cronRebootInput->setStyleSheet(SWITCH_BUTTON_ENABLE_STYLE);
#ifdef WIN32
                        logPrintln("Cron reboot on ...", airstrip::INFO, __FUNCTION__);
#else
                        airstrip::execScript(g_appWorkDir + "script/linux/cron_auto_reboot.sh on");
#endif
                    }
                });

        cronRebootLayout->addWidget(cronRebootLabel);
        cronRebootLayout->addStretch();
        cronRebootLayout->addWidget(cronRebootInput);
        bodyLayout->addWidget(cronReboot);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingMaintain::~MainSettingMaintain() = default;

void MainSettingMaintain::showEvent(QShowEvent *) {
    // data
    if (cronRebootInput) {
        cronRebootInput->setStyleSheet(g_cronReboot ? SWITCH_BUTTON_ENABLE_STYLE : SWITCH_BUTTON_DISABLE_STYLE);
        cronRebootValue = g_cronReboot;
    }
}
