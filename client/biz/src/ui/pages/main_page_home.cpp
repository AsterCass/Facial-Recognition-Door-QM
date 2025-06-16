#include "ui/pages/main_page_home.h"

#include "camera/camera_frame.h"
#include "ui/components/main_component_header.h"

MainPageHome::MainPageHome(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    this->setObjectName("mainPageHome");
    this->setStyleSheet("#mainPageHome{background: transparent;}");
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    header = new MainComponentHeader(this);
    body = CameraFrame::getInstance();

    mainLayout->addWidget(header);
    mainLayout->addWidget(body);
}


MainPageHome::~MainPageHome() = default;


void MainPageHome::showEvent(QShowEvent *) {
    g_closeFaceRecognition = false;
}

void MainPageHome::hideEvent(QHideEvent *) {
    g_closeFaceRecognition = true;
}
