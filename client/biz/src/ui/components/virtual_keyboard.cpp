#include "ui/components/virtual_keyboard.h"

#include <iostream>
#include <ostream>
#include <QPushButton>

using namespace std;

VirtualKeyboard::VirtualKeyboard(QWidget *parent): QWidget(parent) {
    // Layout
    layout = new QVBoxLayout(this);
    mainScreenWidget = new QWidget(this);
    keyboardWidget = new QWidget(this);
    mainScreenWidget->setObjectName("VirtualKeyboardMask");
    mainScreenWidget->setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    keyboardWidget->setObjectName("VirtualKeyboardBody");
    layout->addWidget(mainScreenWidget, 2);
    layout->addWidget(keyboardWidget, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);
    mainScreenWidget->show();
    keyboardWidget->show();

    // Click event
    mainScreenWidgetClickEvent = new VirtualKeyboardMaskEventFilter();
    mainScreenWidget->installEventFilter(mainScreenWidgetClickEvent);

    // Keyboard body
    const int minHeight = parent->height() / 3 / 6;
    keyboardLayout = new QGridLayout(keyboardWidget);
    keyboardLayout->setSpacing(4);
    keyboardLayout->setContentsMargins(4, 4, 4, 4);
    int row = 0;
    for (auto keyRow: keyRows) {
        int col = 0;
        for (auto key: keyRow) {
            key.btn = new QPushButton(QString::fromStdString(key.key), keyboardWidget);

            if (key.key == "Space Bar") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 10);
                col += 10;
            } else if (key.key == "Backspace" || key.key == "Enter" || key.key == "Chinese" || key.key == "Hide") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 3);
                col += 3;
            } else if (key.key == "Tab" || key.key == "\\" || key.key == "Caps") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 2);
                col += 2;
            } else {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col);
                ++col;
            }
        }
        ++row;
    }
}


VirtualKeyboard::~VirtualKeyboard() = default;


void VirtualKeyboard::handleFocusChanged(QWidget *old, QWidget *now) {
    cout << old << now << endl;
}
