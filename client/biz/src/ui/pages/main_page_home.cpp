#include "ui/pages/main_page_home.h"
#include "config/config.h"
#include "enums/general_enums.h"

MainPageHome::MainPageHome(QWidget *parent): QWidget(parent) {
    button = std::unique_ptr<QPushButton>(new QPushButton("I am Home, Click switch", this));
    connect(button.get(), &QPushButton::clicked, [](bool) {
        stackedWidget->setCurrentIndex(MAIN_PAGE_INIT);
    });


    edit = std::unique_ptr<QLineEdit>(new QLineEdit(this));
}


MainPageHome::~MainPageHome() = default;
