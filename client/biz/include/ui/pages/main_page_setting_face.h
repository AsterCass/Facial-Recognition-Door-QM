#ifndef MAIN_PAGE_SETTING_FACE_H
#define MAIN_PAGE_SETTING_FACE_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingFace final : public QWidget {
public:
    explicit MainSettingFace(QWidget *parent = nullptr);

    ~MainSettingFace() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QPushButton *save = nullptr;


    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_FACE_H
