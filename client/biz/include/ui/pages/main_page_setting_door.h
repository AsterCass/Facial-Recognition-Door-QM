#ifndef MAIN_PAGE_SETTING_DOOR_H
#define MAIN_PAGE_SETTING_DOOR_H

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/components/common_components.h>

#include "ui/components/main_component_header.h"
#include "ui/components/setting_header.h"


class MainSettingDoor final : public QWidget {
public:
    explicit MainSettingDoor(QWidget *parent = nullptr);

    ~MainSettingDoor() override;

private:
    void showEvent(QShowEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;

    QWidget *body = nullptr;
    QVBoxLayout *bodyLayout = nullptr;

    QPushButton *save = nullptr;

    QWidget *doorOpenTime = nullptr;
    QHBoxLayout *doorOpenTimeLayout = nullptr;
    QLabel *doorOpenTimeLabel = nullptr;
    QLineEditPro *doorOpenTimeInput = nullptr;

    QWidget *cardOpen = nullptr;
    QHBoxLayout *cardOpenLayout = nullptr;
    QLabel *cardOpenLabel = nullptr;
    QPushButton *cardOpenInput = nullptr;
    int cardOpenValue = 0;

    QWidget *faceOpen = nullptr;
    QHBoxLayout *faceOpenLayout = nullptr;
    QLabel *faceOpenLabel = nullptr;
    QPushButton *faceOpenInput = nullptr;
    int faceOpenValue = 0;

    QWidget *showRgb = nullptr;
    QHBoxLayout *showRgbLayout = nullptr;
    QLabel *showRgbLabel = nullptr;
    QPushButton *showRgbInput = nullptr;
    int showRgbValue = 0;

    QWidget *enableIrLed = nullptr;
    QHBoxLayout *enableIrLedLayout = nullptr;
    QLabel *enableIrLedLabel = nullptr;
    QPushButton *enableIrLedInput = nullptr;
    int enableIrLedValue = 0;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_DOOR_H
