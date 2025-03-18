#include "camera/camera_frame.h"

#include "airstrip_log.h"
#include "api/api.h"
#include "config/config.h"
#ifdef Q_OS_WIN
#include <QCameraInfo>
#include <QCameraViewfinder>
#else
#include "camera/camera_rk.h"
#endif

using namespace std;

CameraFrame::CameraFrame(QWidget *parent): QWidget(parent) {
    this->setStyleSheet("background: transparent");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Camera
    {
#ifdef Q_OS_WIN
        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        if (!cameras.isEmpty()) {
            camera = new QCamera(cameras.first(), this);

            videoWidget = new QVideoWidget(this);
            videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
            mainLayout->addWidget(videoWidget);

            camera->setViewfinder(videoWidget);
        }
#else
        camera = new QLabel(this);
        mainLayout->addWidget(camera);
#endif
    }

    // Mask
    {
        mask = new QWidget(this);
        mask->setGeometry(QRect(0, 0, this->width(), this->height()));
        mask->setObjectName("cameraFrameMask");
        mask->setStyleSheet("#cameraFrameMask{background-color:qlineargradient("
            "spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 0, 0,0 "
            "),stop:0.85 rgba(0, 0, 0,0 ) ,stop:1 #000000)}");
        mainLayout = new QVBoxLayout(mask);

        // bottom and info
        {
            successLabel = new QLabel("核验通过", mask); {
                successLabel->
                        setStyleSheet("border-radius: 32px; font-size: 32px;  background-color: rgb(0, "
                            "180, 42);color: white;");
                successLabel->setFixedSize(240, 80);
                successLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                successLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
                successLabel->hide();
            }

            failLabel = new QLabel("识别失败", mask); {
                failLabel->
                        setStyleSheet("border-radius: 32px; font-size: 32px;  background-color: rgb(245, "
                            "63, 63);color: white;");
                failLabel->setFixedSize(240, 80);
                failLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                failLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
                failLabel->hide();
            }


            bottomWidget = new QWidget(mask); {
                bottomLayout = new QHBoxLayout(bottomWidget);
                bottomLayout->setSpacing(0);
                bottomLayout->setMargin(0);
                // Version
                {
                    versionLabel = new QLabel(QString("Version: %1").arg(APP_VERSION), bottomWidget);
                    versionLabel->setStyleSheet(
                        "background-color: transparent; color: white; font-size: 16px;");
                }
                // SN
                {
                    snLabel = new QLabel(QString("SN: %1").arg(getSn().c_str()), bottomWidget);
                    snLabel->setStyleSheet("background-color: transparent; color: white; font-size: 16px;");
                }
                bottomLayout->addWidget(versionLabel);
                bottomLayout->addStretch();
                bottomLayout->addWidget(snLabel);
            }
        }

        mainLayout->addStretch();
        mainLayout->addStretch();
        mainLayout->addWidget(successLabel, 0, Qt::AlignCenter);
        mainLayout->addWidget(failLabel, 0, Qt::AlignCenter);
        mainLayout->addStretch();
        mainLayout->addWidget(bottomWidget);
    }
}

void CameraFrame::updateFrameRK(const cv::Mat &frame) {
#ifdef Q_OS_WIN
    logPrintln(to_string(camera->status()), airstrip::LogLevel::INFO, __FUNCTION__);
#else
    if (!imageCache) {
        imageCache = new QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888);
    }
    memcpy(imageCache->bits(), frame.data, frame.cols * frame.rows);

    QMetaObject::invokeMethod(camera, "setPixmap", Qt::QueuedConnection,
                              Q_ARG(QPixmap, QPixmap::fromImage(*imageCache)));
#endif
}


void CameraFrame::start() const {
#ifdef Q_OS_WIN
    QMetaObject::invokeMethod(camera, "start", Qt::QueuedConnection);
#else
    startCameraRk();
#endif
}

void CameraFrame::stop() const {
#ifdef Q_OS_WIN
    QMetaObject::invokeMethod(camera, "stop", Qt::QueuedConnection);
#else
    stopCameraRk();
#endif
}

void CameraFrame::resizeEvent(QResizeEvent *) {
    if (mask) {
        mask->setGeometry(QRect(0, 0, this->width(), this->height()));
    }
}


CameraFrame::~CameraFrame() = default;
