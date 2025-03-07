#include "ui/main_router.h"

#include "config/config.h"
#include "enums/general_enums.h"
#include "ui/components/virtual_keyboard.h"
#include "ui/pages/main_page_home.h"
#include "ui/pages/main_page_init.h"
#include <QMessageBox>
#include <sstream>

#include "airstrip_program_options.h"
#include "ui/components/notification.h"

using namespace std;

MainRouter::MainRouter(QWidget *parent): QWidget(parent) {
    // param
    int height = 0;
    airstrip::getProgramOptions(PRO_OPT_APP_HEIGHT, &height);
    int width = 0;
    airstrip::getProgramOptions(PRO_OPT_APP_WIDTH, &width);

    // Size
    this->setObjectName("mainRouter");
    this->setStyleSheet("#mainRouter{background: transparent;}");
    this->setFixedSize(width, height);

    // Load route
    stackedWidget = new QStackedWidget(this);
    this->setObjectName("stackedWidget");
    this->setStyleSheet("#stackedWidget{background: transparent;}");
    stackedWidget->setGeometry(QRect(0, 0, width, height));
    connect(stackedWidget, &QStackedWidget::currentChanged, [](const int newIndex) {
        routerQueue.push_back(newIndex);
        if (routerQueue.size() > 10) {
            routerQueue.pop_front();
        }
        ostringstream oss;
        oss << "Current widget stack: ";
        for (const auto index: routerQueue) {
            oss << index << " ";
        }
        logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);
    });
    stackedWidget->insertWidget(MAIN_PAGE_INIT, MainPageInit::getInstance(stackedWidget));
    stackedWidget->insertWidget(MAIN_PAGE_HOME, MainPageHome::getInstance(stackedWidget));
    stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
    stackedWidget->show();

    // Load notification
    const auto notification = Notification::getInstance(this);
    notification->setGeometry(QRect(0, 0, width, height));
    notification->hide();

    // Load virtual keyboard
    const auto virtualKeyboard = VirtualKeyboard::getInstance(this);
    virtualKeyboard->setGeometry(QRect(0, 0, width, height));
    virtualKeyboard->hide();
}


MainRouter::~MainRouter() {
    delete stackedWidget;
    stackedWidget = nullptr;
}
