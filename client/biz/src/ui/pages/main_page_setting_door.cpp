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
    save->setEnabled(false);
    connect(save, &QPushButton::clicked, this,
            [=] {
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "门禁参数设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
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
}
