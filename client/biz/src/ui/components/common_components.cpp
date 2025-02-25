#include "ui/components/common_components.h"
#include "ui/components/virtual_keyboard.h"


void QLineEditPro::mousePressEvent(QMouseEvent *event) {
    VirtualKeyboard::getInstance()->showKeyboard(this);
    QLineEdit::mousePressEvent(event);
}
