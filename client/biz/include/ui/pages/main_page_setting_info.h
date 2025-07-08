#ifndef MAIN_PAGE_SETTING_INFO_H
#define MAIN_PAGE_SETTING_INFO_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingInfo final : public QWidget {
public:
    explicit MainSettingInfo(QWidget *parent = nullptr);

    ~MainSettingInfo() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QWidget *deviceId = nullptr;
    QHBoxLayout *deviceIdLayout = nullptr;
    QLabel *deviceIdLabel = nullptr;
    QLabel *deviceIdValue = nullptr;

    QWidget *ipAddressWired = nullptr;
    QHBoxLayout *ipAddressWiredLayout = nullptr;
    QLabel *ipAddressWiredLabel = nullptr;
    QLabel *ipAddressWiredValue = nullptr;

    QWidget *ipAddressWireless = nullptr;
    QHBoxLayout *ipAddressWirelessLayout = nullptr;
    QLabel *ipAddressWirelessLabel = nullptr;
    QLabel *ipAddressWirelessValue = nullptr;

    QWidget *ipAddress4G = nullptr;
    QHBoxLayout *ipAddress4GLayout = nullptr;
    QLabel *ipAddress4GLabel = nullptr;
    QLabel *ipAddress4GValue = nullptr;

    QWidget *version = nullptr;
    QHBoxLayout *versionLayout = nullptr;
    QLabel *versionLabel = nullptr;
    QLabel *versionLabelFlag = nullptr;
    QPushButton *versionValue = nullptr;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_INFO_H
