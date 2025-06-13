#include "ui/components/setting_header.h"

#include <QLabel>
#include <QWidget>


SettingHeader::SettingHeader(QWidget *parent, const std::string &titleStr,
                             QPushButton *btn): QWidget(parent) {
#ifdef WIN32
    setFixedHeight(90);
#else
    setFixedHeight(180);
#endif
    setStyleSheet("background-color: rgb(4, 9, 12);");

    // 主布局（用于撑满宽度）
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧容器（左按钮垂直居中）
    leftWidget = new QWidget(this);
    leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(20, 0, 0, 0);
    leftLayout->addStretch();
    leftBtn = new QPushButton("返回", leftWidget);
    leftBtn->setStyleSheet(R"(
    QPushButton {
        border: none;
        background: transparent;
        color: rgb(13, 133, 255);
        font-size: 24px;
        font: inherit;
    }
    QPushButton:hover {
        text-decoration: underline;
    }
)");
    leftLayout->addWidget(leftBtn, 0, Qt::AlignLeft);
    leftLayout->addStretch();

    // 中间容器：中心 label 垂直+水平都居中
    centerWidget = new QWidget(this);
    centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->addStretch();
    centerLabel = new QLabel(QString::fromStdString(titleStr), centerWidget);
    centerLabel->setAlignment(Qt::AlignCenter);
#ifdef WIN32
    centerLabel->setStyleSheet("color: white; font-size: 20px;");
#else
    centerLabel->setStyleSheet("color: white; font-size: 40px;");
#endif
    centerLayout->addWidget(centerLabel, 0, Qt::AlignHCenter);
    centerLayout->addStretch();

    // 右侧容器
    rightWidget = new QWidget(this);
    rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 20, 0);
    rightLayout->addStretch();
    if (btn) {
        rightLayout->addWidget(btn, 0, Qt::AlignRight);
    }
    rightLayout->addStretch();

    // 加入主布局（左右中顺序）
    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(centerWidget, 1);
    mainLayout->addWidget(rightWidget, 1);

    setLayout(mainLayout);
}


SettingHeader::~SettingHeader() = default;
