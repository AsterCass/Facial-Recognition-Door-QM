#include "ui/pages/main_page_init.h"

#include "airstrip_program_options.h"
#include "config/config.h"
#include "ui/components/main_component_header.h"

using namespace std;

MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    body = new QWidget(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setElement();
}

void MainPageInit::setElement() {
    header = MainComponentHeader::getInstance();
    body->setObjectName("pageInitBody");
    QString styleSheet;
    styleSheet.append("#pageInitBody{border-image: url(")
            .append(QString::fromStdString(g_appWorkDir))
            .append("static/images/bg-launch.png)}");
    body->setStyleSheet(styleSheet);

    mainLayout->addWidget(header, 1);
    mainLayout->addWidget(body, 17);
}

void MainPageInit::showEvent(QShowEvent *) {
    setElement();
}


MainPageInit::~MainPageInit() = default;
