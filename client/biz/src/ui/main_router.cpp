#include "ui/main_router.h"

#include "config/config.h"
#include "enums/general_enums.h"
#include "ui/components/virtual_keyboard.h"
#include "ui/pages/main_page_home.h"
#include "ui/pages/main_page_init.h"
#include <sstream>

#include "airstrip_program_options.h"
#include "ui/components/notification.h"
#include "ui/pages/main_page_setting_login.h"
#include "ui/pages/main_page_setting_tmp.h"

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
    g_stackedWidget = new QStackedWidget(this);
    this->setObjectName("stackedWidget");
    this->setStyleSheet("#stackedWidget{background: transparent;}");
    g_stackedWidget->setGeometry(QRect(0, 0, width, height));
    connect(g_stackedWidget, &QStackedWidget::currentChanged, [](const int newIndex) {
        if (!g_routerQueue.empty() && g_routerQueue.back() == newIndex) {
            airstrip::logPrintln("New index repleat : " + to_string(newIndex));
            return;
        }
        g_routerQueue.push_back(newIndex);
        if (g_routerQueue.size() > 20) {
            g_routerQueue.pop_front();
        }
        ostringstream oss;
        oss << "Current widget stack: ";
        for (const auto index: g_routerQueue) {
            oss << index << " ";
        }
        airstrip::logPrintln(oss.str());
    });
    g_stackedWidget->insertWidget(MAIN_PAGE_HOME, MainPageHome::getInstance(g_stackedWidget));
    g_stackedWidget->insertWidget(MAIN_PAGE_SETTING_LOGIN, MainSettingLogin::getInstance(g_stackedWidget));
    g_stackedWidget->insertWidget(MAIN_PAGE_SETTING_TMP, MainSettingTmp::getInstance(g_stackedWidget));
    g_stackedWidget->insertWidget(MAIN_PAGE_INIT, MainPageInit::getInstance(g_stackedWidget));
    g_stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
    g_stackedWidget->show();

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
    delete g_stackedWidget;
    g_stackedWidget = nullptr;
}
