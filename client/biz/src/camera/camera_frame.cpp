#include "camera/camera_frame.h"

#include "airstrip_log.h"
#ifdef Q_OS_WIN
#include <QCameraInfo>
#include <QCameraViewfinder>
#else
#include <QImage>
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
    }
}

void CameraFrame::updateFrameRK(uchar *data, int height, int width) const {
#ifdef Q_OS_WIN
    logPrintln(to_string(camera->status()), airstrip::LogLevel::INFO, __FUNCTION__);
#else
    QImage image(data, width, height, QImage::Format_RGB888);
    camera->setPixmap(QPixmap::fromImage(image));
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
