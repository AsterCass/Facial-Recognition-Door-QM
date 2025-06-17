#ifndef MAIN_PAGE_SETTING_MAIN_H
#define MAIN_PAGE_SETTING_MAIN_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingMain final : public QWidget {
public:
    explicit MainSettingMain(QWidget *parent = nullptr);

    ~MainSettingMain() override;

private:
    QVBoxLayout *mainLayout = nullptr;


    QWidget *settingBody = nullptr;
    QGridLayout *settingMainLayout = nullptr;


    QToolButton *settingInfo = nullptr;
    QToolButton *settingNetwork = nullptr;
    QToolButton *settingLog = nullptr;
    QToolButton *settingFace = nullptr;
    QToolButton *settingSystem = nullptr;
    QToolButton *settingServer = nullptr;
    QToolButton *settingDoor = nullptr;
    QToolButton *settingMaintain = nullptr;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_MAIN_H
