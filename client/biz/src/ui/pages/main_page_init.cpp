#include "ui/pages/main_page_init.h"

#include "config/config.h"
#include "enums/general_enums.h"


MainPageInit::MainPageInit(QWidget *parent): QWidget(parent) {
    button = std::unique_ptr<QPushButton>(new QPushButton("I am Init, Click switch", this));
    connect(button.get(), &QPushButton::clicked, [](bool) {
        stackedWidget->setCurrentIndex(MAIN_PAGE_HOME);
    });
}


MainPageInit::~MainPageInit() = default;
