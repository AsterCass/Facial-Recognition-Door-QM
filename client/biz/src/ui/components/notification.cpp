#include "ui/components/notification.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>


Notification::Notification(QWidget *parent): QWidget(parent) {
    this->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    messageLabel = new QLabel(this);
    messageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
}


Notification::~Notification() = default;


void Notification::setMessage(const QString &message) {
    messageLabel->setText(message);
    this->show();
}
