#ifndef MAIN_PAGE_SETTING_TMP_H
#define MAIN_PAGE_SETTING_TMP_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <QRadioButton>
#include <QCheckBox>

#include "ui/components/common_components.h"


class MainSettingTmp final : public QWidget {
public:
    explicit MainSettingTmp(QWidget *parent = nullptr);

    ~MainSettingTmp() override;

private:
    QVBoxLayout *mainLayout = nullptr;

    QScrollArea *mainScrollArea = nullptr;
    QVBoxLayout *scrollerAreaLayout = nullptr;

    QWidget *scrollContent = nullptr;

    QLabel *faceThresholdLabel = nullptr;
    QLineEditPro *faceThreshold = nullptr;
    QLabel *volLabel = nullptr;
    QLineEditPro *vol = nullptr;
    QLabel *faceDistantLabel = nullptr;
    QWidget *faceDistantWidget = nullptr;
    QHBoxLayout *faceDistantLayout = nullptr;
    QRadioButton *faceDistantLess = nullptr;
    QRadioButton *faceDistantMid = nullptr;
    QRadioButton *faceDistantMore = nullptr;

    QCheckBox *enableLiveness = nullptr;
    QCheckBox *lightOnlyCheck = nullptr;

    QLabel *netModelLabel = nullptr;
    QWidget *netModelWidget = nullptr;
    QHBoxLayout *netModelLayout = nullptr;
    QRadioButton *netModelWired = nullptr;
    QRadioButton *netModelWireless = nullptr;
    QRadioButton *netModelFourG = nullptr;

    QLabel *wifiAccountLabel = nullptr;
    QLineEditPro *wifiAccount = nullptr;
    QLabel *wifiPasswdLabel = nullptr;
    QLineEditPro *wifiPasswdEdit = nullptr;


    QLabel *ipWiredLabel;
    QLabel *ipWirelessLabel;
    QLabel *ipFourGLabel;

    QWidget *bottomWidget = nullptr;

    QPushButton *saveRebootBtn = nullptr;
    QPushButton *checkUpdateBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
};


#endif //MAIN_PAGE_SETTING_TMP_H
