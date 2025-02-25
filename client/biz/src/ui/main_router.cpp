#include "ui/main_router.h"

#include "config/config.h"
#include "enums/general_enums.h"
#include "ui/components/virtual_keyboard.h"
#include "ui/pages/main_page_home.h"
#include "ui/pages/main_page_init.h"
#include <QMessageBox>

#include "airstrip_program_options.h"
#include "ui/components/notification.h"


MainRouter::MainRouter(QWidget *parent): QWidget(parent) {
    // param
    int height = 0;
    airstrip::getProgramOptions(PRO_OPT_APP_HEIGHT, &height);
    int width = 0;
    airstrip::getProgramOptions(PRO_OPT_APP_WIDTH, &width);

    // Size
    this->setFixedSize(width, height);

    // Load route
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(QRect(0, 0, width, height));
    stackedWidget->insertWidget(MAIN_PAGE_INIT, MainPageInit::getInstance());
    stackedWidget->insertWidget(MAIN_PAGE_HOME, MainPageHome::getInstance());
    stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
    stackedWidget->show();

    // Load notification
    const auto notification = Notification::getInstance(this);
    notification->setGeometry(QRect(0, 0, width, height));
    notification->hide();

    // Load virtual keyboard
    const auto virtualKeyboard = VirtualKeyboard::getInstance(this);
    virtualKeyboard->setGeometry(QRect(0, 0, width, height));
    virtualKeyboard->show();
}


MainRouter::~MainRouter() {
    delete stackedWidget;
    stackedWidget = nullptr;
}
