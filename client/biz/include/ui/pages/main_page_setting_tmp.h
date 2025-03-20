#ifndef MAIN_PAGE_SETTING_TMP_H
#define MAIN_PAGE_SETTING_TMP_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>


class MainSettingTmp final : public QWidget {
public:
    explicit MainSettingTmp(QWidget *parent = nullptr);

    ~MainSettingTmp() override;

private:
    QVBoxLayout *mainLayout = nullptr;

    QScrollArea *mainScrollArea = nullptr;
    QVBoxLayout *scrollerAreaLayout = nullptr;

    QWidget *scrollContent = nullptr;


    QLabel *ipWiredLabel;
    QLabel *ipWirelessLabel;
    QLabel *ipFourGLabel;

    QPushButton *saveRebootBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
};


#endif //MAIN_PAGE_SETTING_TMP_H
