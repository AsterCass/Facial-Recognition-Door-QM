#ifndef MAIN_PAGE_SETTING_FACE_H
#define MAIN_PAGE_SETTING_FACE_H

#include <QCheckBox>
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

    QWidget *faceThreshold = nullptr;
    QHBoxLayout *faceThresholdLayout = nullptr;
    QLabel *faceThresholdLabel = nullptr;
    QLineEditPro *faceThresholdInput = nullptr;
    QLabel *faceThresholdTips = nullptr;

    QWidget *faceThresholdNight = nullptr;
    QHBoxLayout *faceThresholdNightLayout = nullptr;
    QLabel *faceThresholdNightLabel = nullptr;
    QLineEditPro *faceThresholdNightInput = nullptr;
    QLabel *faceThresholdNightTips = nullptr;

    QWidget *faceRegCount = nullptr;
    QHBoxLayout *faceRegCountLayout = nullptr;
    QLabel *faceRegCountLabel = nullptr;
    QLineEditPro *faceRegCountInput = nullptr;
    QLabel *faceRegCountTips = nullptr;

    QWidget *faceRegIvSec = nullptr;
    QHBoxLayout *faceRegIvSecLayout = nullptr;
    QLabel *faceRegIvSecLabel = nullptr;
    QLineEditPro *faceRegIvSecInput = nullptr;
    QLabel *faceRegIvSecTips = nullptr;

    QWidget *fullFaceCompare = nullptr;
    QHBoxLayout *fullFaceCompareLayout = nullptr;
    QLabel *fullFaceCompareLabel = nullptr;
    QCheckBox *fullFaceCompareInput = nullptr;

    QWidget *faceDistant = nullptr;
    QHBoxLayout *faceDistantLayout = nullptr;
    QLabel *faceDistantLabel = nullptr;
    QLineEditPro *faceDistantInput = nullptr;


    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_FACE_H
