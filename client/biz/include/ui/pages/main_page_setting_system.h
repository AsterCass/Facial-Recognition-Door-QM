#ifndef MAIN_PAGE_SETTING_SYSTEM_H
#define MAIN_PAGE_SETTING_SYSTEM_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingSystem final : public QWidget {
public:
    explicit MainSettingSystem(QWidget *parent = nullptr);

    ~MainSettingSystem() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QPushButton *save = nullptr;

    QLabel *inDevelop = nullptr;


    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_SYSTEM_H
