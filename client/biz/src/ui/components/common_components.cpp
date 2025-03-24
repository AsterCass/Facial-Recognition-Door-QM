#include "ui/components/common_components.h"
#include "ui/components/virtual_keyboard.h"
#include "ui/components/virtual_keyboard_number.h"


void QLineEditPro::mousePressEvent(QMouseEvent *event) {
    VirtualKeyboard::getInstance()->showKeyboard(this);
}


void QLineEditProNumber::mousePressEvent(QMouseEvent *event) {
    VirtualKeyboardNumber::getInstance()->showKeyboard(this);
}
