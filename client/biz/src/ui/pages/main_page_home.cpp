#include "ui/pages/main_page_home.h"

#include "ui/components/main_component_header.h"

MainPageHome::MainPageHome(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    body = new QWidget(this);

    setElement();
}

void MainPageHome::setElement() {
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    header = MainComponentHeader::getInstance();

    mainLayout->addWidget(header, 1);
    mainLayout->addWidget(body, 17);
}

void MainPageHome::showEvent(QShowEvent *) {
    setElement();
}


MainPageHome::~MainPageHome() = default;
