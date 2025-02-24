#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QLineEdit>


class QLabel;

class Notification : public QWidget {
public:
    Notification(const Notification &) = delete;

    Notification &operator=(const Notification &) = delete;

    static Notification *getInstance(QWidget *parent) {
        static Notification instance(parent);
        return &instance;
    }

    void setMessage(const QString &message);

private:
    explicit Notification(QWidget *parent = nullptr);

    ~Notification() override;

private:
    QLabel *messageLabel = nullptr;
};


#endif // NOTIFICATION_H
