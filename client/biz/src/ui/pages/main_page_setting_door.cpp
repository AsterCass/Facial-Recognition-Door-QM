#include "ui/pages/main_page_setting_door.h"

#include <airstrip_log.h>
#include <sstream>
#include <ui/main_router.h>


using namespace std;

MainSettingDoor::MainSettingDoor(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // save
    save = new QPushButton("保存");
    connect(save, &QPushButton::clicked, this,
            [=] {
                try {
                    {
                        if (nullptr != doorOpenTimeInput) {
                            const auto newData = std::stoi(doorOpenTimeInput->text().trimmed().toStdString());
                            if (g_autoCloseDoorSec != newData) {
                                g_autoCloseDoorSec = newData;
                                g_commonDb.upsertConfig(
                                    PRO_DB_AUTO_CLOSE_DOOR_SEC, to_string(g_autoCloseDoorSec));
                            }
                        }
                        MainRouter::getInstance()->mainNotificationShow("保存成功", nullptr);
                    }
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save door config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                    MainRouter::getInstance()->mainNotificationShow(
                        "保存失败，请确认填写内容有效性", nullptr);
                }
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "门禁参数设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        doorOpenTime = new QWidget(body);
        doorOpenTimeLayout = new QHBoxLayout(doorOpenTime);
        doorOpenTimeLayout->setContentsMargins(20, 0, 0, 0);
        doorOpenTimeLayout->setSpacing(20);
#ifdef WIN32
        doorOpenTime->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        doorOpenTime->setFixedHeight(50);
#else
        doorOpenTime->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        doorOpenTime->setFixedHeight(100);
#endif
        doorOpenTimeLabel = new QLabel("开门保持时间（s）", doorOpenTime);
        doorOpenTimeInput = new QLineEditPro(doorOpenTime);
        doorOpenTimeInput->setAlignment(Qt::AlignRight);
        doorOpenTimeInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        doorOpenTimeInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");

        doorOpenTimeLayout->addWidget(doorOpenTimeLabel);
        doorOpenTimeLayout->addWidget(doorOpenTimeInput);
        bodyLayout->addWidget(doorOpenTime);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingDoor::~MainSettingDoor() = default;

void MainSettingDoor::showEvent(QShowEvent *) {
    // data
    if (doorOpenTimeInput) {
        doorOpenTimeInput->setText(QString::number(g_autoCloseDoorSec));
    }
}
