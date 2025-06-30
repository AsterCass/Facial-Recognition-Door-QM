#include "ui/pages/main_page_setting_server.h"

#include <airstrip_log.h>
#include <sstream>
#include <ui/main_router.h>


using namespace std;

MainSettingServer::MainSettingServer(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // save
    save = new QPushButton("保存");
    connect(save, &QPushButton::clicked, this,
            [=] {
                try {
                    if (nullptr == addressInput) {
                        return;
                    }
                    const auto newServerAddress = addressInput->text().trimmed().toStdString();
                    if (g_serverAddress != newServerAddress) {
                        g_serverAddress = newServerAddress;
                        g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, g_serverAddress);
                    }
                    MainRouter::getInstance()->mainNotificationShow("保存成功", nullptr);
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save server config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                    MainRouter::getInstance()->mainNotificationShow(
                        "保存失败，请确认填写内容有效性", nullptr);
                }
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "服务器设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        address = new QWidget(body);
        addressLayout = new QHBoxLayout(address);
        addressLayout->setContentsMargins(20, 0, 0, 0);
        addressLayout->setSpacing(20);
#ifdef WIN32
        address->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        address->setFixedHeight(50);
#else
        address->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        address->setFixedHeight(100);
#endif
        addressLabel = new QLabel("服务器地址", address);
        addressInput = new QLineEditPro(address);
        addressInput->setAlignment(Qt::AlignRight);
        addressInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        addressInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");

        addressLayout->addWidget(addressLabel);
        addressLayout->addWidget(addressInput);
        bodyLayout->addWidget(address);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingServer::~MainSettingServer() = default;

void MainSettingServer::showEvent(QShowEvent *) {
    // data
    if (addressInput) {
        addressInput->setText(QString::fromStdString(g_serverAddress));
    }
}
