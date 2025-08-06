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

    QWidget *faceRegIvSec = nullptr;
    QHBoxLayout *faceRegIvSecLayout = nullptr;
    QLabel *faceRegIvSecLabel = nullptr;
    QLineEditPro *faceRegIvSecInput = nullptr;
    QLabel *faceRegIvSecTips = nullptr;

    QWidget *faceRegCount = nullptr;
    QHBoxLayout *faceRegCountLayout = nullptr;
    QLabel *faceRegCountLabel = nullptr;
    QLineEditPro *faceRegCountInput = nullptr;
    QLabel *faceRegCountTips = nullptr;


    QWidget *enableFaceSpoof = nullptr;
    QHBoxLayout *enableFaceSpoofLayout = nullptr;
    QLabel *enableFaceSpoofLabel = nullptr;
    QPushButton *enableFaceSpoofInput = nullptr;
    int enableFaceSpoofValue = 0;

    QWidget *showFaceRect = nullptr;
    QHBoxLayout *showFaceRectLayout = nullptr;
    QLabel *showFaceRectLabel = nullptr;
    QPushButton *showFaceRectInput = nullptr;
    int showFaceRectValue = 0;

    QWidget *longDistanceDetect = nullptr;
    QHBoxLayout *longDistanceDetectLayout = nullptr;
    QLabel *longDistanceDetectLabel = nullptr;
    QPushButton *longDistanceDetectInput = nullptr;
    int longDistanceDetectValue = 0;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_FACE_H
