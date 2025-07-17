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

    QWidget *voice = nullptr;
    QHBoxLayout *voiceLayout = nullptr;
    QLabel *voiceLabel = nullptr;
    QPushButton *voiceInput = nullptr;
    int voiceValue = 0;

    QWidget *voiceVol = nullptr;
    QHBoxLayout *voiceVolLayout = nullptr;
    QLabel *voiceVolLabel = nullptr;
    QLineEditPro *voiceVolInput = nullptr;

    QWidget *faceRegCoreIvMillSecTime = nullptr;
    QHBoxLayout *faceRegCoreIvMillSecTimeLayout = nullptr;
    QLabel *faceRegCoreIvMillSecTimeLabel = nullptr;
    QLineEditPro *faceRegCoreIvMillSecTimeInput = nullptr;
    QLabel *faceRegCoreIvMillSecTimeTips = nullptr;

    QWidget *taskIvSecTime = nullptr;
    QHBoxLayout *taskIvSecTimeLayout = nullptr;
    QLabel *taskIvSecTimeLabel = nullptr;
    QLineEditPro *taskIvSecTimeInput = nullptr;
    QLabel *taskIvSecTimeTips = nullptr;

    QWidget *lightOnlyCheck = nullptr;
    QHBoxLayout *lightOnlyCheckLayout = nullptr;
    QLabel *lightOnlyCheckLabel = nullptr;
    QPushButton *lightOnlyCheckInput = nullptr;
    int lightOnlyCheckValue = 0;

    QWidget *needBackup = nullptr;
    QHBoxLayout *needBackupLayout = nullptr;
    QLabel *needBackupLabel = nullptr;
    QPushButton *needBackupInput = nullptr;
    int needBackupValue = 0;
    QLabel *needBackupTips = nullptr;

    SettingHeader *settingHeader = nullptr;
    MainComponentHeader *mainHeader = nullptr;
};


#endif //MAIN_PAGE_SETTING_SYSTEM_H
