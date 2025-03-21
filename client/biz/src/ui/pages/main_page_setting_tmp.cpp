#include "ui/pages/main_page_setting_tmp.h"

#include "airstrip_log.h"
#include "ui/main_router.h"
#include "utils/global_data_manager.h"
#include <sstream>
#include <boost/json.hpp>

#include "airstrip_command.h"
#include "config/config.h"


using namespace std;

MainSettingTmp::MainSettingTmp(QWidget *parent): QWidget(parent) {
    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainScrollArea = new QScrollArea(this);
    mainScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setObjectName("mainSettingTmpMain");
    mainScrollArea->setStyleSheet("#mainSettingTmpMain{background-color: white}");
    mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef WIN32
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 15px;
        background: #f0f0f0;
    }

    QScrollBar::handle:vertical {
        background: #666666;
    }

    QScrollBar::handle:vertical:hover {
        background: #333333;
    }

)");
#else
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 30px;
        background: #f0f0f0;
    }

    QScrollBar::handle:vertical {
        background: #666666;
    }

    QScrollBar::handle:vertical:hover {
        background: #333333;
    }

)");
#endif

    mainLayout->addWidget(mainScrollArea);

    scrollContent = new QWidget(mainScrollArea);
    mainScrollArea->setWidget(scrollContent);

    // Content
    scrollerAreaLayout = new QVBoxLayout(scrollContent);
    scrollerAreaLayout->setSpacing(20);
    scrollerAreaLayout->setMargin(20);

    serverAddressLabel = new QLabel("服务器地址：", scrollContent);
    serverAddress = new QLineEditPro(scrollContent);
    faceThresholdLabel = new QLabel("人脸识别阈值（0 - 0.6）（推荐 0.48）：", scrollContent);
    faceThreshold = new QLineEditPro(scrollContent);
    volLabel = new QLabel("设备音量（0 - 100）：", scrollContent);
    vol = new QLineEditPro(scrollContent);
    faceDistantLabel = new QLabel("人脸识别距离：", scrollContent);;
    faceDistantWidget = new QWidget(scrollContent);
    faceDistantLayout = new QHBoxLayout(faceDistantWidget);
    faceDistantLess = new QRadioButton("近", faceDistantWidget);
    faceDistantMid = new QRadioButton("中", faceDistantWidget);
    faceDistantMore = new QRadioButton("远", faceDistantWidget);
    faceDistantLayout->addWidget(faceDistantLess);
    faceDistantLayout->addWidget(faceDistantMid);
    faceDistantLayout->addWidget(faceDistantMore);
    faceDistantGroup = new QButtonGroup(faceDistantWidget);
    faceDistantGroup->addButton(faceDistantLess, 1);
    faceDistantGroup->addButton(faceDistantMid, 2);
    faceDistantGroup->addButton(faceDistantMore, 3);


    netModelLabel = new QLabel("网络模式：", scrollContent);
    netModelWidget = new QWidget(scrollContent);
    netModelLayout = new QHBoxLayout(netModelWidget);
    netModelWired = new QRadioButton("有线", netModelWidget);
    netModelWireless = new QRadioButton("无线", netModelWidget);
    netModelFourG = new QRadioButton("4G", netModelWidget);
    netModelLayout->addWidget(netModelWired);
    netModelLayout->addWidget(netModelWireless);
    netModelLayout->addWidget(netModelFourG);
    netModelGroup = new QButtonGroup(faceDistantWidget);
    netModelGroup->addButton(netModelWired, 1);
    netModelGroup->addButton(netModelWireless, 2);
    netModelGroup->addButton(netModelFourG, 3);

    wifiAccountLabel = new QLabel("WIFI账号：", scrollContent);
    wifiAccount = new QLineEditPro(scrollContent);
    wifiPasswdLabel = new QLabel("WIFI密码：", scrollContent);
    wifiPasswdEdit = new QLineEditPro(scrollContent);
    wifiPasswdEdit->setEchoMode(QLineEdit::Password);

    enableFaceSpoof = new QCheckBox("开启活体验证", scrollContent);
    lightOnlyCheck = new QCheckBox("仅在核验时开启补光灯", scrollContent);

    ipWiredLabel = new QLabel("有线IP地址：", scrollContent);
    ipWirelessLabel = new QLabel("无线IP地址：", scrollContent);
    ipFourGLabel = new QLabel("4GIP地址：", scrollContent);

    saveRebootBtn = new QPushButton("保存并重启", scrollContent);
    connect(saveRebootBtn, &QPushButton::clicked, this,
            [=] {
                try {
                    // todo if not equal save db

                    g_serverAddress = serverAddress->text().trimmed().toStdString();
                    g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, g_serverAddress);

                    g_faceThreshold = faceThreshold->text().trimmed().toDouble();
                    g_commonDb.upsertConfig(PRO_DB_FACE_THRESHOLD, to_string(g_faceThreshold));

                    g_volNum = vol->text().trimmed().toInt();
                    g_commonDb.upsertConfig(PRO_DB_VOL_NUM, to_string(g_volNum));

                    g_faceDistance = faceDistantGroup->checkedId();
                    g_commonDb.upsertConfig(PRO_DB_FACE_DISTANCE, to_string(g_faceDistance));

                    g_netModel = netModelGroup->checkedId();
                    g_commonDb.upsertConfig(PRO_DB_NET_MODEL, to_string(g_netModel));

                    g_wifiAccount = wifiAccount->text().trimmed().toStdString();
                    g_commonDb.upsertConfig(PRO_DB_WIFI_ACCOUNT, g_wifiAccount);

                    g_wifiPasswd = wifiPasswdEdit->text().trimmed().toStdString();
                    g_commonDb.upsertConfig(PRO_DB_WIFI_PASSWD, g_wifiPasswd);

                    g_enableFaceSpoof = enableFaceSpoof->isChecked() ? 1 : 0;
                    g_commonDb.upsertConfig(PRO_DB_ENABLE_FACE_SPOOF, to_string(g_enableFaceSpoof));

                    g_lightOnlyCheck = lightOnlyCheck->isChecked() ? 1 : 0;
                    g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_ONLY_CHECK, to_string(g_lightOnlyCheck));
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                }
#ifdef WIN32
                logPrintln("Reboot ...", airstrip::INFO, __FUNCTION__);
#else
                airstrip::execScript(g_appWorkDir + "script/linux/reboot_app.sh");
#endif
            });
    checkUpdateBtn = new QPushButton("检查更新", scrollContent);
    connect(checkUpdateBtn, &QPushButton::clicked, this,
            [=] {
#ifdef WIN32
                logPrintln("Update ...", airstrip::INFO, __FUNCTION__);
#else
                //todo
#endif
            });
    cancelBtn = new QPushButton("取消", scrollContent);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backUntilPage(MAIN_PAGE_HOME);
            });


    scrollerAreaLayout->addWidget(serverAddressLabel);
    scrollerAreaLayout->addWidget(serverAddress);
    scrollerAreaLayout->addWidget(faceThresholdLabel);
    scrollerAreaLayout->addWidget(faceThreshold);
    scrollerAreaLayout->addWidget(volLabel);
    scrollerAreaLayout->addWidget(vol);
    scrollerAreaLayout->addWidget(faceDistantLabel);
    scrollerAreaLayout->addWidget(faceDistantWidget);


    scrollerAreaLayout->addWidget(netModelLabel);
    scrollerAreaLayout->addWidget(netModelWidget);
    scrollerAreaLayout->addWidget(wifiAccountLabel);
    scrollerAreaLayout->addWidget(wifiAccount);
    scrollerAreaLayout->addWidget(wifiPasswdLabel);
    scrollerAreaLayout->addWidget(wifiPasswdEdit);

    scrollerAreaLayout->addWidget(enableFaceSpoof);
    scrollerAreaLayout->addWidget(lightOnlyCheck);

    scrollerAreaLayout->addWidget(ipWiredLabel);
    scrollerAreaLayout->addWidget(ipWirelessLabel);
    scrollerAreaLayout->addWidget(ipFourGLabel);

    scrollerAreaLayout->addWidget(saveRebootBtn);
    scrollerAreaLayout->addWidget(checkUpdateBtn);
    scrollerAreaLayout->addWidget(cancelBtn);

    // Data
    serverAddress->setText(QString::fromStdString(g_serverAddress));
    faceThreshold->setText(QString::number(g_faceThreshold));
    vol->setText(QString::number(g_volNum));
    faceDistantGroup->button(g_faceDistance)->setChecked(true);
    enableFaceSpoof->setCheckState(g_enableFaceSpoof ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    lightOnlyCheck->setCheckState(g_lightOnlyCheck ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    netModelGroup->button(g_netModel)->setChecked(true);
    wifiAccount->setText(QString::fromStdString(g_wifiAccount));
    wifiPasswdEdit->setText(QString::fromStdString(g_wifiPasswd));


    // Connect
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWiredChange,
            this, [=](const string &ip) {
                const auto ipFull = "有线IP地址：" + ip;
                if (nullptr != ipWiredLabel) {
                    ipWiredLabel->setText(QString::fromStdString(ipFull));
                }
            });
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWirelessChange,
            this, [=](const string &ip) {
                const auto ipFull = "无线IP地址：" + ip;
                if (nullptr != ipWirelessLabel) {
                    ipWirelessLabel->setText(QString::fromStdString(ipFull));
                }
            });

    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerFourGChange,
            this, [=](const string &ip) {
                const auto ipFull = "4GIP地址：" + ip;
                if (nullptr != ipFourGLabel) {
                    ipFourGLabel->setText(QString::fromStdString(ipFull));
                }
            });
}


MainSettingTmp::~MainSettingTmp() = default;
