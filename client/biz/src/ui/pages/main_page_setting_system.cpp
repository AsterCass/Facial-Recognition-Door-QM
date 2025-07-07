#include "ui/pages/main_page_setting_system.h"

#include <airstrip_log.h>
#include <airstrip_thread_pool.h>
#include <sstream>
#include <api/api.h>
#include <ui/main_router.h>


using namespace std;


MainSettingSystem::MainSettingSystem(QWidget *parent): QWidget(parent) {
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
    settingHeader = new SettingHeader(this, "系统参数设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        inDevelop = new QLabel("功能开发中", body);
        inDevelop->setStyleSheet("color: white; font-size: 24px");
        inDevelop->setAlignment(Qt::AlignCenter);
        bodyLayout->addWidget(inDevelop);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingSystem::~MainSettingSystem() = default;

void MainSettingSystem::showEvent(QShowEvent *) {
    // data
}
