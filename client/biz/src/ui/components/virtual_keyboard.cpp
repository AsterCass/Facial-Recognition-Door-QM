#include "ui/components/virtual_keyboard.h"

#include <QPushButton>

using namespace std;

bool VirtualKeyboardMaskEventFilter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        const auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            VirtualKeyboard::getInstance()->hideKeyboard();
        }
    }
    // 继续传播事件
    return QObject::eventFilter(watched, event);
}

VirtualKeyboard::VirtualKeyboard(QWidget *parent): QWidget(parent) {
    if (nullptr == parent) {
        return;
    }
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored
            key.btn = new QPushButton(QString::fromStdString(key.key), keyboardWidget);
#pragma clang diagnostic pop
            QFont font = key.btn->font();
            font.setPointSize(5);
            key.btn->setFont(font);
            if (key.key == "Space Bar") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 10);
                connect(key.btn, &QPushButton::clicked, [this](bool) {
                    if (!currentInput) {
                        return;
                    }
                    currentInput->insert(" ");
                });
                col += 10;
            } else if (key.key == "Backspace" || key.key == "Enter" || key.key == "Chinese" || key.key == "Hide") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 3);
                if (key.key == "Backspace") {
                    connect(key.btn, &QPushButton::clicked, [this](bool) {
                        if (!currentInput) {
                            return;
                        }
                        currentInput->backspace();
                    });
                } else if (key.key == "Enter" || key.key == "Hide") {
                    connect(key.btn, &QPushButton::clicked, [this](bool) {
                        if (!currentInput) {
                            return;
                        }
                        hideKeyboard();
                    });
                } else if (key.key == "Chinese") {
                    //todo
                }
                col += 3;
            } else if (key.key == "Tab" || key.key == "\\" || key.key == "Caps") {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col, 1, 2);
                if (key.key == "Tab") {
                    connect(key.btn, &QPushButton::clicked, [this](bool) {
                        if (!currentInput) {
                            return;
                        }
                        currentInput->insert("    ");
                    });
                } else if (key.key == "\\") {
                    connect(key.btn, &QPushButton::clicked, [this, key](bool) {
                        if (!currentInput) {
                            return;
                        }
                        currentInput->insert(QString::fromStdString(key.key));
                    });
                } else if (key.key == "Caps") {
                    //todo
                }
                col += 2;
            } else {
                key.btn->setFixedHeight(minHeight);
                keyboardLayout->addWidget(key.btn, row, col);
                connect(key.btn, &QPushButton::clicked, [this, key](bool) {
                    if (!currentInput) {
                        return;
                    }
                    currentInput->insert(QString::fromStdString(key.key));
                });
                ++col;
            }
        }
        ++row;
    }
}


VirtualKeyboard::~VirtualKeyboard() = default;
