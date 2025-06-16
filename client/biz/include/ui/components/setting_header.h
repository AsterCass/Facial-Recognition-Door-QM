#ifndef SETTING_HEADER_H
#define SETTING_HEADER_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>


class SettingHeader final : public QWidget {
public:
    explicit SettingHeader(QWidget *parent = nullptr, const std::string &titleStr = "",
                           QPushButton *btn = nullptr);

    ~SettingHeader() override;

private:
    QHBoxLayout *mainLayout = nullptr;
    QWidget *mainScreenWidget = nullptr;


    QWidget *centerWidget = nullptr;
    QVBoxLayout *centerLayout = nullptr;
    QLabel *centerLabel = nullptr;

    QWidget *leftWidget = nullptr;
    QVBoxLayout *leftLayout = nullptr;
    QPushButton *leftBtn = nullptr;

    QWidget *rightWidget = nullptr;
    QVBoxLayout *rightLayout = nullptr;
};


#endif // SETTING_HEADER_H
