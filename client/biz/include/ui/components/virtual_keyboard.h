#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <QLineEdit>
#include <QVBoxLayout>
#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <QScreen>
#include <QDebug>
#include <QMouseEvent>
#include "airstrip_log.h"

class VirtualKeyboardMaskEventFilter final : public QObject {
protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            const auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                airstrip::logPrintln(watched->objectName().toStdString(), INFO, __FUNCTION__);
                return true;
            }
        }
        // 继续传播事件
        return QObject::eventFilter(watched, event);
    }
};

typedef struct VirtualKeyboardKey {
    std::string key;
    std::string keyCap;
    QPushButton *btn;
} VirtualKeyboardKey;


class VirtualKeyboard final : public QWidget {
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

private:
    const std::vector<std::vector<VirtualKeyboardKey> > keyRows = {
        {
            {"`", "~"}, {"1", "!"}, {"2", "@"},
            {"3", "#"}, {"4", "$"}, {"5", "%"},
            {"6", "^"}, {"7", "&"}, {"8", "*"},
            {"9", "("}, {"0", ")"}, {"-", "_"},
            {"=", "+"}, {"Backspace", "Backspace"},
        },
        {
            {"Tab", "Tab"}, {"q", "Q"}, {"w", "W"},
            {"e", "E"}, {"r", "R"}, {"t", "T"},
            {"y", "Y"}, {"u", "U"}, {"i", "I"},
            {"o", "O"}, {"p", "P"}, {"[", "{"},
            {"]", "}"}, {"\\", "|"},
        },
        {
            {"Caps", "Caps"}, {"a", "A"}, {"s", "S"},
            {"d", "D"}, {"f", "F"}, {"g", "G"},
            {"h", "H"}, {"j", "J"}, {"k", "K"},
            {"l", "L"}, {";", ":"}, {"'", "\""},
            {"Enter"},
        },
        {
            {"Chinese", "Chinese"}, {"z", "Z"}, {"x", "X"},
            {"c", "C"}, {"v", "V"}, {"b", "B"},
            {"n", "N"}, {"m", "M"}, {",", "<"},
            {".", ">"}, {"/", "?"}, {"Chinese", "Chinese"},
        },
        {
            {"Hide", "Hide"}, {"Space Bar", "Space Bar"}, {"Hide", "Hide"}
        },
    };
    QVBoxLayout *layout = nullptr;
    QWidget *mainScreenWidget = nullptr;
    VirtualKeyboardMaskEventFilter *mainScreenWidgetClickEvent = nullptr;
    QWidget *keyboardWidget = nullptr;
    QGridLayout *keyboardLayout = nullptr;
};


#endif // VIRTUAL_KEYBOARD_H
