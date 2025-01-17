#include "ui/main_router.h"

#include "config/config.h"
#include "enums/general_enums.h"
#include "ui/pages/main_page_home.h"
#include "ui/pages/main_page_init.h"


MainRouter::MainRouter(QWidget *parent): QWidget(parent) {
    // Size
    this->setGeometry(QRect(0, 0, 800, 600));
    // Load router
    stackedWidget = new QStackedWidget();
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);

    // Load all page
    stackedWidget->insertWidget(MAIN_PAGE_INIT, MainPageInit::getInstance());
    stackedWidget->insertWidget(MAIN_PAGE_HOME, MainPageHome::getInstance());

    // Current page
    stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
}


MainRouter::~MainRouter() {
    delete stackedWidget;
    stackedWidget = nullptr;
}
