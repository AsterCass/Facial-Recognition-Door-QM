#include "ui/pages/main_page_init.h"

#include "config/config.h"
#include "ui/components/main_component_header.h"

using namespace std;

MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    header = new MainComponentHeader(this);
    body = new QWidget(this);

    body->setObjectName("pageInitBody");
    QString styleSheet;
    styleSheet.append("#pageInitBody{border-image: url(")
            .append(QString::fromStdString(g_appWorkDir))
            .append("static/images/bg-launch.png)}");
    body->setStyleSheet(styleSheet);

    mainLayout->addWidget(header);
    mainLayout->addWidget(body);
}


MainPageInit::~MainPageInit() = default;
