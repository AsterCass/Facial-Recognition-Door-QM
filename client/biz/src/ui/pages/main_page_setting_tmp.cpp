#include "ui/pages/main_page_setting_tmp.h"

#include "ui/main_router.h"


using namespace std;

MainSettingTmp::MainSettingTmp(QWidget *parent): QWidget(parent) {
    setObjectName("mainSettingTmp");
    setStyleSheet("#mainSettingTmp{background-color: white}");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);


    cancelBtn = new QPushButton("取消", this);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backUntilPage(MAIN_PAGE_HOME);
            });

    mainLayout->addWidget(cancelBtn);
    mainLayout->addStretch();
}


MainSettingTmp::~MainSettingTmp() = default;
