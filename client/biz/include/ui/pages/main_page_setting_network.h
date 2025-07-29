#ifndef MAIN_PAGE_SETTING_NETWORK_H
#define MAIN_PAGE_SETTING_NETWORK_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingNetwork final : public QWidget {
public:
    explicit MainSettingNetwork(QWidget *parent = nullptr);

    ~MainSettingNetwork() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QPushButton *save = nullptr;

    QWidget *wired = nullptr;
    QHBoxLayout *wiredLayout = nullptr;
    QLabel *wiredLabel = nullptr;
    QPushButton *wiredInput = nullptr;
    int showMoreWiredConfig = 0;

    QWidget *wiredDhcp = nullptr;
    QHBoxLayout *wiredDhcpLayout = nullptr;
    QLabel *wiredDhcpLabel = nullptr;
    QPushButton *wiredDhcpInput = nullptr;
    int wiredDhcpValue = 0;
    QLabel *wiredDhcpTips = nullptr;

    QWidget *wiredIp = nullptr;
    QHBoxLayout *wiredIpLayout = nullptr;
    QLabel *wiredIpLabel = nullptr;
    QLineEditPro *wiredIpInput = nullptr;

    QWidget *wiredMask = nullptr;
    QHBoxLayout *wiredMaskLayout = nullptr;
    QLabel *wiredMaskLabel = nullptr;
    QLineEditPro *wiredMaskInput = nullptr;

    QWidget *wiredGateway = nullptr;
    QHBoxLayout *wiredGatewayLayout = nullptr;
    QLabel *wiredGatewayLabel = nullptr;
    QLineEditPro *wiredGatewayInput = nullptr;

    QWidget *wireless = nullptr;
    QHBoxLayout *wirelessLayout = nullptr;
    QLabel *wirelessLabel = nullptr;
    QPushButton *wirelessInput = nullptr;
    int showMoreWirelessConfig = 0;

    QWidget *wirelessEnable = nullptr;
    QHBoxLayout *wirelessEnableLayout = nullptr;
    QLabel *wirelessEnableLabel = nullptr;
    QPushButton *wirelessEnableInput = nullptr;
    int wirelessEnableValue = 0;

    QWidget *wirelessSsid = nullptr;
    QHBoxLayout *wirelessSsidLayout = nullptr;
    QLabel *wirelessSsidLabel = nullptr;
    QLineEditPro *wirelessSsidInput = nullptr;

    QWidget *wirelessPasswd = nullptr;
    QHBoxLayout *wirelessPasswdLayout = nullptr;
    QLabel *wirelessPasswdLabel = nullptr;
    QLineEditPro *wirelessPasswdInput = nullptr;

    QWidget *fourG = nullptr;
    QHBoxLayout *fourGLayout = nullptr;
    QLabel *fourGLabel = nullptr;
    QPushButton *fourGInput = nullptr;
    int showMoreFourGConfig = 0;

    QWidget *fourGEnable = nullptr;
    QHBoxLayout *fourGEnableLayout = nullptr;
    QLabel *fourGEnableLabel = nullptr;
    QPushButton *fourGEnableInput = nullptr;
    int fourGEnableValue = 0;

    QWidget *dns1 = nullptr;
    QHBoxLayout *dns1Layout = nullptr;
    QLabel *dns1Label = nullptr;
    QLineEditPro *dns1Input = nullptr;

    QWidget *dns2 = nullptr;
    QHBoxLayout *dns2Layout = nullptr;
    QLabel *dns2Label = nullptr;
    QLineEditPro *dns2Input = nullptr;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_NETWORK_H
