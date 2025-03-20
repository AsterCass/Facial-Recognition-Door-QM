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
    cancelBtn = new QPushButton("取消", scrollContent);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backUntilPage(MAIN_PAGE_HOME);
            });


    scrollerAreaLayout->addWidget(ipWiredLabel);
    scrollerAreaLayout->addWidget(ipWirelessLabel);
    scrollerAreaLayout->addWidget(ipFourGLabel);
    scrollerAreaLayout->addWidget(saveRebootBtn);
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
