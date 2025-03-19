#include "ui/pages/main_page_setting_tmp.h"


using namespace std;

MainSettingTmp::MainSettingTmp(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setElement();
}

void MainSettingTmp::setElement() {
}


void MainSettingTmp::showEvent(QShowEvent *) {
    setElement();
}


MainSettingTmp::~MainSettingTmp() = default;
