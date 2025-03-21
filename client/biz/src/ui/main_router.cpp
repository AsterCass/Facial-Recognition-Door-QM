#include "ui/main_router.h"

#include <deque>

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


std::mutex stackChangeMutex;
deque<int> mainRouterQueue = {};

void printMainRouterQueue(const string &functionName) {
    ostringstream oss;
    oss << "Current widget stack: ";
    for (const auto index: mainRouterQueue) {
        oss << index << " ";
    }
    logPrintln(oss.str(), airstrip::INFO, functionName);
}

void MainRouter::addPage(const MainPage page) const {
    if (nullptr == stackedWidget) return;

    lock_guard<mutex> lock(stackChangeMutex);

    stackedWidget->setCurrentIndex(page);
    mainRouterQueue.push_back(page);
    if (mainRouterQueue.size() > 100) {
        mainRouterQueue.pop_front();
    }

    printMainRouterQueue(__FUNCTION__);
}

void MainRouter::removePageFromTop(const MainPage page) const {
    if (nullptr == stackedWidget) return;

    lock_guard<mutex> lock(stackChangeMutex);

    if (mainRouterQueue.empty() || mainRouterQueue.back() != page) {
        printMainRouterQueue(__FUNCTION__);
        return;
    }

    mainRouterQueue.pop_back();
    if (!mainRouterQueue.empty()) {
        stackedWidget->setCurrentIndex(mainRouterQueue.back());
    }
    printMainRouterQueue(__FUNCTION__);
}

void MainRouter::removePageAll(const MainPage page) const {
    if (nullptr == stackedWidget) return;

    lock_guard<mutex> lock(stackChangeMutex);

    if (mainRouterQueue.empty()) {
        printMainRouterQueue(__FUNCTION__);
        return;
    }

    mainRouterQueue.erase(std::remove(
                              mainRouterQueue.begin(), mainRouterQueue.end(), page),
                          mainRouterQueue.end());

    if (!mainRouterQueue.empty()) {
        stackedWidget->setCurrentIndex(mainRouterQueue.back());
    }
    printMainRouterQueue(__FUNCTION__);
}

void MainRouter::backPage() const {
    if (nullptr == stackedWidget) return;

    lock_guard<mutex> lock(stackChangeMutex);

    if (mainRouterQueue.empty()) {
        printMainRouterQueue(__FUNCTION__);
        return;
    }

    mainRouterQueue.pop_back();
    if (!mainRouterQueue.empty()) {
        stackedWidget->setCurrentIndex(mainRouterQueue.back());
    }
    printMainRouterQueue(__FUNCTION__);
}

void MainRouter::backUntilPage(const MainPage page) const {
    if (nullptr == stackedWidget) return;

    lock_guard<mutex> lock(stackChangeMutex);

    if (mainRouterQueue.empty()) {
        printMainRouterQueue(__FUNCTION__);
        return;
    }

    while (!mainRouterQueue.empty() && mainRouterQueue.back() != page) {
        mainRouterQueue.pop_back();
    }

    if (mainRouterQueue.empty()) {
        mainRouterQueue.push_back(MAIN_PAGE_INIT);
        mainRouterQueue.push_back(MAIN_PAGE_HOME);
        stackedWidget->setCurrentIndex(MAIN_PAGE_HOME);
    } else {
        stackedWidget->setCurrentIndex(mainRouterQueue.back());
    }

    printMainRouterQueue(__FUNCTION__);
}

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
    stackedWidget->insertWidget(MAIN_PAGE_INIT, new MainPageInit(this));
    stackedWidget->insertWidget(MAIN_PAGE_HOME, new MainPageHome(this));
    stackedWidget->insertWidget(MAIN_PAGE_SETTING_LOGIN, new MainSettingLogin(this));
    stackedWidget->insertWidget(MAIN_PAGE_SETTING_TMP, new MainSettingTmp(this));
    addPage(MAIN_PAGE_INIT);
    stackedWidget->show();

    // Load notification
    const auto notification = new Notification(this);
    notification->setGeometry(QRect(0, 0, width, height));
    notification->hide();

    // Load virtual keyboard
    const auto virtualKeyboard = VirtualKeyboard::getInstance(this);
    virtualKeyboard->setGeometry(QRect(0, 0, width, height));
    virtualKeyboard->hide();
}


MainRouter::~MainRouter() = default;
