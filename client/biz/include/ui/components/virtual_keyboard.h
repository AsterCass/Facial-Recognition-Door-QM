#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <QLineEdit>


class VirtualKeyboard : public QWidget {
public:
    VirtualKeyboard(const VirtualKeyboard &) = delete;

    VirtualKeyboard &operator=(const VirtualKeyboard &) = delete;

    static VirtualKeyboard *getInstance(QWidget *parent) {
        static VirtualKeyboard instance(parent);
        return &instance;
    }

private:
    explicit VirtualKeyboard(QWidget *parent = nullptr);

    ~VirtualKeyboard() override;

private slots:
    void handleFocusChanged(QWidget *old, QWidget *now);
};


#endif // VIRTUAL_KEYBOARD_H
