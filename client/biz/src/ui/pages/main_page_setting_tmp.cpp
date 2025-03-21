#include "ui/pages/main_page_setting_tmp.h"

#include "airstrip_log.h"
#include "ui/main_router.h"
#include "utils/global_data_manager.h"


using namespace std;

MainSettingTmp::MainSettingTmp(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainScrollArea = new QScrollArea(this);
    mainScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setObjectName("mainSettingTmpMain");
    mainScrollArea->setStyleSheet("#mainSettingTmpMain{background-color: white}");
    mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainLayout->addWidget(mainScrollArea);

    scrollContent = new QWidget(scrollContent);
    mainScrollArea->setWidget(scrollContent);

    // Content
    scrollerAreaLayout = new QVBoxLayout(scrollContent);
    scrollerAreaLayout->setSpacing(20);
    scrollerAreaLayout->setMargin(20);

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


    netModelLabel = new QLabel("网络模式：", scrollContent);
    netModelWidget = new QWidget(scrollContent);
    netModelLayout = new QHBoxLayout(netModelWidget);
    netModelWired = new QRadioButton("有线", netModelWidget);
    netModelWireless = new QRadioButton("无线", netModelWidget);
    netModelFourG = new QRadioButton("4G", netModelWidget);
    netModelLayout->addWidget(netModelWired);
    netModelLayout->addWidget(netModelWireless);
    netModelLayout->addWidget(netModelFourG);

    wifiAccountLabel = new QLabel("WIFI账号：", scrollContent);
    wifiAccount = new QLineEditPro(scrollContent);
    wifiPasswdLabel = new QLabel("WIFI密码：", scrollContent);
    wifiPasswdEdit = new QLineEditPro(scrollContent);
    wifiPasswdEdit->setEchoMode(QLineEdit::Password);

    enableLiveness = new QCheckBox("开启活体验证", scrollContent);
    lightOnlyCheck = new QCheckBox("仅在核验时开启补光灯", scrollContent);

    ipWiredLabel = new QLabel("有线IP地址：", scrollContent);
    ipWirelessLabel = new QLabel("无线IP地址：", scrollContent);
    ipFourGLabel = new QLabel("4GIP地址：", scrollContent);

    saveRebootBtn = new QPushButton("保存并重启", scrollContent);
    connect(saveRebootBtn, &QPushButton::clicked, this,
            [=] {
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


    //     bottomWidget = new QWidget(scrollContent);
    // #ifdef WIN32
    //     bottomWidget->setFixedHeight(120);
    // #else
    //     bottomWidget->setFixedHeight(240);
    // #endif

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

    scrollerAreaLayout->addWidget(enableLiveness);
    scrollerAreaLayout->addWidget(lightOnlyCheck);

    scrollerAreaLayout->addWidget(ipWiredLabel);
    scrollerAreaLayout->addWidget(ipWirelessLabel);
    scrollerAreaLayout->addWidget(ipFourGLabel);


    // scrollerAreaLayout->addWidget(bottomWidget);

    scrollerAreaLayout->addWidget(saveRebootBtn);
    scrollerAreaLayout->addWidget(checkUpdateBtn);
    scrollerAreaLayout->addWidget(cancelBtn);


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
