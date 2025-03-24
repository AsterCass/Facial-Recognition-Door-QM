#include "ui/components/virtual_keyboard_number.h"

#include <QMouseEvent>
#include <QPushButton>

using namespace std;

bool VirtualKeyboardNumberMaskEventFilter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            VirtualKeyboardNumber::getInstance()->hideKeyboard();
        }
    }
    // 继续传播事件
    return QObject::eventFilter(watched, event);
}

VirtualKeyboardNumber::VirtualKeyboardNumber(QWidget *parent): QWidget(parent) {
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
    keyboardWidget->setStyleSheet("background-color: rgb(31, 31, 31)");
    layout->addWidget(mainScreenWidget, 2);
    layout->addWidget(keyboardWidget, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);
    mainScreenWidget->show();
    keyboardWidget->show();

    // Click event
    mainScreenWidgetClickEvent = new VirtualKeyboardNumberMaskEventFilter();
    mainScreenWidget->installEventFilter(mainScreenWidgetClickEvent);


    // Keyboard body
    const int minHeight = parent->height() / 3 / 5;
    keyboardLayout = new QGridLayout(keyboardWidget);
    keyboardLayout->setSpacing(8);
    keyboardLayout->setContentsMargins(8, 8, 8, 8);
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (3 == row) {
                if (0 == col) {
                    hideBtn = new QPushButton("⇩");
                    QFont font = hideBtn->font();
                    font.setPointSize(15);
                    hideBtn->setFont(font);
                    hideBtn->setFixedHeight(minHeight);
                    hideBtn->setStyleSheet("QPushButton {background-color: rgb(101, 101, 101);"
                        "border-radius: 8px;color: white;} QPushButton:pressed {background-color: rgb(13, 133, 255);}");
                    keyboardLayout->addWidget(hideBtn, row, col);
                    connect(hideBtn, &QPushButton::clicked, [this] {
                        if (!currentInput) {
                            return;
                        }
                        hideKeyboard();
                    });
                } else if (1 == col) {
                    numberBtn[0] = new QPushButton(QString::number(0));
                    QFont font = numberBtn[0]->font();
                    font.setPointSize(10);
                    numberBtn[0]->setFont(font);
                    numberBtn[0]->setFixedHeight(minHeight);
                    numberBtn[0]->setStyleSheet("QPushButton {background-color: rgb(101, 101, 101);"
                        "border-radius: 8px;color: white;} QPushButton:pressed {background-color: rgb(13, 133, 255);}");
                    keyboardLayout->addWidget(numberBtn[0], row, col);
                    connect(numberBtn[0], &QPushButton::clicked, [this] {
                        if (!currentInput) {
                            return;
                        }
                        currentInput->insert(QString::number(0));
                        currentInput->setFocus();
                    });
                } else {
                    deleteBtn = new QPushButton("⌫");
                    QFont font = deleteBtn->font();
                    font.setPointSize(15);
                    deleteBtn->setFont(font);
                    deleteBtn->setFixedHeight(minHeight);
                    deleteBtn->setStyleSheet("QPushButton {background-color: rgb(101, 101, 101);"
                        "border-radius: 8px;color: white;} QPushButton:pressed {background-color: rgb(13, 133, 255);}");
                    keyboardLayout->addWidget(deleteBtn, row, col);
                    connect(deleteBtn, &QPushButton::clicked, [this] {
                        if (!currentInput) {
                            return;
                        }
                        currentInput->backspace();
                        currentInput->setFocus();
                    });
                }
            } else {
                const auto btnNum = row * 3 + col + 1;
                numberBtn[btnNum] = new QPushButton(QString::number(btnNum));
                QFont font = numberBtn[btnNum]->font();
                font.setPointSize(10);
                numberBtn[btnNum]->setFont(font);
                numberBtn[btnNum]->setFixedHeight(minHeight);
                numberBtn[btnNum]->setStyleSheet("QPushButton {background-color: rgb(101, 101, 101);"
                    "border-radius: 8px;color: white;} QPushButton:pressed {background-color: rgb(13, 133, 255);}");
                keyboardLayout->addWidget(numberBtn[btnNum], row, col);
                connect(numberBtn[btnNum], &QPushButton::clicked, [this, btnNum] {
                    if (!currentInput) {
                        return;
                    }
                    currentInput->insert(QString::number(btnNum));
                    currentInput->setFocus();
                });
            }
        }
    }
}


VirtualKeyboardNumber::~VirtualKeyboardNumber() = default;
