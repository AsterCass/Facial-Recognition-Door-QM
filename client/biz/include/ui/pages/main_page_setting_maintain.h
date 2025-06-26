#ifndef MAIN_PAGE_SETTING_MAINTAIN_H
#define MAIN_PAGE_SETTING_MAINTAIN_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingMaintain final : public QWidget {
public:
    explicit MainSettingMaintain(QWidget *parent = nullptr);

    ~MainSettingMaintain() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QPushButton *save = nullptr;


    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_MAINTAIN_H
