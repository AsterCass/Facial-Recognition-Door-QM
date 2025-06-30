#include "ui/components/notification.h"

#include <QLabel>
#include <QWidget>
#include <QThread>


Notification::Notification(QWidget *parent): QWidget(parent) {
    this->setObjectName("VirtualNotificationMask");
    this->setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainScreenWidget = new QWidget(this);
    messageLabel = new QLabel("", mainScreenWidget);
    messageLabel->setAlignment(Qt::AlignHCenter);
    btnWidget = new QWidget(mainScreenWidget);

    notificationLayout = new QVBoxLayout(mainScreenWidget);
    notificationLayout->setMargin(10);
    notificationLayout->addWidget(messageLabel);
    notificationLayout->addWidget(btnWidget);

#ifdef WIN32
    mainScreenWidget->setStyleSheet(
        "background-color: rgb(31, 31, 31); border-radius: 8px; color: white; font-size: 14px");
    mainScreenWidget->setFixedWidth(300);
#else
    mainScreenWidget->setStyleSheet(
    "background-color: rgb(31, 31, 31); border-radius: 16px; color: white; font-size: 28px");
    mainScreenWidget->setFixedWidth(600);
#endif

    mainLayout->addStretch();
    mainLayout->addWidget(mainScreenWidget);
    mainLayout->addStretch();

    // Content
#ifdef WIN32
    messageLabel->setStyleSheet("margin: 12px; font-size: 16px;");
#else
    messageLabel->setStyleSheet("margin: 12px; font-size: 32px;");
#endif
    messageLabel->setWordWrap(true);

    // Buttons
    btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setMargin(0);
    cancelBtn = new QPushButton("取消", btnWidget);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                if (confirmCallback) {
                    confirmCallback(false);
                }
                this->hide();
            });
    confirmBtn = new QPushButton("确认", btnWidget);
    connect(confirmBtn, &QPushButton::clicked, this,
            [=] {
                if (confirmCallback) {
                    confirmCallback(true);
                }
                this->hide();
            });
#ifdef WIN32
    confirmBtn->setStyleSheet(
        "background-color: rgb(13, 133, 255); color: white; border-radius: 8px;box-shadow: none;"
        " height: 25px; font-size: 14px");
    cancelBtn->setStyleSheet(
        "background-color: rgb(101, 101, 101); color: white; border-radius: 8px;box-shadow: none;"
        " height: 25px; font-size: 14px");
#else
    confirmBtn->setStyleSheet(
        "background-color: rgb(13, 133, 255); color: white; border-radius: 16px;box-shadow: none;"
        " height: 50px; font-size: 28px");
    cancelBtn->setStyleSheet(
        "background-color: rgb(101, 101, 101); color: white; border-radius: 16px;box-shadow: none;"
        " height: 50px; font-size: 28px");
#endif
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);
}


Notification::~Notification() = default;


void Notification::setMessage(const std::string &message, const std::function<void(bool)> &callback) {
    // 投递到主线程
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, [this, message, callback] {
            setMessage(message, callback);
        }, Qt::QueuedConnection);
        return;
    }
    confirmCallback = callback;
    messageLabel->setText(QString::fromStdString(message));
    this->show();
}
