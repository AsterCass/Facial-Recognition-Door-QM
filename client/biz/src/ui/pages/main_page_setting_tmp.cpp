#include "ui/pages/main_page_setting_tmp.h"

#include <airstrip_thread_pool.h>

#include "airstrip_log.h"
#include "ui/main_router.h"
#include "utils/global_data_manager.h"
#include <sstream>
#include <api/api.h>
#include <boost/json.hpp>
#include <utils/face_recognition.h>

#include "airstrip_command.h"
#include "config/config.h"


using namespace std;

std::string checkExtension(const std::string &filename) {
    if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".zip") == 0) {
        return ".zip";
    }
    if (filename.size() >= 7 && filename.compare(filename.size() - 7, 7, ".tar.gz") == 0) {
        return ".tar.gz";
    }
    if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".tar") == 0) {
        return ".tar";
    }
    return "";
}

void updateVersion(const bool confirm) {
    if (!confirm) {
        return;
    }
    logPrintln("Prepare to update ...", airstrip::INFO, __FUNCTION__);

    std::ostringstream oss;
    oss << "sh " << g_appWorkDir << "script/linux/update.sh";

    const auto suffix = checkExtension(g_prepareUpdateUrl);
    if (suffix.empty() || g_prepareUpdateVersion.empty()) {
        logPrintln(
            "Update not support for url: " + g_prepareUpdateUrl + " version: " + g_prepareUpdateVersion,
            airstrip::INFO, __FUNCTION__
        );
        return;
    }

    oss << " " << g_appWorkDir << "app-" << g_prepareUpdateVersion << suffix << " " << g_prepareUpdateUrl;

    logPrintln("Update command : " + oss.str(), airstrip::INFO, __FUNCTION__);
#ifndef WIN32
    airstrip::execCommandNoReturn(oss.str());
#endif
}

MainSettingTmp::MainSettingTmp(QWidget *parent): QWidget(parent) {
    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainScrollArea = new QScrollArea(this);
    mainScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setObjectName("mainSettingTmpMain");
    mainScrollArea->setStyleSheet("#mainSettingTmpMain{background-color: white}");
    mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef WIN32
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 15px;
        background: #f0f0f0;
    }

    QScrollBar::handle:vertical {
        background: #666666;
    }

    QScrollBar::handle:vertical:hover {
        background: #333333;
    }

)");
#else
    mainScrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 30px;
        background: #f0f0f0;
    }

    QScrollBar::handle:vertical {
        background: #666666;
    }

    QScrollBar::handle:vertical:hover {
        background: #333333;
    }

)");
#endif

    mainLayout->addWidget(mainScrollArea);

    scrollContent = new QWidget(mainScrollArea);
    mainScrollArea->setWidget(scrollContent);

    // Content
    scrollerAreaLayout = new QVBoxLayout(scrollContent);
    scrollerAreaLayout->setSpacing(20);
    scrollerAreaLayout->setMargin(20);

    serverAddressLabel = new QLabel("服务器地址：", scrollContent);
    serverAddress = new QLineEditPro(scrollContent);
    faceThresholdLabel = new QLabel("人脸识别阈值（0 - 0.6）（推荐 0.48）（重启生效）：", scrollContent);
    faceThresholdLabel->setWordWrap(true);
    faceThreshold = new QLineEditPro(scrollContent);
    faceThresholdNightLabel = new QLabel("夜间人脸识别阈值（0 - 0.6）（推荐 0.42）（重启生效）：", scrollContent);
    faceThresholdNightLabel->setWordWrap(true);
    faceThresholdNight = new QLineEditPro(scrollContent);
    fullFaceCompare = new QCheckBox("核验人脸全量比对（重启生效）", scrollContent);
    volLabel = new QLabel("设备音量（0 - 100）：", scrollContent);
    vol = new QLineEditPro(scrollContent);
    faceRegCoreIvMillSecLabel = new QLabel("机器识别超频（0-1000，推荐 500）（0为极致超频，高温下可能会过热关机）：", scrollContent);
    faceRegCoreIvMillSecLabel->setWordWrap(true);
    faceRegCoreIvMillSec = new QLineEditPro(scrollContent);
    faceRegCountLabel = new QLabel("失败N次触发人脸信息验证：", scrollContent);
    faceRegCount = new QLineEditPro(scrollContent);
    taskIvSecLabel = new QLabel("获取任务间隔秒数（最小为5）：", scrollContent);
    taskIvSec = new QLineEditPro(scrollContent);
    faceRegIvSecLabel = new QLabel("N秒内不重复报错：", scrollContent);
    faceRegIvSec = new QLineEditPro(scrollContent);
    camExposeLabel = new QLabel("摄像头曝光量（需禁用自动调光，范围 1-1121，默认1121）：", scrollContent);
    camExposeLabel->setWordWrap(true);
    camExpose = new QLineEditPro(scrollContent);
    camGainLabel = new QLabel("摄像头进光量（需禁用自动调光，范围 64-300，默认64）：", scrollContent);
    camGainLabel->setWordWrap(true);
    camGain = new QLineEditPro(scrollContent);
    camLightLabel = new QLabel("补光灯亮度（需禁用自动调光，范围 0-255，默认0）：", scrollContent);
    camLightLabel->setWordWrap(true);
    camLight = new QLineEditPro(scrollContent);


    faceDistantLabel = new QLabel("人脸识别距离：", scrollContent);;
    faceDistantWidget = new QWidget(scrollContent);
    faceDistantLayout = new QHBoxLayout(faceDistantWidget);
    faceDistantLess = new QRadioButton("近", faceDistantWidget);
    faceDistantMid = new QRadioButton("中", faceDistantWidget);
    faceDistantMore = new QRadioButton("远", faceDistantWidget);
    faceDistantLayout->addWidget(faceDistantLess);
    faceDistantLayout->addWidget(faceDistantMid);
    faceDistantLayout->addWidget(faceDistantMore);
    faceDistantGroup = new QButtonGroup(faceDistantWidget);
    faceDistantGroup->addButton(faceDistantLess, 1);
    faceDistantGroup->addButton(faceDistantMid, 2);
    faceDistantGroup->addButton(faceDistantMore, 3);


    netModelLabel = new QLabel("网络模式（4G和无线不能同时开启）：", scrollContent);
    netModelWidget = new QWidget(scrollContent);
    netModelLayout = new QHBoxLayout(netModelWidget);
    netModelWired = new QRadioButton("有线", netModelWidget);
    netModelWireless = new QRadioButton("无线", netModelWidget);
    netModelFourG = new QRadioButton("4G", netModelWidget);
    netModelLayout->addWidget(netModelWired);
    netModelLayout->addWidget(netModelWireless);
    netModelLayout->addWidget(netModelFourG);
    netModelGroup = new QButtonGroup(faceDistantWidget);
    netModelGroup->addButton(netModelWired, 1);
    netModelGroup->addButton(netModelWireless, 2);
    netModelGroup->addButton(netModelFourG, 3);

    lightThresholdLabel = new QLabel("判亮阈值（0-255）：", scrollContent);
    lightThresholdInput = new QLineEditPro(scrollContent);
    darkThresholdLabel = new QLabel("判暗阈值（0-255）：", scrollContent);
    darkThresholdInput = new QLineEditPro(scrollContent);
    lightRatioLabel = new QLabel("升亮点（0-1）：", scrollContent);
    lightRatioInput = new QLineEditPro(scrollContent);
    darkRatioLabel = new QLabel("降亮点（0-1）：", scrollContent);
    darkRatioInput = new QLineEditPro(scrollContent);

    wifiAccountLabel = new QLabel("WIFI账号：", scrollContent);
    wifiAccount = new QLineEditPro(scrollContent);
    wifiPasswdLabel = new QLabel("WIFI密码：", scrollContent);
    wifiPasswdEdit = new QLineEditPro(scrollContent);
    wifiPasswdEdit->setEchoMode(QLineEdit::Password);

    enableFaceSpoof = new QCheckBox("活体验证（仅供调试，用户需开启）", scrollContent);
    lightOnlyCheck = new QCheckBox("仅在核验时开启补光灯", scrollContent);
    showConfUser = new QCheckBox("核验通过显示用户名和置信", scrollContent);
    camAutoLight = new QCheckBox("自动调光（仅供调试，用户需开启）", scrollContent);

    ipWiredLabel = new QLabel("有线IP地址：", scrollContent);
    ipWirelessLabel = new QLabel("无线IP地址：", scrollContent);
    ipFourGLabel = new QLabel("4GIP地址：", scrollContent);

    saveBtn = new QPushButton("保存", scrollContent);
    connect(saveBtn, &QPushButton::clicked, this,
            [=] {
                try {
                    const auto newServerAddress = serverAddress->text().trimmed().toStdString();
                    if (g_serverAddress != newServerAddress) {
                        g_serverAddress = newServerAddress;
                        g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, g_serverAddress);
                    }

                    const auto newFaceThreshold = faceThreshold->text().trimmed().toDouble();
                    if (g_faceThreshold != newFaceThreshold) {
                        g_faceThreshold = newFaceThreshold;
                        g_commonDb.upsertConfig(PRO_DB_FACE_THRESHOLD, to_string(g_faceThreshold));
                    }

                    const auto newFaceThresholdNight = faceThresholdNight->text().trimmed().toDouble();
                    if (g_faceThresholdNight != newFaceThresholdNight) {
                        g_faceThresholdNight = newFaceThresholdNight;
                        g_commonDb.upsertConfig(
                            PRO_DB_FACE_THRESHOLD_NIG, to_string(g_faceThresholdNight));
                    }

                    const auto newVolNum = vol->text().trimmed().toInt();
                    if (g_volNum != newVolNum) {
                        g_volNum = newVolNum;
#ifndef WIN32
airstrip::execScript(g_appWorkDir + "script/linux/reset_vol.sh " + std::to_string(g_volNum));
#endif
                        g_commonDb.upsertConfig(PRO_DB_VOL_NUM, to_string(g_volNum));
                    }

                    const auto newFaceDistance = faceDistantGroup->checkedId();
                    if (g_faceDistance != newFaceDistance) {
                        g_faceDistance = newFaceDistance;
                        g_commonDb.upsertConfig(PRO_DB_FACE_DISTANCE, to_string(g_faceDistance));
                    }

                    const auto newNetModel = netModelGroup->checkedId();
                    if (g_netModel != newNetModel) {
                        g_netModel = newNetModel;
                        g_commonDb.upsertConfig(PRO_DB_NET_MODEL, to_string(g_netModel));
                    }

                    const auto lightThreshold = lightThresholdInput->text().trimmed().toDouble();
                    if (g_lightThreshold != lightThreshold) {
                        g_lightThreshold = lightThreshold;
                        g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_THRESHOLD, to_string(g_lightThreshold));
                    }

                    const auto darkThreshold = darkThresholdInput->text().trimmed().toDouble();
                    if (g_darkThreshold != darkThreshold) {
                        g_darkThreshold = darkThreshold;
                        g_commonDb.upsertConfig(PRO_DB_ENABLE_DARK_THRESHOLD, to_string(g_darkThreshold));
                    }

                    const auto lightRatio = lightRatioInput->text().trimmed().toDouble();
                    if (g_lightRatio != lightRatio) {
                        g_lightRatio = lightRatio;
                        g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_RATIO, to_string(g_lightRatio));
                    }

                    const auto darkRatio = darkRatioInput->text().trimmed().toDouble();
                    if (g_darkRatio != darkRatio) {
                        g_darkRatio = darkRatio;
                        g_commonDb.upsertConfig(PRO_DB_ENABLE_DARK_RATIO, to_string(g_darkRatio));
                    }

                    const auto newWifiAccount = wifiAccount->text().trimmed().toStdString();
                    if (g_wifiAccount != newWifiAccount) {
                        g_wifiAccount = newWifiAccount;
                        g_commonDb.upsertConfig(PRO_DB_WIFI_ACCOUNT, g_wifiAccount);
                    }


                    const auto newWifiPasswd = wifiPasswdEdit->text().trimmed().toStdString();
                    if (g_wifiPasswd != newWifiPasswd) {
                        g_wifiPasswd = newWifiPasswd;
                        g_commonDb.upsertConfig(PRO_DB_WIFI_PASSWD, g_wifiPasswd);
                    }


                    const auto newEnableFaceSpoof = enableFaceSpoof->isChecked() ? 1 : 0;
                    if (g_enableFaceSpoof != newEnableFaceSpoof) {
                        g_enableFaceSpoof = newEnableFaceSpoof;
                        g_commonDb.upsertConfig(
                            PRO_DB_ENABLE_FACE_SPOOF, to_string(g_enableFaceSpoof));
                    }

                    const auto newLightOnlyCheck = lightOnlyCheck->isChecked() ? 1 : 0;
                    if (g_lightOnlyCheck != newLightOnlyCheck) {
                        g_lightOnlyCheck = newLightOnlyCheck;
                        g_commonDb.upsertConfig(
                            PRO_DB_ENABLE_LIGHT_ONLY_CHECK, to_string(g_lightOnlyCheck));
                    }

                    const auto newFaceRegCount = faceRegCount->text().trimmed().toInt();
                    if (g_faceRegCount != newFaceRegCount) {
                        g_faceRegCount = newFaceRegCount;
                        g_commonDb.upsertConfig(PRO_DB_FACE_REG_COUNT, to_string(g_faceRegCount));
                    }

                    const auto newTaskIvSec = taskIvSec->text().trimmed().toInt();
                    if (g_taskIvSec != newTaskIvSec) {
                        g_taskIvSec = newTaskIvSec;
                        g_commonDb.upsertConfig(PRO_DB_TASK_IV_SEC, to_string(g_taskIvSec));
                    }

                    const auto newShowConfUser = showConfUser->isChecked() ? 1 : 0;
                    if (g_showConfUser != newShowConfUser) {
                        g_showConfUser = newShowConfUser;
                        g_commonDb.upsertConfig(
                            PRO_DB_SHOW_CONF_USER, to_string(g_showConfUser));
                    }

                    // Light
                    {
                        bool changeLightProperty = false;
                        const auto newCamAutoLight = camAutoLight->isChecked() ? 1 : 0;
                        if (g_camAutoLight != newCamAutoLight) {
                            changeLightProperty = true;
                            g_camAutoLight = newCamAutoLight;
                            g_commonDb.upsertConfig(
                                PRO_DB_CAM_AUTO_LIGHT, to_string(g_camAutoLight));
                        }

                        const auto newCamExpose = camExpose->text().trimmed().toInt();
                        if (g_camExpose != newCamExpose) {
                            changeLightProperty = true;
                            g_camExpose = newCamExpose;
                            g_commonDb.upsertConfig(PRO_DB_CAM_EXPOSE, to_string(g_camExpose));
                        }
                        const auto newCamGain = camGain->text().trimmed().toInt();
                        if (g_camGain != newCamGain) {
                            changeLightProperty = true;
                            g_camGain = newCamGain;
                            g_commonDb.upsertConfig(PRO_DB_CAM_GAIN, to_string(g_camGain));
                        }
                        const auto newCamLight = camLight->text().trimmed().toInt();
                        if (g_camLight != newCamLight) {
                            changeLightProperty = true;
                            g_camLight = newCamLight;
                            g_commonDb.upsertConfig(PRO_DB_CAM_LIGHT, to_string(g_camLight));
                        }

                        // update
                        if (changeLightProperty && !g_camAutoLight) {
#ifndef WIN32
                            updateLight(g_camExpose, g_camGain, g_camLight);
#endif
                        }
                    }

                    const auto newFullFaceCompare = fullFaceCompare->isChecked() ? 1 : 0;
                    if (g_fullFaceCompare != newFullFaceCompare) {
                        g_fullFaceCompare = newFullFaceCompare;
                        g_commonDb.upsertConfig(
                            PRO_DB_FULL_FACE_COMPARE, to_string(g_fullFaceCompare));
                    }

                    const auto newFaceRegIvSec = faceRegIvSec->text().trimmed().toInt();
                    if (g_faceRegIvSec != newFaceRegIvSec) {
                        g_faceRegIvSec = newFaceRegIvSec;
                        g_commonDb.upsertConfig(PRO_DB_FACE_REG_IV_SEC, to_string(g_faceRegIvSec));
                    }

                    const auto newFaceRegCoreIvMillSec = faceRegCoreIvMillSec->text().trimmed().toInt();
                    if (g_faceRegCoreIvMillSec != newFaceRegCoreIvMillSec) {
                        g_faceRegCoreIvMillSec = newFaceRegCoreIvMillSec;
                        g_commonDb.upsertConfig(
                            PRO_DB_FACE_REG_CORE_IV_MILL_SEC, to_string(g_faceRegCoreIvMillSec));
                    }
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                }
            });
    rebootBtn = new QPushButton("重启", scrollContent);
    connect(rebootBtn, &QPushButton::clicked, this,
            [=] {
#ifdef WIN32
                logPrintln("Reboot ...", airstrip::INFO, __FUNCTION__);
#else
                airstrip::execScript(g_appWorkDir + "script/linux/reboot_app.sh");
#endif
            });
    checkUpdateBtn = new QPushButton("检查更新", scrollContent);
    connect(checkUpdateBtn, &QPushButton::clicked, this,
            [=] {
                static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue([] {
                    const auto updateNotification = appUpdate();
                    if (!updateNotification.isSuccessful) {
                        MainRouter::getInstance()->
                                mainNotificationShow("获取版本信息失败，请稍后再试",
                                                     bind(updateVersion, false));
                    } else if (!updateNotification.isNeedUpdate) {
                        MainRouter::getInstance()->
                                mainNotificationShow("当前版本已经是最新版本，无需升级",
                                                     bind(updateVersion, false));
                    } else {
                        g_prepareUpdateVersion = updateNotification.updateVersion;
                        g_prepareUpdateUrl = updateNotification.updateUrl;
                        MainRouter::getInstance()->
                                mainNotificationShow(
                                    "检测到最新版本：" + updateNotification.updateVersion + "，是否现在升级？",
                                    bind(updateVersion, std::placeholders::_1)
                                );
                    }
                });
            });
    cancelBtn = new QPushButton("取消", scrollContent);
    connect(cancelBtn, &QPushButton::clicked, this,
            [=] {
                MainRouter::getInstance()->backUntilPage(MAIN_PAGE_HOME);
            });


    scrollerAreaLayout->addWidget(serverAddressLabel);
    scrollerAreaLayout->addWidget(serverAddress);
    scrollerAreaLayout->addWidget(faceRegCoreIvMillSecLabel);
    scrollerAreaLayout->addWidget(faceRegCoreIvMillSec);
    scrollerAreaLayout->addWidget(faceThresholdLabel);
    scrollerAreaLayout->addWidget(faceThreshold);
    scrollerAreaLayout->addWidget(faceThresholdNightLabel);
    scrollerAreaLayout->addWidget(faceThresholdNight);
    scrollerAreaLayout->addWidget(fullFaceCompare);
    scrollerAreaLayout->addWidget(volLabel);
    scrollerAreaLayout->addWidget(vol);
    scrollerAreaLayout->addWidget(faceRegCountLabel);
    scrollerAreaLayout->addWidget(faceRegCount);
    scrollerAreaLayout->addWidget(taskIvSecLabel);
    scrollerAreaLayout->addWidget(taskIvSec);
    scrollerAreaLayout->addWidget(faceRegIvSecLabel);
    scrollerAreaLayout->addWidget(faceRegIvSec);
    scrollerAreaLayout->addWidget(camExposeLabel);
    scrollerAreaLayout->addWidget(camExpose);
    scrollerAreaLayout->addWidget(camGainLabel);
    scrollerAreaLayout->addWidget(camGain);
    scrollerAreaLayout->addWidget(camLightLabel);
    scrollerAreaLayout->addWidget(camLight);


    scrollerAreaLayout->addWidget(faceDistantLabel);
    scrollerAreaLayout->addWidget(faceDistantWidget);


    scrollerAreaLayout->addWidget(lightThresholdLabel);
    scrollerAreaLayout->addWidget(lightThresholdInput);
    scrollerAreaLayout->addWidget(darkThresholdLabel);
    scrollerAreaLayout->addWidget(darkThresholdInput);
    scrollerAreaLayout->addWidget(lightRatioLabel);
    scrollerAreaLayout->addWidget(lightRatioInput);
    scrollerAreaLayout->addWidget(darkRatioLabel);
    scrollerAreaLayout->addWidget(darkRatioInput);

    scrollerAreaLayout->addWidget(netModelLabel);
    scrollerAreaLayout->addWidget(netModelWidget);
    scrollerAreaLayout->addWidget(wifiAccountLabel);
    scrollerAreaLayout->addWidget(wifiAccount);
    scrollerAreaLayout->addWidget(wifiPasswdLabel);
    scrollerAreaLayout->addWidget(wifiPasswdEdit);

    scrollerAreaLayout->addWidget(enableFaceSpoof);
    scrollerAreaLayout->addWidget(lightOnlyCheck);
    scrollerAreaLayout->addWidget(showConfUser);
    scrollerAreaLayout->addWidget(camAutoLight);

    scrollerAreaLayout->addWidget(ipWiredLabel);
    scrollerAreaLayout->addWidget(ipWirelessLabel);
    scrollerAreaLayout->addWidget(ipFourGLabel);

    scrollerAreaLayout->addWidget(saveBtn);
    scrollerAreaLayout->addWidget(rebootBtn);
    scrollerAreaLayout->addWidget(checkUpdateBtn);
    scrollerAreaLayout->addWidget(cancelBtn);

    // Data
    serverAddress->setText(QString::fromStdString(g_serverAddress));
    faceThreshold->setText(QString::number(g_faceThreshold));
    faceThresholdNight->setText(QString::number(g_faceThresholdNight));
    vol->setText(QString::number(g_volNum));
    faceDistantGroup->button(g_faceDistance)->setChecked(true);
    enableFaceSpoof->setCheckState(g_enableFaceSpoof ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    lightOnlyCheck->setCheckState(g_lightOnlyCheck ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    showConfUser->setCheckState(g_showConfUser ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    camAutoLight->setCheckState(g_camAutoLight ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    fullFaceCompare->setCheckState(g_fullFaceCompare ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    netModelGroup->button(g_netModel)->setChecked(true);
    wifiAccount->setText(QString::fromStdString(g_wifiAccount));
    wifiPasswdEdit->setText(QString::fromStdString(g_wifiPasswd));
    lightThresholdInput->setText(QString::number(g_lightThreshold));
    darkThresholdInput->setText(QString::number(g_darkThreshold));
    lightRatioInput->setText(QString::number(g_lightRatio));
    darkRatioInput->setText(QString::number(g_darkRatio));
    faceRegCount->setText(QString::number(g_faceRegCount));
    taskIvSec->setText(QString::number(g_taskIvSec));
    faceRegIvSec->setText(QString::number(g_faceRegIvSec));
    camExpose->setText(QString::number(g_camExpose));
    camGain->setText(QString::number(g_camGain));
    camLight->setText(QString::number(g_camLight));
    faceRegCoreIvMillSec->setText(QString::number(g_faceRegCoreIvMillSec));


    // Connect
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWiredChange,
            this, [=](const string &ip) {
                const auto ipFull = "有线IP地址：" + ip;
                if (nullptr != ipWiredLabel) {
                    ipWiredLabel->setText(QString::fromStdString(ipFull));
                }
            });
    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerWirelessChange,
            this, [=](const string &ip) {
                const auto ipFull = "无线IP地址：" + ip;
                if (nullptr != ipWirelessLabel) {
                    ipWirelessLabel->setText(QString::fromStdString(ipFull));
                }
            });

    connect(GlobalDataManager::getInstance(), &GlobalDataManager::headerFourGChange,
            this, [=](const string &ip) {
                const auto ipFull = "4GIP地址：" + ip;
                if (nullptr != ipFourGLabel) {
                    ipFourGLabel->setText(QString::fromStdString(ipFull));
                }
            });
}


MainSettingTmp::~MainSettingTmp() = default;
