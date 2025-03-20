#ifndef MAIN_COMPONENT_HEADER_H
#define MAIN_COMPONENT_HEADER_H
#include <iostream>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QToolButton>
#include "config/config.h"


class MainComponentHeader final : public QWidget {
public:
    explicit MainComponentHeader(QWidget *parent = nullptr);

    ~MainComponentHeader() override;

private:
    void updateTimeText(const std::string &timeStr) const {
        if (nullptr == time) return;
        time->setText(QString::fromStdString(timeStr));
    }

    void updateWiredStatus(const std::string &ip) const {
        if (nullptr == wiredIcon) return;
        if (!ip.empty()) {
            wiredIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-wired.svg")));
        } else {
            wiredIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-wired-no.svg")));
        }
    }

    void updateWirelessStatus(const std::string &ip) const {
        if (nullptr == wirelessIcon) return;
        if (!ip.empty()) {
            wirelessIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-wireless.svg")));
        } else {
            wirelessIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-wireless-no.svg")));
        }
    }

    void update4GStatus(const std::string &ip) const {
        if (nullptr == fourGIcon) return;
        if (!ip.empty()) {
            fourGIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-4g.svg")));
        } else {
            fourGIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-4g-no.svg")));
        }
    }

    void updateServerStatus(const bool connected) const {
        if (nullptr == serverIcon) return;
        if (connected) {
            serverIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-server.svg")));
        } else {
            serverIcon->setIcon(QIcon(QString::fromStdString(
                g_appWorkDir + "static/images/status-server-no.svg")));
        }
    }

    QHBoxLayout *mainLayout = nullptr;
    QSpacerItem *mainSpacer = nullptr;

    QLabel *time = nullptr;

    QPushButton *hideBtn = nullptr;

    QToolButton *wiredIcon = nullptr;
    QToolButton *wirelessIcon = nullptr;
    QToolButton *fourGIcon = nullptr;
    QToolButton *serverIcon = nullptr;
};

#endif // MAIN_COMPONENT_HEADER_H
