#include "ui/pages/main_page_init.h"
#include "config/config.h"
#include "enums/general_enums.h"

MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    button = std::make_unique<QPushButton>("I am Init, Click switch", this);
    QObject::connect(button.get(), &QPushButton::clicked, [](bool) {
        stackedWidget->setCurrentIndex(MAIN_PAGE_HOME);
    });
}


MainPageInit::~MainPageInit() = default;
