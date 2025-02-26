#include "ui/pages/main_page_init.h"
#include "ui/components/main_component_header.h"


MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    header = MainComponentHeader::getInstance(this);
    body = new QWidget(this);
    body->setObjectName("pageInitBody");
    body->setStyleSheet("#pageInitBody{border-image: url(:/assets/images/bg-launch.png)}");


    mainLayout->addWidget(header, 1);
    mainLayout->addWidget(body, 17);
}


MainPageInit::~MainPageInit() = default;
