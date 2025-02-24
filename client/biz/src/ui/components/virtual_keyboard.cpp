#include "ui/components/virtual_keyboard.h"

#include <iostream>
#include <ostream>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QScreen>

#include "airstrip_log.h"


VirtualKeyboard::VirtualKeyboard(QWidget *parent): QWidget(parent) {
}


VirtualKeyboard::~VirtualKeyboard() = default;


void VirtualKeyboard::handleFocusChanged(QWidget *old, QWidget *now) {
}
