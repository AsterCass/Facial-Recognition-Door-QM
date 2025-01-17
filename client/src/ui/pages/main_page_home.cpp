#include "ui/pages/main_page_home.h"
#include "config/config.h"
#include "enums/general_enums.h"

MainPageHome::MainPageHome(QWidget *parent): QWidget(parent) {
    button = std::make_unique<QPushButton>("I am Home, Click switch", this);
    QObject::connect(button.get(), &QPushButton::clicked, [](bool) {
        stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
    });
}


MainPageHome::~MainPageHome() = default;
