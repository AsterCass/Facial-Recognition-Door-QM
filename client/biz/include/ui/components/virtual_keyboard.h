#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <QLineEdit>
#include <QVBoxLayout>
#include <iostream>
#include <QPushButton>
#include <QMouseEvent>
#include "airstrip_log.h"

class VirtualKeyboardMaskEventFilter final : public QObject {
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
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

    static VirtualKeyboard *getInstance(QWidget *parent = nullptr) {
        static VirtualKeyboard instance(parent);
        return &instance;
    }

    void hideKeyboard() {
        currentInput = nullptr;
        this->hide();
    }

    void showKeyboard(QLineEdit *input) {
        currentInput = input;
        this->show();
    }

private:
    explicit VirtualKeyboard(QWidget *parent = nullptr);

    ~VirtualKeyboard() override;

    void switchCaps();

    std::vector<std::vector<VirtualKeyboardKey> > keyRows = {
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
            {"Enter", "Enter"},
        },
        {
            {"Chinese", "Chinese"}, {"z", "Z"}, {"x", "X"},
            {"c", "C"}, {"v", "V"}, {"b", "B"},
            {"n", "N"}, {"m", "M"}, {",", "<"},
            {".", ">"}, {"/", "?"}, {"Chinese", "Chinese"},
        },
        {
            {"Hide", "Hide"}, {"Space Bar", "Space Bar"},
            {"Hide", "Hide"}
        },
    };
    QVBoxLayout *layout = nullptr;
    QWidget *mainScreenWidget = nullptr;
    VirtualKeyboardMaskEventFilter *mainScreenWidgetClickEvent = nullptr;
    QWidget *keyboardWidget = nullptr;
    QGridLayout *keyboardLayout = nullptr;
    QLineEdit *currentInput = nullptr;
    bool isCaps = false;
};


#endif // VIRTUAL_KEYBOARD_H
