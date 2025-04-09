#include "ui/components/face_register.h"

#include <airstrip_thread_pool.h>
#include <thread>
#include <unistd.h>
#include "api/api.h"
#include "config/config.h"
#include "ui/components/virtual_keyboard_number.h"


FaceRegister::FaceRegister(QWidget *parent): QWidget(parent) {
    this->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    faceRegisterWidget = new QWidget(this);
    faceRegisterWidget->setObjectName("faceRegisterMain");

#ifdef WIN32
    faceRegisterWidget->setStyleSheet(
        "background-color: rgb(31, 31, 31); border-radius: 8px; color: white; font-size: 16px");
    faceRegisterWidget->setFixedWidth(300);
#else
    faceRegisterWidget->setStyleSheet(
    "background-color: rgb(31, 31, 31); border-radius: 16px; color: white; font-size: 32px");
    faceRegisterWidget->setFixedWidth(600);
#endif
    mainLayout->addStretch(1);
    mainLayout->addWidget(faceRegisterWidget);
    mainLayout->addStretch(2);

    // Content
    faceRegisterLayout = new QVBoxLayout(faceRegisterWidget);
    faceRegisterLayout->setAlignment(Qt::AlignCenter);
    faceRegisterTitle = new QLabel("请输入手机号", faceRegisterWidget);
    faceRegisterTitle->setAlignment(Qt::AlignCenter);
    faceRegisterTitle->setStyleSheet("margin: 20px");
    phoneNumberWidget = new QWidget(faceRegisterWidget);
    btnWidget = new QWidget(faceRegisterWidget);
#ifdef WIN32
    btnWidget->setFixedHeight(60);
#else
    btnWidget->setFixedHeight(120);
#endif
    errorTips = new QLabel("");
    errorTips->setAlignment(Qt::AlignCenter);
    errorTips->setStyleSheet("margin-top: 5px; font-size: 16px; color: red");
    faceRegisterLayout->addWidget(faceRegisterTitle);
    faceRegisterLayout->addWidget(phoneNumberWidget);
    faceRegisterLayout->addWidget(errorTips);
    faceRegisterLayout->addWidget(btnWidget);

    // Input
    phoneNumberLayout = new QHBoxLayout(phoneNumberWidget);
    phoneNumberLayout->setMargin(0);
    phoneNumberLayout->setSpacing(0);
#ifdef WIN32
    phoneNumberFirst = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberFirst->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 24px; padding-left: 18px");
    phoneNumberFirst->setFixedSize(75, 35);

    phoneNumberSecond = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberSecond->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 24px; padding-left: 18px");
    phoneNumberSecond->setFixedSize(90, 35);

    phoneNumberThird = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberThird->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 24px; padding-left: 18px");
    phoneNumberThird->setFixedSize(90, 35);
#else
    phoneNumberFirst = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberFirst->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 48px; padding-left: 24px");
    phoneNumberFirst->setFixedSize(150, 70);

    phoneNumberSecond = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberSecond->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 48px; padding-left: 24px");
    phoneNumberSecond->setFixedSize(180, 70);

    phoneNumberThird = new QLineEditProNumber(phoneNumberWidget);
    phoneNumberThird->setStyleSheet(
        "background-color: rgb(28, 50, 79); font-size: 48px; padding-left: 24px");
    phoneNumberThird->setFixedSize(180, 70);
#endif


    connect(phoneNumberFirst, &QLineEdit::textChanged, this,
            [=] {
                if (phoneNumberFirst->text().size() > 3) {
                    phoneNumberSecond->setFocus();
                    VirtualKeyboardNumber::getInstance()->switchCurrentInput(phoneNumberSecond);

                    const auto ch = phoneNumberFirst->text().at(3);
                    phoneNumberFirst->backspace();
                    if (phoneNumberSecond->text().isEmpty()) {
                        phoneNumberSecond->setText(ch);
                    }
                }
            });
    connect(phoneNumberSecond, &QLineEdit::textChanged, this,
            [=] {
                if (phoneNumberSecond->text().isEmpty()) {
                    phoneNumberFirst->setFocus();
                    VirtualKeyboardNumber::getInstance()->switchCurrentInput(phoneNumberFirst);
                }
                if (phoneNumberSecond->text().size() > 4) {
                    phoneNumberThird->setFocus();
                    VirtualKeyboardNumber::getInstance()->switchCurrentInput(phoneNumberThird);

                    const auto ch = phoneNumberSecond->text().at(4);
                    phoneNumberSecond->backspace();
                    if (phoneNumberThird->text().isEmpty()) {
                        phoneNumberThird->setText(ch);
                    }
                }
            });
    connect(phoneNumberThird, &QLineEdit::textChanged, this,
            [=] {
                if (phoneNumberThird->text().isEmpty()) {
                    phoneNumberSecond->setFocus();
                    VirtualKeyboardNumber::getInstance()->switchCurrentInput(phoneNumberSecond);
                }
                if (phoneNumberThird->text().size() > 4) {
                    phoneNumberThird->backspace();
                }
            });
    phoneNumberLayout->addWidget(phoneNumberFirst);
    phoneNumberLayout->addWidget(phoneNumberSecond);
    phoneNumberLayout->addWidget(phoneNumberThird);

    // Buttons
    btnLayout = new QHBoxLayout(btnWidget);
    cancelBtn = new QPushButton("取消", btnWidget);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                this->hide();
            });
    cancelBtn->setStyleSheet("background-color: rgb(101, 101, 101);");
#ifdef WIN32
    cancelBtn->setFixedSize(120, 30);
#else
    cancelBtn->setFixedSize(240, 60);
#endif
    registerBtn = new QPushButton("确认", btnWidget);
    connect(registerBtn, &QPushButton::clicked, this,
            [=] {
                if (g_onFaceRegisterProcess) {
                    return;
                }
                g_onFaceRegisterProcess = true;
                const auto phoneNumber = phoneNumberFirst->text() +
                                         phoneNumberSecond->text() +
                                         phoneNumberThird->text();
                if (phoneNumber.size() != 11) {
                    resetTips(false, "手机号码格式错误");
                    g_onFaceRegisterProcess = false;
                } else {
                    if (lastFrame.empty()) {
                        resetTips(false, "图片采集质量不合格，请取消后重试");
                        g_onFaceRegisterProcess = false;
                    } else {
                        resetTips(true, "信息查询中...");
                        enableRegisterBtn(false);
                        static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue([this, phoneNumber] {
                            const auto ret = faceGrant(lastFrame, phoneNumber.toStdString());
                            if (ret) {
                                resetTips(true, "录入成功");
                                std::this_thread::sleep_for(std::chrono::seconds(2));
                                this->hide();
                            } else {
                                resetTips(false, "未查询到配租信息，请联系窗口服务");
                            }
                            g_onFaceRegisterProcess = false;
                            this->enableRegisterBtn(true);
                        });
                    }
                }
            });
    registerBtn->setStyleSheet("background-color: rgb(13, 133, 255);");
#ifdef WIN32
    registerBtn->setFixedSize(120, 30);
#else
    registerBtn->setFixedSize(240, 60);
#endif
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(registerBtn);
}


void FaceRegister::showEvent(QShowEvent *) {
    g_closeFaceRecognition = true;
}

void FaceRegister::hideEvent(QHideEvent *) {
    g_closeFaceRecognition = false;

    if (nullptr != phoneNumberThird) {
        phoneNumberThird->setText("");
    }
    if (nullptr != phoneNumberSecond) {
        phoneNumberSecond->setText("");
    }
    if (nullptr != phoneNumberFirst) {
        phoneNumberFirst->setText("");
        phoneNumberFirst->setFocus();
    }
    if (nullptr != errorTips) {
        errorTips->setText("");
    }
    lastFrame.release();
}


FaceRegister::~FaceRegister() = default;
