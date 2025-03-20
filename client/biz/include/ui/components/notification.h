#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QLabel>

class Notification final : public QWidget {
public:
    explicit Notification(QWidget *parent = nullptr);

    ~Notification() override;

    void setMessage(const QString &message);

private:
    QLabel *messageLabel = nullptr;
};


#endif // NOTIFICATION_H
