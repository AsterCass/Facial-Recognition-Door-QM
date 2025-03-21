#include "ui/pages/main_page_setting_tmp.h"

#include "airstrip_log.h"
#include "ui/main_router.h"
#include "utils/global_data_manager.h"
#include <sstream>
#include <boost/json.hpp>

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
#ifdef WIN32
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 15px;
    }

    QScrollBar::handle:vertical {
        background: #111111;
    }

    QScrollBar::handle:vertical:hover {
        background: #000000;
    }
)");
#else
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 30px;
    }

    QScrollBar::handle:vertical {
        background: #111111;
    }

    QScrollBar::handle:vertical:hover {
        background: #000000;
    }
)");
#endif

    mainLayout->addWidget(mainScrollArea);

    scrollContent = new QWidget(scrollContent);
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
    vol = new QLineEditPro(vol);
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
                    // Address
                    g_serverAddress = serverAddress->text().trimmed().toStdString();
                    g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, g_serverAddress);
                    // Common
                    boost::json::object commonDataJson;
                    commonDataJson[PRO_DB_FACE_THRESHOLD] = faceThreshold->text().trimmed().toDouble();
                    commonDataJson[PRO_DB_VOL_NUM] = vol->text().trimmed().toInt();
                    commonDataJson[PRO_DB_FACE_DISTANCE] = faceDistantGroup->checkedId();
                    commonDataJson[PRO_DB_NET_MODEL] = netModelGroup->checkedId();
                    commonDataJson[PRO_DB_WIFI_ACCOUNT] = wifiAccount->text().trimmed().toStdString();
                    commonDataJson[PRO_DB_WIFI_PASSWD] = wifiPasswdEdit->text().trimmed().toStdString();
                    commonDataJson[PRO_DB_ENABLE_FACE_SPOOF] = enableFaceSpoof->isChecked() ? 1 : 0;
                    commonDataJson[PRO_DB_ENABLE_LIGHT_ONLY_CHECK] = lightOnlyCheck->isChecked() ? 1 : 0;
                    g_commonData = serialize(commonDataJson);
                    g_commonDb.upsertConfig(PRO_DB_COMMON_DATA, g_commonData);
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                }
#ifdef WIN32
                logPrintln("Reboot ...", airstrip::INFO, __FUNCTION__);
#else
                //todo
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
    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_FACE_THRESHOLD).as_double();
        faceThreshold->setText(QString::number(data));
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load serverAddress data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_VOL_NUM).as_int64();
        vol->setText(QString::number(data));
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load vol data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_FACE_DISTANCE).as_int64();
        faceDistantGroup->button(data)->setChecked(true);
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load faceDistantGroup data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_ENABLE_FACE_SPOOF).as_int64();
        enableFaceSpoof->setCheckState(data ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load enableFaceSpoof data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_ENABLE_LIGHT_ONLY_CHECK).as_int64();
        lightOnlyCheck->setCheckState(data ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load lightOnlyCheck data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_NET_MODEL).as_int64();
        netModelGroup->button(data)->setChecked(true);
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load netModelGroup data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_WIFI_ACCOUNT).as_string();
        wifiAccount->setText(QString::fromStdString(data.data()));
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load wifiAccount data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }

    try {
        auto commonDataJson = boost::json::parse(g_commonData);
        auto data = commonDataJson.at(PRO_DB_WIFI_PASSWD).as_string();
        wifiPasswdEdit->setText(QString::fromStdString(data.data()));
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << "Load wifiPasswdEdit data error :" << e.what();
        logPrintln(errMsg.str(), airstrip::WARN, __FUNCTION__);
    }


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
