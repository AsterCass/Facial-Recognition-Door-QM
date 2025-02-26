#ifndef MAIN_COMPONENT_HEADER_H
#define MAIN_COMPONENT_HEADER_H
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QToolButton>


class MainComponentHeader final : public QWidget {
public:
    MainComponentHeader(const MainComponentHeader &) = delete;

    MainComponentHeader &operator=(const MainComponentHeader &) = delete;

    static MainComponentHeader *getInstance(QWidget *parent = nullptr) {
        static MainComponentHeader instance(parent);
        return &instance;
    }

    void updateTimeText(const std::string &timeStr) const {
        if (nullptr == time) return;
        time->setText(QString::fromStdString(timeStr));
    }

    void updateWiredStatus(const bool connected) const {
        if (nullptr == wiredIcon) return;
        if (connected) {
            wiredIcon->setIcon(QIcon(":/assets/images/status-wired.svg"));
        } else {
            wiredIcon->setIcon(QIcon(":/assets/images/status-wired-no.svg"));
        }
    }

    void updateWirelessStatus(const bool connected) const {
        if (nullptr == wirelessIcon) return;
        if (connected) {
            wirelessIcon->setIcon(QIcon(":/assets/images/status-wireless.svg"));
        } else {
            wirelessIcon->setIcon(QIcon(":/assets/images/status-wireless-no.svg"));
        }
    }

    void update4GStatus(const bool connected) const {
        if (nullptr == fourGIcon) return;
        if (connected) {
            fourGIcon->setIcon(QIcon(":/assets/images/status-4g.svg"));
        } else {
            fourGIcon->setIcon(QIcon(":/assets/images/status-4g-no.svg"));
        }
    }

    void updateServerStatus(const bool connected) const {
        if (nullptr == serverIcon) return;
        if (connected) {
            serverIcon->setIcon(QIcon(":/assets/images/status-server.svg"));
        } else {
            serverIcon->setIcon(QIcon(":/assets/images/status-server-no.svg"));
        }
    }

private:
    explicit MainComponentHeader(QWidget *parent = nullptr);

    ~MainComponentHeader() override;

    QHBoxLayout *mainLayout = nullptr;
    QSpacerItem *mainSpacer = nullptr;

    QLabel *time = nullptr;

    QToolButton *wiredIcon = nullptr;
    QToolButton *wirelessIcon = nullptr;
    QToolButton *fourGIcon = nullptr;
    QToolButton *serverIcon = nullptr;
};

#endif // MAIN_COMPONENT_HEADER_H
