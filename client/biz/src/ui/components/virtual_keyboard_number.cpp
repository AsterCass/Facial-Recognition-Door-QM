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
    keyboardWidget->setStyleSheet("background-color: white");
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
    keyboardLayout->setSpacing(4);
    keyboardLayout->setContentsMargins(4, 4, 4, 4);
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (3 == row) {
                if (0 == col) {
                    hideBtn = new QPushButton("Hide");
                    QFont font = hideBtn->font();
                    font.setPointSize(5);
                    hideBtn->setFont(font);
                    hideBtn->setFixedHeight(minHeight);
                    keyboardLayout->addWidget(hideBtn, row, col);
                } else if (1 == col) {
                    numberBtn[0] = new QPushButton(QString::number(0));
                    QFont font = numberBtn[0]->font();
                    font.setPointSize(5);
                    numberBtn[0]->setFont(font);
                    numberBtn[0]->setFixedHeight(minHeight);
                    keyboardLayout->addWidget(numberBtn[0], row, col);
                } else {
                    deleteBtn = new QPushButton("Delete");
                    QFont font = deleteBtn->font();
                    font.setPointSize(5);
                    deleteBtn->setFont(font);
                    deleteBtn->setFixedHeight(minHeight);
                    keyboardLayout->addWidget(deleteBtn, row, col);
                }
            } else {
                const auto btnNum = row * 3 + col + 1;
                numberBtn[btnNum] = new QPushButton(QString::number(btnNum));
                QFont font = numberBtn[btnNum]->font();
                font.setPointSize(5);
                numberBtn[btnNum]->setFont(font);
                numberBtn[btnNum]->setFixedHeight(minHeight);
                keyboardLayout->addWidget(numberBtn[btnNum], row, col);
                connect(numberBtn[btnNum], &QPushButton::clicked, [this, btnNum] {
                    if (!currentInput) {
                        return;
                    }
                    currentInput->insert(QString::number(btnNum));
                });
            }
        }
    }
}


VirtualKeyboardNumber::~VirtualKeyboardNumber() = default;
