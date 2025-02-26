#include "ui/pages/main_page_init.h"

#include "airstrip_program_options.h"
#include "config/config.h"
#include "ui/components/main_component_header.h"

using namespace std;

MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    header = MainComponentHeader::getInstance(this);
    body = new QWidget(this);
    body->setObjectName("pageInitBody");
    QString styleSheet;
    styleSheet.append("#pageInitBody{border-image: url(")
            .append(QString::fromStdString(appWorkDir))
            .append("static/images/bg-launch.png)}");
    body->setStyleSheet(styleSheet);


    mainLayout->addWidget(header, 1);
    mainLayout->addWidget(body, 17);
}


MainPageInit::~MainPageInit() = default;
