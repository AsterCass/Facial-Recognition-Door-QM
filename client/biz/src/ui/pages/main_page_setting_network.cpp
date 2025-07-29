#include "ui/pages/main_page_setting_network.h"

#include <airstrip_log.h>
#include <airstrip_thread_pool.h>
#include <sstream>
#include <api/api.h>
#include <config/style.h>
#include <ui/main_router.h>


using namespace std;


MainSettingNetwork::MainSettingNetwork(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // save
    save = new QPushButton("保存");
    connect(save, &QPushButton::clicked, this,
            [=] {
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "网络设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        // wired
        wired = new QWidget(body);
        wiredLayout = new QHBoxLayout(wired);
        wiredLayout->setContentsMargins(20, 0, 0, 0);
        wiredLayout->setSpacing(20);
#ifdef WIN32
        wired->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        wired->setFixedHeight(50);
#else
        wired->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        wired->setFixedHeight(100);
#endif
        wiredLabel = new QLabel("有线网络", wired);
        wiredInput = new QPushButton("▼", wired);
        wiredInput->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#ifdef WIN32
        wiredInput->setStyleSheet("QPushButton { padding-right: 20px; text-align: right; font-size: 10px;} "
            "QPushButton:focus { outline: none;}");
#else
        wiredInput->setStyleSheet("QPushButton { padding-right: 40px; text-align: right; font-size: 20px;} "
                                  "QPushButton:focus { outline: none;}");
#endif
        connect(wiredInput, &QPushButton::clicked, this,
                [=] {
                    showMoreWiredConfig = !showMoreWiredConfig;
                    if (showMoreWiredConfig) {
                        wiredInput->setText("▲");
                        wiredDhcp->show();
                        if (wiredDhcpValue) {
                            wiredIp->show();
                            wiredMask->show();
                            wiredGateway->show();
                        } else {
                            wiredIp->hide();
                            wiredMask->hide();
                            wiredGateway->hide();
                        }
                    } else {
                        wiredInput->setText("▼");
                        wiredDhcp->hide();
                        wiredIp->hide();
                        wiredMask->hide();
                        wiredGateway->hide();
                    }
                });
        wiredLayout->addWidget(wiredLabel);
        wiredLayout->addWidget(wiredInput);
        bodyLayout->addWidget(wired);

        // for wired
        {
            wiredDhcp = new QWidget(body);
            wiredDhcpLayout = new QHBoxLayout(wiredDhcp);
            wiredDhcpLayout->setContentsMargins(20, 0, 0, 0);
            wiredDhcpLayout->setSpacing(20);
#ifdef WIN32
            wiredDhcp->setStyleSheet(
                "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
            wiredDhcp->setFixedHeight(50);
#else
                wiredDhcp->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 60px");
                wiredDhcp->setFixedHeight(100);
#endif
            wiredDhcpLabel = new QLabel("DHCP", wiredDhcp);
            wiredDhcpInput = new QPushButton("⬤", wiredDhcp);
            wiredDhcpInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
            connect(wiredDhcpInput, &QPushButton::clicked, this,
                    [=] {
                        wiredDhcpValue = wiredDhcpValue ? 0 : 1;
                        wiredDhcpInput->setStyleSheet(wiredDhcpValue
                                                          ? SWITCH_BUTTON_ENABLE_STYLE
                                                          : SWITCH_BUTTON_DISABLE_STYLE);
                        if (wiredDhcpValue) {
                            wiredIp->show();
                            wiredMask->show();
                            wiredGateway->show();
                        } else {
                            wiredIp->hide();
                            wiredMask->hide();
                            wiredGateway->hide();
                        }
                    });

            wiredDhcpLayout->addWidget(wiredDhcpLabel);
            wiredDhcpLayout->addStretch();
            wiredDhcpLayout->addWidget(wiredDhcpInput);
            bodyLayout->addWidget(wiredDhcp);

            // for dhcp
            {
                // wiredIp
                wiredIp = new QWidget(body);
                wiredIpLayout = new QHBoxLayout(wiredIp);
                wiredIpLayout->setContentsMargins(20, 0, 0, 0);
                wiredIpLayout->setSpacing(20);
#ifdef WIN32
                wiredIp->setStyleSheet(
                    "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
                wiredIp->setFixedHeight(50);
#else
            wiredIp->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 30px");
            wiredIp->setFixedHeight(100);
#endif
                wiredIpLabel = new QLabel("IP地址", wiredIp);
                wiredIpInput = new QLineEditPro(wiredIp);
                wiredIpInput->setAlignment(Qt::AlignRight);
                wiredIpInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                wiredIpInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
                wiredIpLayout->addWidget(wiredIpLabel);
                wiredIpLayout->addWidget(wiredIpInput);
                bodyLayout->addWidget(wiredIp);


                // wiredMask
                wiredMask = new QWidget(body);
                wiredMaskLayout = new QHBoxLayout(wiredMask);
                wiredMaskLayout->setContentsMargins(20, 0, 0, 0);
                wiredMaskLayout->setSpacing(20);
#ifdef WIN32
                wiredMask->setStyleSheet(
                    "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
                wiredMask->setFixedHeight(50);
#else
            wiredMask->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 30px");
            wiredMask->setFixedHeight(100);
#endif
                wiredMaskLabel = new QLabel("子网掩码", wiredMask);
                wiredMaskInput = new QLineEditPro(wiredMask);
                wiredMaskInput->setAlignment(Qt::AlignRight);
                wiredMaskInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                wiredMaskInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
                wiredMaskLayout->addWidget(wiredMaskLabel);
                wiredMaskLayout->addWidget(wiredMaskInput);
                bodyLayout->addWidget(wiredMask);


                // wiredGateway
                wiredGateway = new QWidget(body);
                wiredGatewayLayout = new QHBoxLayout(wiredGateway);
                wiredGatewayLayout->setContentsMargins(20, 0, 0, 0);
                wiredGatewayLayout->setSpacing(20);
#ifdef WIN32
                wiredGateway->setStyleSheet(
                    "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
                wiredGateway->setFixedHeight(50);
#else
            wiredGateway->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 30px");
            wiredGateway->setFixedHeight(100);
#endif
                wiredGatewayLabel = new QLabel("网关", wiredGateway);
                wiredGatewayInput = new QLineEditPro(wiredGateway);
                wiredGatewayInput->setAlignment(Qt::AlignRight);
                wiredGatewayInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                wiredGatewayInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
                wiredGatewayLayout->addWidget(wiredGatewayLabel);
                wiredGatewayLayout->addWidget(wiredGatewayInput);
                bodyLayout->addWidget(wiredGateway);
            }
        }


        // wireless
        wireless = new QWidget(body);
        wirelessLayout = new QHBoxLayout(wireless);
        wirelessLayout->setContentsMargins(20, 0, 0, 0);
        wirelessLayout->setSpacing(20);
#ifdef WIN32
        wireless->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        wireless->setFixedHeight(50);
#else
        wireless->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        wireless->setFixedHeight(100);
#endif
        wirelessLabel = new QLabel("WLAN", wireless);
        wirelessInput = new QPushButton("▼", wireless);
        wirelessInput->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#ifdef WIN32
        wirelessInput->setStyleSheet("QPushButton { padding-right: 20px; text-align: right; font-size: 10px;} "
            "QPushButton:focus { outline: none;}");
#else
        wirelessInput->setStyleSheet("QPushButton { padding-right: 40px; text-align: right; font-size: 20px;} "
                                  "QPushButton:focus { outline: none;}");
#endif
        connect(wirelessInput, &QPushButton::clicked, this,
                [=] {
                    showMoreWirelessConfig = !showMoreWirelessConfig;
                    if (showMoreWirelessConfig) {
                        wirelessInput->setText("▲");
                        wirelessEnable->show();
                        if (wirelessEnableValue) {
                            wirelessSsid->show();
                            wirelessPasswd->show();
                        } else {
                            wirelessSsid->hide();
                            wirelessPasswd->hide();
                        }
                    } else {
                        wiredInput->setText("▼");
                        wirelessEnable->hide();
                        wirelessSsid->hide();
                        wirelessPasswd->hide();
                    }
                });
        wirelessLayout->addWidget(wirelessLabel);
        wirelessLayout->addWidget(wirelessInput);
        bodyLayout->addWidget(wireless);

        // for wireless
        {
            wirelessEnable = new QWidget(body);
            wirelessEnableLayout = new QHBoxLayout(wirelessEnable);
            wirelessEnableLayout->setContentsMargins(20, 0, 0, 0);
            wirelessEnableLayout->setSpacing(20);
#ifdef WIN32
            wirelessEnable->setStyleSheet(
                "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
            wirelessEnable->setFixedHeight(50);
#else
            wirelessEnable->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 60px");
            wirelessEnable->setFixedHeight(100);
#endif
            wirelessEnableLabel = new QLabel("开启", wirelessEnable);
            wirelessEnableInput = new QPushButton("⬤", wirelessEnable);
            wirelessEnableInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
            connect(wirelessEnableInput, &QPushButton::clicked, this,
                    [=] {
                        wirelessEnableValue = wirelessEnableValue ? 0 : 1;
                        wirelessEnableInput->setStyleSheet(wirelessEnableValue
                                                               ? SWITCH_BUTTON_ENABLE_STYLE
                                                               : SWITCH_BUTTON_DISABLE_STYLE);
                        if (wirelessEnableValue) {
                            wirelessSsid->show();
                            wirelessPasswd->show();
                        } else {
                            wirelessSsid->hide();
                            wirelessPasswd->hide();
                        }
                    });

            wirelessEnableLayout->addWidget(wirelessEnableLabel);
            wirelessEnableLayout->addStretch();
            wirelessEnableLayout->addWidget(wirelessEnableInput);
            bodyLayout->addWidget(wirelessEnable);

            // for wirelessEnable
            {
                // wirelessSsid
                wirelessSsid = new QWidget(body);
                wirelessSsidLayout = new QHBoxLayout(wirelessSsid);
                wirelessSsidLayout->setContentsMargins(20, 0, 0, 0);
                wirelessSsidLayout->setSpacing(20);
#ifdef WIN32
                wirelessSsid->setStyleSheet(
                    "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
                wirelessSsid->setFixedHeight(50);
#else
            wirelessSsid->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 30px");
            wirelessSsid->setFixedHeight(100);
#endif
                wirelessSsidLabel = new QLabel("网络名称", wirelessSsid);
                wirelessSsidInput = new QLineEditPro(wirelessSsid);
                wirelessSsidInput->setAlignment(Qt::AlignRight);
                wirelessSsidInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                wirelessSsidInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
                wirelessSsidLayout->addWidget(wirelessSsidLabel);
                wirelessSsidLayout->addWidget(wirelessSsidInput);
                bodyLayout->addWidget(wirelessSsid);


                // wirelessPasswd
                wirelessPasswd = new QWidget(body);
                wirelessPasswdLayout = new QHBoxLayout(wirelessPasswd);
                wirelessPasswdLayout->setContentsMargins(20, 0, 0, 0);
                wirelessPasswdLayout->setSpacing(20);
#ifdef WIN32
                wirelessPasswd->setStyleSheet(
                    "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
                wirelessPasswd->setFixedHeight(50);
#else
            wirelessPasswd->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 30px");
            wirelessPasswd->setFixedHeight(100);
#endif
                wirelessPasswdLabel = new QLabel("网络密码", wirelessPasswd);
                wirelessPasswdInput = new QLineEditPro(wirelessPasswd);
                wirelessPasswdInput->setEchoMode(QLineEdit::Password);
                wirelessPasswdInput->setAlignment(Qt::AlignRight);
                wirelessPasswdInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                wirelessPasswdInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
                wirelessPasswdLayout->addWidget(wirelessPasswdLabel);
                wirelessPasswdLayout->addWidget(wirelessPasswdInput);
                bodyLayout->addWidget(wirelessPasswd);
            }
        }


        // fourG
        fourG = new QWidget(body);
        fourGLayout = new QHBoxLayout(fourG);
        fourGLayout->setContentsMargins(20, 0, 0, 0);
        fourGLayout->setSpacing(20);
#ifdef WIN32
        fourG->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        fourG->setFixedHeight(50);
#else
        fourG->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        fourG->setFixedHeight(100);
#endif
        fourGLabel = new QLabel("4G", fourG);
        fourGInput = new QPushButton("▼", fourG);
        fourGInput->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#ifdef WIN32
        fourGInput->setStyleSheet("QPushButton { padding-right: 20px; text-align: right; font-size: 10px;} "
            "QPushButton:focus { outline: none;}");
#else
        fourGInput->setStyleSheet("QPushButton { padding-right: 40px; text-align: right; font-size: 20px;} "
                                  "QPushButton:focus { outline: none;}");
#endif
        connect(fourGInput, &QPushButton::clicked, this,
                [=] {
                    showMoreFourGConfig = !showMoreFourGConfig;
                    if (showMoreFourGConfig) {
                        fourGInput->setText("▲");
                        fourGEnable->show();
                    } else {
                        wiredInput->setText("▼");
                        fourGEnable->hide();
                    }
                });
        fourGLayout->addWidget(fourGLabel);
        fourGLayout->addWidget(fourGInput);
        bodyLayout->addWidget(fourG);

        // for fourG
        {
            fourGEnable = new QWidget(body);
            fourGEnableLayout = new QHBoxLayout(fourGEnable);
            fourGEnableLayout->setContentsMargins(20, 0, 0, 0);
            fourGEnableLayout->setSpacing(20);
#ifdef WIN32
            fourGEnable->setStyleSheet(
                "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white; margin-left: 30px");
            fourGEnable->setFixedHeight(50);
#else
            fourGEnable->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white; margin-left: 60px");
            fourGEnable->setFixedHeight(100);
#endif
            fourGEnableLabel = new QLabel("开启", fourGEnable);
            fourGEnableInput = new QPushButton("⬤", fourGEnable);
            fourGEnableInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
            connect(fourGEnableInput, &QPushButton::clicked, this,
                    [=] {
                        fourGEnableValue = fourGEnableValue ? 0 : 1;
                        fourGEnableInput->setStyleSheet(fourGEnableValue
                                                            ? SWITCH_BUTTON_ENABLE_STYLE
                                                            : SWITCH_BUTTON_DISABLE_STYLE);
                    });

            fourGEnableLayout->addWidget(fourGEnableLabel);
            fourGEnableLayout->addStretch();
            fourGEnableLayout->addWidget(fourGEnableInput);
            bodyLayout->addWidget(fourGEnable);
        }


        // dns1
        dns1 = new QWidget(body);
        dns1Layout = new QHBoxLayout(dns1);
        dns1Layout->setContentsMargins(20, 0, 0, 0);
        dns1Layout->setSpacing(20);
#ifdef WIN32
        dns1->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        dns1->setFixedHeight(50);
#else
        dns1->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        dns1->setFixedHeight(100);
#endif
        dns1Label = new QLabel("DNS1", dns1);
        dns1Input = new QLineEditPro(dns1);
        dns1Input->setAlignment(Qt::AlignRight);
        dns1Input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        dns1Input->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
        dns1Layout->addWidget(dns1Label);
        dns1Layout->addWidget(dns1Input);
        bodyLayout->addWidget(dns1);

        // dns2
        dns2 = new QWidget(body);
        dns2Layout = new QHBoxLayout(dns2);
        dns2Layout->setContentsMargins(20, 0, 0, 0);
        dns2Layout->setSpacing(20);
#ifdef WIN32
        dns2->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        dns2->setFixedHeight(50);
#else
        dns2->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        dns2->setFixedHeight(100);
#endif
        dns2Label = new QLabel("DNS2", dns2);
        dns2Input = new QLineEditPro(dns2);
        dns2Input->setAlignment(Qt::AlignRight);
        dns2Input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        dns2Input->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
        dns2Layout->addWidget(dns2Label);
        dns2Layout->addWidget(dns2Input);
        bodyLayout->addWidget(dns2);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingNetwork::~MainSettingNetwork() = default;

void MainSettingNetwork::showEvent(QShowEvent *) {
    wiredInput->setText("▼");
    wiredDhcp->hide();
    wiredIp->hide();
    wiredMask->hide();
    wiredGateway->hide();

    wirelessInput->setText("▼");
    wirelessEnable->hide();
    wirelessSsid->hide();
    wirelessPasswd->hide();

    fourGInput->setText("▼");
    fourGEnable->hide();

    // data
}
