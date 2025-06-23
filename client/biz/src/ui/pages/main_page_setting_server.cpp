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
    save->setEnabled(false);
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
                    save->clearFocus();
                    save->setEnabled(false);
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save server config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
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
        address->setFixedHeight(60);
#else
        address->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        address->setFixedHeight(120);
#endif
        addressLabel = new QLabel("服务器地址", address);
        addressInput = new QLineEditPro(address);
        addressInput->setAlignment(Qt::AlignRight);
        addressInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        addressInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
                QLineEdit:focus {
                        background-color: rgb(27, 50, 77);
                        border: 2px solid rgb(13, 133, 255);
                }
        )");
        connect(addressInput, &QLineEdit::textChanged, this, [this](const QString &text) {
            if (save == nullptr) {
                return;
            }
            if (text.toStdString() == g_serverAddress) {
                save->setEnabled(false);
            } else {
                save->setEnabled(true);
            }
        });

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
    addressInput->setText(QString::fromStdString(g_serverAddress));
}
