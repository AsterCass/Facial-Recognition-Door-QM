#include "ui/pages/main_page_setting_system.h"

#include <airstrip_log.h>
#include <airstrip_thread_pool.h>
#include <sstream>
#include <api/api.h>
#include <config/style.h>
#include <ui/main_router.h>


using namespace std;


MainSettingSystem::MainSettingSystem(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // save
    save = new QPushButton("保存");
    connect(save, &QPushButton::clicked, this,
            [=] {
                try {
                    {
                        if (g_voice != voiceValue) {
                            g_voice = voiceValue;
                            g_commonDb.upsertConfig(
                                PRO_DB_VOICE, to_string(g_voice));
                        }
                        if (nullptr != voiceVolInput) {
                            const auto newData = std::stoi(voiceVolInput->text().trimmed().toStdString());
                            if (g_volNum != newData) {
                                g_volNum = newData;
                                g_commonDb.upsertConfig(
                                    PRO_DB_VOL_NUM, to_string(g_volNum));
                            }
                        }
                        if (nullptr != faceRegCoreIvMillSecTimeInput) {
                            const auto newData = std::stoi(
                                faceRegCoreIvMillSecTimeInput->text().trimmed().toStdString());
                            if (g_faceRegCoreIvMillSec != newData) {
                                g_faceRegCoreIvMillSec = newData;
                                g_commonDb.upsertConfig(
                                    PRO_DB_FACE_REG_CORE_IV_MILL_SEC, to_string(g_faceRegCoreIvMillSec));
                            }
                        }
                        if (nullptr != taskIvSecTimeInput) {
                            const auto newData = std::stoi(taskIvSecTimeInput->text().trimmed().toStdString());
                            if (g_taskIvSec != newData) {
                                g_taskIvSec = newData;
                                g_commonDb.upsertConfig(
                                    PRO_DB_TASK_IV_SEC, to_string(g_taskIvSec));
                            }
                        }

                        if (g_lightOnlyCheck != lightOnlyCheckValue) {
                            g_lightOnlyCheck = lightOnlyCheckValue;
                            g_commonDb.upsertConfig(
                                PRO_DB_ENABLE_LIGHT_ONLY_CHECK, to_string(g_lightOnlyCheck));
                        }
                        if (g_needBackup != needBackupValue) {
                            g_needBackup = needBackupValue;
                            g_commonDb.upsertConfig(
                                PRO_DB_NEED_BACKUP, to_string(g_needBackup));
                        }

                        MainRouter::getInstance()->mainNotificationShow("保存成功", nullptr);
                    }
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save system config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                    MainRouter::getInstance()->mainNotificationShow(
                        "保存失败，请确认填写内容有效性", nullptr);
                }
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "系统参数设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(20);
    //body
    {
        // voice
        voice = new QWidget(body);
        voiceLayout = new QHBoxLayout(voice);
        voiceLayout->setContentsMargins(20, 0, 0, 0);
        voiceLayout->setSpacing(20);
#ifdef WIN32
        voice->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        voice->setFixedHeight(50);
#else
        voice->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        voice->setFixedHeight(100);
#endif
        voiceLabel = new QLabel("语音提示", voice);
        voiceInput = new QPushButton("⬤", voice);
        voiceInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
        connect(voiceInput, &QPushButton::clicked, this,
                [=] {
                    voiceValue = voiceValue ? 0 : 1;
                    voiceInput->setStyleSheet(voiceValue
                                                  ? SWITCH_BUTTON_ENABLE_STYLE
                                                  : SWITCH_BUTTON_DISABLE_STYLE);
                });

        voiceLayout->addWidget(voiceLabel);
        voiceLayout->addStretch();
        voiceLayout->addWidget(voiceInput);
        bodyLayout->addWidget(voice);

        // voiceVol
        voiceVol = new QWidget(body);
        voiceVolLayout = new QHBoxLayout(voiceVol);
        voiceVolLayout->setContentsMargins(20, 0, 0, 0);
        voiceVolLayout->setSpacing(20);
#ifdef WIN32
        voiceVol->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        voiceVol->setFixedHeight(50);
#else
        voiceVol->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        voiceVol->setFixedHeight(100);
#endif
        voiceVolLabel = new QLabel("音量", voiceVol);
        voiceVolInput = new QLineEditPro(voiceVol);
        voiceVolInput->setAlignment(Qt::AlignRight);
        voiceVolInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        voiceVolInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
        voiceVolLayout->addWidget(voiceVolLabel);
        voiceVolLayout->addWidget(voiceVolInput);
        bodyLayout->addWidget(voiceVol);


        // faceRegCoreIvMillSecTime
        faceRegCoreIvMillSecTimeTips = new QLabel("机器识别超频（0-1000，推荐500）（0为极致超频，高温下可能会过热关机）", body);
        faceRegCoreIvMillSecTimeTips->setWordWrap(true);
#ifdef WIN32
        faceRegCoreIvMillSecTimeTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin-right: 8px");
#else
            faceRegCoreIvMillSecTimeTips->setStyleSheet("font-size: 16px; color: rgb(150, 150, 150); margin-right: 16px");
#endif

        faceRegCoreIvMillSecTime = new QWidget(body);
        faceRegCoreIvMillSecTimeLayout = new QHBoxLayout(faceRegCoreIvMillSecTime);
        faceRegCoreIvMillSecTimeLayout->setContentsMargins(20, 0, 0, 0);
        faceRegCoreIvMillSecTimeLayout->setSpacing(20);
#ifdef WIN32
        faceRegCoreIvMillSecTime->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        faceRegCoreIvMillSecTime->setFixedHeight(50);
#else
            faceRegCoreIvMillSecTime->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            faceRegCoreIvMillSecTime->setFixedHeight(100);
#endif
        faceRegCoreIvMillSecTimeLabel = new QLabel("超频指数", faceRegCoreIvMillSecTime);
        faceRegCoreIvMillSecTimeInput = new QLineEditPro(faceRegCoreIvMillSecTime);
        faceRegCoreIvMillSecTimeInput->setAlignment(Qt::AlignRight);
        faceRegCoreIvMillSecTimeInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        faceRegCoreIvMillSecTimeInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
        faceRegCoreIvMillSecTimeLayout->addWidget(faceRegCoreIvMillSecTimeLabel);
        faceRegCoreIvMillSecTimeLayout->addWidget(faceRegCoreIvMillSecTimeInput);
        bodyLayout->addWidget(faceRegCoreIvMillSecTime);
        bodyLayout->addWidget(faceRegCoreIvMillSecTimeTips);


        // taskIvSecTime
        taskIvSecTimeTips = new QLabel("机器从服务器获取任务时钟，生效值最低为5", body);
        taskIvSecTimeTips->setWordWrap(true);
#ifdef WIN32
        taskIvSecTimeTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin-right: 8px");
#else
            taskIvSecTimeTips->setStyleSheet("font-size: 16px; color: rgb(150, 150, 150); margin-right: 16px");
#endif
        taskIvSecTime = new QWidget(body);
        taskIvSecTimeLayout = new QHBoxLayout(taskIvSecTime);
        taskIvSecTimeLayout->setContentsMargins(20, 0, 0, 0);
        taskIvSecTimeLayout->setSpacing(20);
#ifdef WIN32
        taskIvSecTime->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        taskIvSecTime->setFixedHeight(50);
#else
            taskIvSecTime->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            taskIvSecTime->setFixedHeight(100);
#endif
        taskIvSecTimeLabel = new QLabel("任务间隔（s）", taskIvSecTime);
        taskIvSecTimeInput = new QLineEditPro(taskIvSecTime);
        taskIvSecTimeInput->setAlignment(Qt::AlignRight);
        taskIvSecTimeInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        taskIvSecTimeInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");
        taskIvSecTimeLayout->addWidget(taskIvSecTimeLabel);
        taskIvSecTimeLayout->addWidget(taskIvSecTimeInput);
        bodyLayout->addWidget(taskIvSecTime);
        bodyLayout->addWidget(taskIvSecTimeTips);

        // lightOnlyCheck
        lightOnlyCheck = new QWidget(body);
        lightOnlyCheckLayout = new QHBoxLayout(lightOnlyCheck);
        lightOnlyCheckLayout->setContentsMargins(20, 0, 0, 0);
        lightOnlyCheckLayout->setSpacing(20);
#ifdef WIN32
        lightOnlyCheck->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        lightOnlyCheck->setFixedHeight(50);
#else
            lightOnlyCheck->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            lightOnlyCheck->setFixedHeight(100);
#endif
        lightOnlyCheckLabel = new QLabel("夜间仅在核验时开补光灯", lightOnlyCheck);
        lightOnlyCheckInput = new QPushButton("⬤", lightOnlyCheck);
        lightOnlyCheckInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
        connect(lightOnlyCheckInput, &QPushButton::clicked, this,
                [=] {
                    lightOnlyCheckValue = lightOnlyCheckValue ? 0 : 1;
                    lightOnlyCheckInput->setStyleSheet(lightOnlyCheckValue
                                                           ? SWITCH_BUTTON_ENABLE_STYLE
                                                           : SWITCH_BUTTON_DISABLE_STYLE);
                });

        lightOnlyCheckLayout->addWidget(lightOnlyCheckLabel);
        lightOnlyCheckLayout->addStretch();
        lightOnlyCheckLayout->addWidget(lightOnlyCheckInput);
        bodyLayout->addWidget(lightOnlyCheck);


        // needBackup
        needBackupTips = new QLabel("消耗较多流量，4G模式慎用", body);
        needBackupTips->setWordWrap(true);
#ifdef WIN32
        needBackupTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin-right: 8px");
#else
            needBackupTips->setStyleSheet("font-size: 16px; color: rgb(150, 150, 150); margin-right: 16px");
#endif
        needBackup = new QWidget(body);
        needBackupLayout = new QHBoxLayout(needBackup);
        needBackupLayout->setContentsMargins(20, 0, 0, 0);
        needBackupLayout->setSpacing(20);
#ifdef WIN32
        needBackup->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        needBackup->setFixedHeight(50);
#else
            needBackup->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
            needBackup->setFixedHeight(100);
#endif
        needBackupLabel = new QLabel("数据备份", needBackup);
        needBackupInput = new QPushButton("⬤", needBackup);
        needBackupInput->setStyleSheet(SWITCH_BUTTON_DISABLE_STYLE);
        connect(needBackupInput, &QPushButton::clicked, this,
                [=] {
                    needBackupValue = needBackupValue ? 0 : 1;
                    needBackupInput->setStyleSheet(needBackupValue
                                                       ? SWITCH_BUTTON_ENABLE_STYLE
                                                       : SWITCH_BUTTON_DISABLE_STYLE);
                });

        needBackupLayout->addWidget(needBackupLabel);
        needBackupLayout->addStretch();
        needBackupLayout->addWidget(needBackupInput);
        bodyLayout->addWidget(needBackup);
        bodyLayout->addWidget(needBackupTips);
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingSystem::~MainSettingSystem() = default;

void MainSettingSystem::showEvent(QShowEvent *) {
    // data
    if (voiceInput) {
        voiceInput->setStyleSheet(g_voice ? SWITCH_BUTTON_ENABLE_STYLE : SWITCH_BUTTON_DISABLE_STYLE);
        voiceValue = g_voice;
    }
    if (voiceVolInput) {
        voiceVolInput->setText(QString::number(g_volNum));
    }
    if (faceRegCoreIvMillSecTimeInput) {
        faceRegCoreIvMillSecTimeInput->setText(QString::number(g_faceRegCoreIvMillSec));
    }
    if (taskIvSecTimeInput) {
        taskIvSecTimeInput->setText(QString::number(g_taskIvSec));
    }
    if (lightOnlyCheckInput) {
        lightOnlyCheckInput->setStyleSheet(g_lightOnlyCheck ? SWITCH_BUTTON_ENABLE_STYLE : SWITCH_BUTTON_DISABLE_STYLE);
        lightOnlyCheckValue = g_lightOnlyCheck;
    }
    if (needBackupInput) {
        needBackupInput->setStyleSheet(g_needBackup ? SWITCH_BUTTON_ENABLE_STYLE : SWITCH_BUTTON_DISABLE_STYLE);
        needBackupValue = g_needBackup;
    }
}
