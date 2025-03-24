#ifndef VIRTUAL_KEYBOARD_NUMBER_H
#define VIRTUAL_KEYBOARD_NUMBER_H

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class VirtualKeyboardNumberMaskEventFilter final : public QObject {
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};


class VirtualKeyboardNumber final : public QWidget {
public:
    VirtualKeyboardNumber(const VirtualKeyboardNumber &) = delete;

    VirtualKeyboardNumber &operator=(const VirtualKeyboardNumber &) = delete;

    static VirtualKeyboardNumber *getInstance(QWidget *parent = nullptr) {
        static VirtualKeyboardNumber instance(parent);
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

    void switchCurrentInput(QLineEdit *input) {
        currentInput = input;
    }

private:
    explicit VirtualKeyboardNumber(QWidget *parent = nullptr);

    ~VirtualKeyboardNumber() override;

    std::array<QPushButton *, 10> numberBtn = {};
    QPushButton *hideBtn = nullptr;
    QPushButton *deleteBtn = nullptr;

    QVBoxLayout *layout = nullptr;
    QWidget *mainScreenWidget = nullptr;
    VirtualKeyboardNumberMaskEventFilter *mainScreenWidgetClickEvent = nullptr;
    QWidget *keyboardWidget = nullptr;
    QGridLayout *keyboardLayout = nullptr;
    QLineEdit *currentInput = nullptr;
};


#endif // VIRTUAL_KEYBOARD_NUMBER_H
