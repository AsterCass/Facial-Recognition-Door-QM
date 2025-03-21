#ifndef MAIN_PAGE_SETTING_TMP_H
#define MAIN_PAGE_SETTING_TMP_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>

#include "ui/components/common_components.h"


class MainSettingTmp final : public QWidget {
public:
    explicit MainSettingTmp(QWidget *parent);

    ~MainSettingTmp() override;

private:
    QVBoxLayout *mainLayout;

    QScrollArea *mainScrollArea;
    QVBoxLayout *scrollerAreaLayout;

    QWidget *scrollContent;

    QLabel *serverAddressLabel;
    QLineEditPro *serverAddress;
    QLabel *faceThresholdLabel;
    QLineEditPro *faceThreshold;
    QLabel *volLabel;
    QLineEditPro *vol;
    QLabel *faceDistantLabel;
    QWidget *faceDistantWidget;
    QHBoxLayout *faceDistantLayout;
    QRadioButton *faceDistantLess;
    QRadioButton *faceDistantMid;
    QRadioButton *faceDistantMore;
    QButtonGroup *faceDistantGroup;

    QCheckBox *enableFaceSpoof;
    QCheckBox *lightOnlyCheck;

    QLabel *netModelLabel;
    QWidget *netModelWidget;
    QHBoxLayout *netModelLayout;
    QRadioButton *netModelWired;
    QRadioButton *netModelWireless;
    QRadioButton *netModelFourG;
    QButtonGroup *netModelGroup;

    QLabel *wifiAccountLabel;
    QLineEditPro *wifiAccount;
    QLabel *wifiPasswdLabel;
    QLineEditPro *wifiPasswdEdit;


    QLabel *ipWiredLabel;
    QLabel *ipWirelessLabel;
    QLabel *ipFourGLabel;

    QPushButton *saveRebootBtn;
    QPushButton *checkUpdateBtn;
    QPushButton *cancelBtn;
};


#endif //MAIN_PAGE_SETTING_TMP_H
