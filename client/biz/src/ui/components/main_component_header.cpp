#include "ui/components/main_component_header.h"

#include "airstrip_program_options.h"
#include "config/config.h"


MainComponentHeader::MainComponentHeader(QWidget *parent): QWidget(parent) {
    // Configuration
    {
        airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);
    }

    // Main widget
    {
        this->setAutoFillBackground(true);
        auto pal = this->palette();
        pal.setColor(QPalette::Window, QColor(4, 9, 12));
        this->setPalette(pal);
    }

    // Main layout
    {
        mainLayout = new QHBoxLayout(this);
        mainLayout->setSpacing(10);
        mainLayout->setContentsMargins(10, 0, 10, 0);
    }

    // Status bar
    {
        // Time
        {
            time = new QLabel(this);
            time->setAutoFillBackground(true);
            auto pal = time->palette();
            pal.setColor(QPalette::Window, QColor(4, 9, 12));
            pal.setColor(QPalette::WindowText, Qt::white);
            time->setPalette(pal);
            auto font = time->font();
            font.setPointSize(MINI_FONT_SIZE);
            time->setFont(font);

            mainLayout->addWidget(time);
        }

        // Hide setting btn
        {
            hideBtn = new QPushButton(this);
            hideBtn->setFocusPolicy(Qt::NoFocus);
            hideBtn->setStyleSheet("background-color: transparent;border: none;");
            connect(hideBtn, &QPushButton::clicked, this,
                    [=] {
                        ++tryGoManagementCount;
                    });
            mainLayout->addWidget(hideBtn);
        }

        // Spacer
        {
            mainSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
            mainLayout->addItem(mainSpacer);
        }

        // Wired
        {
            wiredIcon = new QToolButton();
            wiredIcon->setIcon(QIcon(QString::fromStdString(
                appWorkDir + "static/images/status-wired-no.svg")));
            wiredIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            wiredIcon->setFocusPolicy(Qt::NoFocus);
            wiredIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wiredIcon);
        }

        // Wireless
        {
            wirelessIcon = new QToolButton();
            wirelessIcon->setIcon(QIcon(QString::fromStdString(
                appWorkDir + "static/images/status-wireless-no.svg")));
            wirelessIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            wirelessIcon->setFocusPolicy(Qt::NoFocus);
            wirelessIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wirelessIcon);
        }

        // 4G
        {
            fourGIcon = new QToolButton();
            fourGIcon->setIcon(QIcon(QString::fromStdString(
                appWorkDir + "static/images/status-4g-no.svg")));
            fourGIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            fourGIcon->setFocusPolicy(Qt::NoFocus);
            fourGIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(fourGIcon);
        }

        // Server
        {
            serverIcon = new QToolButton();
            serverIcon->setIcon(QIcon(QString::fromStdString(
                appWorkDir + "static/images/status-server-no.svg")));
            serverIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            serverIcon->setFocusPolicy(Qt::NoFocus);
            serverIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(serverIcon);
        }
    }
}


MainComponentHeader::~MainComponentHeader() = default;
