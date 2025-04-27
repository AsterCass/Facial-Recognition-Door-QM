#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>


class Notification final : public QWidget {
public:
    explicit Notification(QWidget *parent = nullptr);

    ~Notification() override;

    void setMessage(const std::string &message, const std::function<void(bool)> &callback);

private:
    QVBoxLayout *mainLayout = nullptr;
    QWidget *mainScreenWidget = nullptr;

    QVBoxLayout *notificationLayout = nullptr;
    QWidget *notificationWidget = nullptr;

    QWidget *btnWidget = nullptr;
    QHBoxLayout *btnLayout = nullptr;

    QLabel *messageLabel = nullptr;
    QPushButton *confirmBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    std::function<void(bool)> confirmCallback = nullptr;
};


#endif // NOTIFICATION_H
