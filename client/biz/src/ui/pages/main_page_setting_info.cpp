#include "ui/pages/main_page_setting_info.h"

#include <airstrip_command.h>
#include <airstrip_log.h>
#include <airstrip_thread_pool.h>
#include <sstream>
#include <api/api.h>
#include <ui/main_router.h>


using namespace std;

std::string checkUpdateFileExtension(const std::string &filename) {
    if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".zip") == 0) {
        return ".zip";
    }
    if (filename.size() >= 7 && filename.compare(filename.size() - 7, 7, ".tar.gz") == 0) {
        return ".tar.gz";
    }
    if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".tar") == 0) {
        return ".tar";
    }
    return "";
}

void updateAppVersion(const bool confirm) {
    if (!confirm) {
        return;
    }
    logPrintln("Prepare to update ...", airstrip::INFO, __FUNCTION__);

    std::ostringstream oss;
    oss << "sh " << g_appWorkDir << "script/linux/update.sh";

    const auto suffix = checkUpdateFileExtension(g_prepareUpdateUrl);
    if (suffix.empty() || g_prepareUpdateVersion.empty()) {
        logPrintln(
            "Update not support for url: " + g_prepareUpdateUrl + " version: " + g_prepareUpdateVersion,
            airstrip::INFO, __FUNCTION__
        );
        return;
    }

    oss << " " << g_appWorkDir << "app-" << g_prepareUpdateVersion << suffix << " " << g_prepareUpdateUrl;

    logPrintln("Update command : " + oss.str(), airstrip::INFO, __FUNCTION__);
#ifndef WIN32
    airstrip::execCommandNoReturn(oss.str());
#endif
}

void checkVersionAndNotification() {
    const auto updateNotification = appUpdate();
    if (!updateNotification.isSuccessful) {
        MainRouter::getInstance()->
                mainNotificationShow("获取版本信息失败，请稍后再试",
                                     bind(updateAppVersion, false));
    } else if (!updateNotification.isNeedUpdate) {
        MainRouter::getInstance()->
                mainNotificationShow("当前版本已经是最新版本，无需升级",
                                     bind(updateAppVersion, false));
    } else {
        g_prepareUpdateVersion = updateNotification.updateVersion;
        g_prepareUpdateUrl = updateNotification.updateUrl;
        MainRouter::getInstance()->
                mainNotificationShow(
                    "检测到最新版本：" + updateNotification.updateVersion + "，是否现在升级？",
                    bind(updateAppVersion, std::placeholders::_1)
                );
    }
    g_isCheckVersion = false;
}

MainSettingInfo::MainSettingInfo(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "设备信息", nullptr);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        // deviceId
        deviceId = new QWidget(body);
        deviceIdLayout = new QHBoxLayout(deviceId);
        deviceIdLayout->setContentsMargins(20, 0, 0, 0);
        deviceIdLayout->setSpacing(20);
#ifdef WIN32
        deviceId->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        deviceId->setFixedHeight(50);
#else
        deviceId->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        deviceId->setFixedHeight(100);
#endif
        deviceIdLabel = new QLabel("设备ID", deviceId);
        deviceIdValue = new QLabel(getSn().c_str(), deviceId);
        deviceIdValue->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        deviceIdValue->setStyleSheet(R"(
                QLabel {
                        padding-right: 20px;
                }
        )");

        deviceIdLayout->addWidget(deviceIdLabel);
        deviceIdLayout->addWidget(deviceIdValue);
        bodyLayout->addWidget(deviceId);

        // ipAddressWired
        ipAddressWired = new QWidget(body);
        ipAddressWiredLayout = new QHBoxLayout(ipAddressWired);
        ipAddressWiredLayout->setContentsMargins(20, 0, 0, 0);
        ipAddressWiredLayout->setSpacing(20);
#ifdef WIN32
        ipAddressWired->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        ipAddressWired->setFixedHeight(50);
#else
        ipAddressWired->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        ipAddressWired->setFixedHeight(100);
#endif
        ipAddressWiredLabel = new QLabel("有线IP地址", ipAddressWired);
        ipAddressWiredValue = new QLabel(getSn().c_str(), ipAddressWired);
        ipAddressWiredValue->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        ipAddressWiredValue->setStyleSheet(R"(
                QLabel {
                        padding-right: 20px;
                }
        )");

        ipAddressWiredLayout->addWidget(ipAddressWiredLabel);
        ipAddressWiredLayout->addWidget(ipAddressWiredValue);
        bodyLayout->addWidget(ipAddressWired);

        // ipAddressWireless
        ipAddressWireless = new QWidget(body);
        ipAddressWirelessLayout = new QHBoxLayout(ipAddressWireless);
        ipAddressWirelessLayout->setContentsMargins(20, 0, 0, 0);
        ipAddressWirelessLayout->setSpacing(20);
#ifdef WIN32
        ipAddressWireless->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        ipAddressWireless->setFixedHeight(50);
#else
            ipAddressWireless->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            ipAddressWireless->setFixedHeight(100);
#endif
        ipAddressWirelessLabel = new QLabel("无线IP地址", ipAddressWireless);
        ipAddressWirelessValue = new QLabel(getSn().c_str(), ipAddressWireless);
        ipAddressWirelessValue->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        ipAddressWirelessValue->setStyleSheet(R"(
                QLabel {
                        padding-right: 20px;
                }
        )");

        ipAddressWirelessLayout->addWidget(ipAddressWirelessLabel);
        ipAddressWirelessLayout->addWidget(ipAddressWirelessValue);
        bodyLayout->addWidget(ipAddressWireless);

        // ipAddress4G
        ipAddress4G = new QWidget(body);
        ipAddress4GLayout = new QHBoxLayout(ipAddress4G);
        ipAddress4GLayout->setContentsMargins(20, 0, 0, 0);
        ipAddress4GLayout->setSpacing(20);
#ifdef WIN32
        ipAddress4G->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        ipAddress4G->setFixedHeight(50);
#else
            ipAddress4G->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            ipAddress4G->setFixedHeight(100);
#endif
        ipAddress4GLabel = new QLabel("4GIP地址", ipAddress4G);
        ipAddress4GValue = new QLabel(getSn().c_str(), ipAddress4G);
        ipAddress4GValue->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        ipAddress4GValue->setStyleSheet(R"(
                QLabel {
                        padding-right: 20px;
                }
        )");

        ipAddress4GLayout->addWidget(ipAddress4GLabel);
        ipAddress4GLayout->addWidget(ipAddress4GValue);
        bodyLayout->addWidget(ipAddress4G);

        // version
        version = new QWidget(body);
        versionLayout = new QHBoxLayout(version);
        versionLayout->setContentsMargins(20, 0, 0, 0);
        versionLayout->setSpacing(0);
#ifdef WIN32
        version->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        version->setFixedHeight(50);
#else
        version->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        version->setFixedHeight(100);
#endif
        versionLabel = new QLabel("当前版本号", version);
        versionLabelFlag = new QLabel("", version);
        versionLabelFlag->setEnabled(false);
        versionLabelFlag->setStyleSheet("color: red; font-size: 15px");
        versionValue = new QPushButton(getSn().c_str(), version);
        versionValue->setStyleSheet(R"(
                QPushButton {
                        text-align: right;
                        padding-right: 20px;
                        padding-left: 10px;
                        padding-top: 40px;
                        padding-bottom: 40px;
                }
                QPushButton:focus {
                        outline: none;
                }
        )");
        connect(versionValue, &QPushButton::clicked, this,
                [=] {
                    if (g_isCheckVersion) {
                        return;
                    }
                    g_isCheckVersion = true;
                    static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue([] {
                        checkVersionAndNotification();
                    });
                });

        versionLayout->addWidget(versionLabel);
        versionLayout->addStretch();
        versionLayout->addWidget(versionLabelFlag);
        versionLayout->addWidget(versionValue);
        bodyLayout->addWidget(version);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingInfo::~MainSettingInfo() = default;

void MainSettingInfo::showEvent(QShowEvent *) {
    // data
    if (deviceIdValue) {
        deviceIdValue->setText(getSn().c_str());
    }
    if (ipAddressWiredValue) {
        ipAddressWiredValue->setText(g_wiredIp.c_str());
    }
    if (ipAddressWirelessValue) {
        ipAddressWirelessValue->setText(g_wirelessIp.c_str());
    }
    if (ipAddress4GValue) {
        ipAddress4GValue->setText(g_fourGIp.c_str());
    }
    if (versionValue) {
        versionValue->setText(APP_VERSION);
    }
    if (versionLabelFlag) {
        versionLabelFlag->setText(APP_VERSION == g_latestVersion ? "" : "●");
    }

    // status
    if (g_fourGIp.empty()) {
        ipAddress4G->hide();
    } else {
        ipAddress4G->show();
    }
    if (g_wirelessIp.empty()) {
        ipAddressWireless->hide();
    } else {
        ipAddressWireless->show();
    }
    if (g_wiredIp.empty()) {
        ipAddressWired->hide();
    } else {
        ipAddressWired->show();
    }
}
