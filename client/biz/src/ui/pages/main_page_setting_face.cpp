#include "ui/pages/main_page_setting_face.h"

#include <airstrip_log.h>
#include <sstream>
#include <ui/main_router.h>


using namespace std;

MainSettingFace::MainSettingFace(QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: rgb(4, 9, 12)");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // save
    save = new QPushButton("保存");
    connect(save, &QPushButton::clicked, this,
            [=] {
                try {
                    if (nullptr != faceThresholdInput) {
                        const auto newFaceThreshold = faceThresholdInput->text().trimmed().toDouble();
                        if (g_faceThreshold != newFaceThreshold) {
                            g_faceThreshold = newFaceThreshold;
                            g_commonDb.upsertConfig(PRO_DB_FACE_THRESHOLD, to_string(g_faceThreshold));
                        }
                    }
                    if (nullptr != faceThresholdNightInput) {
                        const auto newFaceThresholdNight = faceThresholdNightInput->text().trimmed().toDouble();
                        if (g_faceThresholdNight != newFaceThresholdNight) {
                            g_faceThresholdNight = newFaceThresholdNight;
                            g_commonDb.upsertConfig(
                                PRO_DB_FACE_THRESHOLD_NIG, to_string(g_faceThresholdNight));
                        }
                    }

                    MainRouter::getInstance()->mainNotificationShow("保存成功", nullptr);
                } catch (const std::exception &e) {
                    ostringstream errMsg;
                    errMsg << "Save face config data error :" << e.what();
                    logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                    MainRouter::getInstance()->mainNotificationShow(
                        "保存失败，请确认填写内容有效性", nullptr);
                }
            });

    mainHeader = new MainComponentHeader(this);
    settingHeader = new SettingHeader(this, "人脸设置", save);


    body = new QWidget(this);
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setMargin(20);
    bodyLayout->setSpacing(0);
    //body
    {
        // faceThreshold
        faceThresholdTips = new QLabel("人脸识别阈值（0 - 0.6）（推荐 0.48）（重启生效）", body);
        faceThresholdTips->setWordWrap(true);
#ifdef WIN32
        faceThresholdTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin: 8px");
#else
        faceThresholdTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin: 8px");
#endif

        faceThreshold = new QWidget(body);
        faceThresholdLayout = new QHBoxLayout(faceThreshold);
        faceThresholdLayout->setContentsMargins(20, 0, 0, 0);
        faceThresholdLayout->setSpacing(20);
#ifdef WIN32
        faceThreshold->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        faceThreshold->setFixedHeight(50);
#else
        faceThreshold->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        faceThreshold->setFixedHeight(100);
#endif
        faceThresholdLabel = new QLabel("人脸识别阈值", faceThreshold);
        faceThresholdInput = new QLineEditPro(faceThreshold);
        faceThresholdInput->setAlignment(Qt::AlignRight);
        faceThresholdInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        faceThresholdInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");

        faceThresholdLayout->addWidget(faceThresholdLabel);
        faceThresholdLayout->addWidget(faceThresholdInput);
        bodyLayout->addWidget(faceThreshold);
        bodyLayout->addWidget(faceThresholdTips);


        // faceThresholdNight
        faceThresholdNightTips = new QLabel("夜间人脸识别阈值（0 - 0.6）（推荐 0.42）（重启生效）", body);
        faceThresholdNightTips->setWordWrap(true);
#ifdef WIN32
        faceThresholdNightTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin: 8px");
#else
        faceThresholdNightTips->setStyleSheet("font-size: 8px; color: rgb(150, 150, 150); margin: 8px");
#endif

        faceThresholdNight = new QWidget(body);
        faceThresholdNightLayout = new QHBoxLayout(faceThresholdNight);
        faceThresholdNightLayout->setContentsMargins(20, 0, 0, 0);
        faceThresholdNightLayout->setSpacing(20);
#ifdef WIN32
        faceThresholdNight->setStyleSheet(
            "background-color: rgb(31, 31, 31);  border-radius: 8px; font-size: 14px; color: white");
        faceThresholdNight->setFixedHeight(50);
#else
        faceThresholdNight->setStyleSheet("background-color: rgb(31, 31, 31);  border-radius: 16px; font-size: 28px; color: white");
        faceThresholdNight->setFixedHeight(100);
#endif
        faceThresholdNightLabel = new QLabel("夜间人脸识别阈值", faceThresholdNight);
        faceThresholdNightInput = new QLineEditPro(faceThresholdNight);
        faceThresholdNightInput->setAlignment(Qt::AlignRight);
        faceThresholdNightInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        faceThresholdNightInput->setStyleSheet(R"(
                QLineEdit {
                        padding-right: 20px;
                }
        )");

        faceThresholdNightLayout->addWidget(faceThresholdNightLabel);
        faceThresholdNightLayout->addWidget(faceThresholdNightInput);
        bodyLayout->addWidget(faceThresholdNight);
        bodyLayout->addWidget(faceThresholdNightTips);


        //else
    }
    bodyLayout->addStretch();


    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(mainHeader);
    mainLayout->addWidget(settingHeader);
    mainLayout->addWidget(body);
}


MainSettingFace::~MainSettingFace() = default;

void MainSettingFace::showEvent(QShowEvent *) {
    // data
    if (faceThresholdInput) {
        faceThresholdInput->setText(QString::number(g_faceThreshold));
    }
    if (faceThresholdNightInput) {
        faceThresholdNightInput->setText(QString::number(g_faceThreshold));
    }
}
