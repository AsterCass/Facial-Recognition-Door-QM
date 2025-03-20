#include "ui/components/main_component_header.h"

#include "airstrip_program_options.h"
#include "config/config.h"
#include "utils/global_data_manager.h"


MainComponentHeader::MainComponentHeader(QWidget *parent): QWidget(parent) {
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
#ifdef WIN32
            time->setStyleSheet("font-size: 12px");
#else
            time->setStyleSheet("font-size: 24px");
#endif
            auto pal = time->palette();
            pal.setColor(QPalette::Window, QColor(4, 9, 12));
            pal.setColor(QPalette::WindowText, Qt::white);
            time->setPalette(pal);
            mainLayout->addWidget(time);
        }

        // Hide setting btn
        {
            hideBtn = new QPushButton(this);
            hideBtn->setFocusPolicy(Qt::NoFocus);
            hideBtn->setStyleSheet("background-color: transparent;border: none; width: 100px");
            hideBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(hideBtn, &QPushButton::clicked, this,
                    [=] {
                        ++g_tryGoManagementCount;
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
                g_appWorkDir + "static/images/status-wired-no.svg")));
            wiredIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            wiredIcon->setFocusPolicy(Qt::NoFocus);
            wiredIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wiredIcon);
        }

        // Wireless
        {
            wirelessIcon = new QToolButton();
            wirelessIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-wireless-no.svg")));
            wirelessIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            wirelessIcon->setFocusPolicy(Qt::NoFocus);
            wirelessIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wirelessIcon);
        }

        // 4G
        {
            fourGIcon = new QToolButton();
            fourGIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-4g-no.svg")));
            fourGIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            fourGIcon->setFocusPolicy(Qt::NoFocus);
            fourGIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(fourGIcon);
        }

        // Server
        {
            serverIcon = new QToolButton();
            serverIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-server-no.svg")));
            serverIcon->setIconSize(QSize(MINI_ICON_SIZE, MINI_ICON_SIZE));
            serverIcon->setFocusPolicy(Qt::NoFocus);
            serverIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(serverIcon);
        }
    }

    // Connect
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerTimeChange,
            this, &MainComponentHeader::updateTimeText);
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWiredChange,
            this, &MainComponentHeader::updateWiredStatus);
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWirelessChange,
            this, &MainComponentHeader::updateWirelessStatus);
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerFourGChange,
            this, &MainComponentHeader::update4GStatus);
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerServerChange,
            this, &MainComponentHeader::updateServerStatus);
}


MainComponentHeader::~MainComponentHeader() = default;
