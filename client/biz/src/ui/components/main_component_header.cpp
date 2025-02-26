#include "ui/components/main_component_header.h"


MainComponentHeader::MainComponentHeader(QWidget *parent): QWidget(parent) {
    if (parent == nullptr) {
        return;
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
        mainLayout->setSpacing(5);
        mainLayout->setContentsMargins(5, 0, 5, 0);
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
            font.setPointSize(6);
            time->setFont(font);

            mainLayout->addWidget(time);
        }

        // Hide setting btn
        {
        }

        // Spacer
        {
            mainSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
            mainLayout->addItem(mainSpacer);
        }

        // Wired
        {
            wiredIcon = new QToolButton();
            wiredIcon->setIcon(QIcon(":/assets/images/status-wired-no.svg"));
            wiredIcon->setIconSize(QSize(40, 40));
            wiredIcon->setFocusPolicy(Qt::NoFocus);
            wiredIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wiredIcon);
        }

        // Wireless
        {
            wirelessIcon = new QToolButton();
            wirelessIcon->setIcon(QIcon(":/assets/images/status-wireless-no.svg"));
            wirelessIcon->setIconSize(QSize(40, 40));
            wirelessIcon->setFocusPolicy(Qt::NoFocus);
            wirelessIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(wirelessIcon);
        }

        // 4G
        {
            fourGIcon = new QToolButton();
            fourGIcon->setIcon(QIcon(":/assets/images/status-4g-no.svg"));
            fourGIcon->setIconSize(QSize(40, 40));
            fourGIcon->setFocusPolicy(Qt::NoFocus);
            fourGIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(fourGIcon);
        }

        // Server
        {
            serverIcon = new QToolButton();
            serverIcon->setIcon(QIcon(":/assets/images/status-server-no.svg"));
            serverIcon->setIconSize(QSize(40, 40));
            serverIcon->setFocusPolicy(Qt::NoFocus);
            serverIcon->setStyleSheet("background-color: transparent;border: none;");
            mainLayout->addWidget(serverIcon);
        }
    }
}


MainComponentHeader::~MainComponentHeader() = default;
