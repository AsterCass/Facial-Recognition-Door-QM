#include "ui/components/notification.h"

#include <QLabel>
#include <QWidget>


Notification::Notification(QWidget *parent): QWidget(parent) {
    this->setObjectName("VirtualNotificationMask");
    this->setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainScreenWidget = new QWidget(this);
    messageLabel = new QLabel("", mainScreenWidget);
    btnWidget = new QWidget(mainScreenWidget);

    notificationLayout = new QVBoxLayout(mainScreenWidget);
    notificationLayout->setAlignment(Qt::AlignCenter);
    notificationLayout->addWidget(messageLabel);
    notificationLayout->addWidget(btnWidget);

#ifdef WIN32
    mainScreenWidget->setStyleSheet(
        "background-color: rgb(31, 31, 31); border-radius: 8px; color: white; font-size: 16px");
    mainScreenWidget->setFixedWidth(300);
#else
    mainScreenWidget->setStyleSheet(
    "background-color: rgb(31, 31, 31); border-radius: 16px; color: white; font-size: 32px");
    mainScreenWidget->setFixedWidth(600);
#endif

    mainLayout->addStretch(1);
    mainLayout->addWidget(mainScreenWidget);
    mainLayout->addStretch(2);

    // Content
#ifdef WIN32
    messageLabel->setStyleSheet("margin: 12px; font-size: 16px;");
#else
    messageLabel->setStyleSheet("margin: 12px; font-size: 32px;");
#endif
    messageLabel->setWordWrap(true);

    // Buttons
    btnLayout = new QHBoxLayout(btnWidget);
    cancelBtn = new QPushButton("取消", btnWidget);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                if (confirmCallback) {
                    confirmCallback(false);
                }
                this->hide();
            });
    cancelBtn->setStyleSheet("background-color: rgb(101, 101, 101);");
#ifdef WIN32
    cancelBtn->setFixedSize(120, 30);
#else
    cancelBtn->setFixedSize(240, 60);
#endif
    confirmBtn = new QPushButton("确认", btnWidget);
    connect(confirmBtn, &QPushButton::clicked, this,
            [=] {
                if (confirmCallback) {
                    confirmCallback(true);
                }
                this->hide();
            });
    confirmBtn->setStyleSheet("background-color: rgb(13, 133, 255);");
#ifdef WIN32
    confirmBtn->setFixedSize(120, 30);
#else
    confirmBtn->setFixedSize(240, 60);
#endif
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);
}


Notification::~Notification() = default;


void Notification::setMessage(const std::string &message, const std::function<void(bool)> &callback) {
    confirmCallback = callback;
    messageLabel->setText(QString::fromStdString(message));
    this->show();
}
