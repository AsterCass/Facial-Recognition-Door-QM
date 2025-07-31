#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QThread>
#include <opencv2/core.hpp>
#ifdef Q_OS_WIN
#include <QCamera>
#include <QVideoWidget>
#else
#include <QLabel>
#include <QImage>
#endif
#include <QPainter>

class CameraFrame final : public QWidget {
public:
    CameraFrame(const CameraFrame &) = delete;

    CameraFrame &operator=(const CameraFrame &) = delete;

    static CameraFrame *getInstance() {
        static CameraFrame instance;
        return &instance;
    }

    void start() const;

    void stop() const;

    void updateFrameRK(const cv::Mat &frame);

    void positiveMessage(const std::string &extraStr = "") {
        if (nullptr == successLabel) {
            return;
        }

        // 投递到主线程
        if (QThread::currentThread() != this->thread()) {
            QMetaObject::invokeMethod(this, [this, extraStr] {
                positiveMessage(extraStr);
            }, Qt::QueuedConnection);
            return;
        }

        if (extraStr.empty()) {
            successLabel->setText("核验通过");
        } else {
            successLabel->setText(QString::fromStdString(extraStr));
        }
        failLabel->hide();
        successLabel->show();
    }

    void negativeMessage() {
        if (nullptr == failLabel) {
            return;
        }

        // 投递到主线程
        if (QThread::currentThread() != this->thread()) {
            QMetaObject::invokeMethod(this, [this] {
                negativeMessage();
            }, Qt::QueuedConnection);
            return;
        }

        successLabel->hide();
        failLabel->show();
    }

    void hideAllMessage() {
        if (nullptr == failLabel || nullptr == successLabel) {
            return;
        }

        // 投递到主线程
        if (QThread::currentThread() != this->thread()) {
            QMetaObject::invokeMethod(this, [this] {
                hideAllMessage();
            }, Qt::QueuedConnection);
            return;
        }

        successLabel->hide();
        failLabel->hide();
    }

    void setFaceRects(const double x, const double y, const double w, const double h) {
        // 投递到主线程
        if (QThread::currentThread() != this->thread()) {
            QMetaObject::invokeMethod(this, [this, x, y, w, h] {
                setFaceRects(x, y, w, h);
            }, Qt::QueuedConnection);
            return;
        }

        faceRect = QRect(x, y, w, h);
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QWidget::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::green, 2));
        painter.drawRect(faceRect);
    }

private:
    explicit CameraFrame(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    ~CameraFrame() override;

    QRect faceRect;

    QWidget *mask = nullptr;
    QVBoxLayout *mainLayout = nullptr;

    QVBoxLayout *maskLayout = nullptr;
    QWidget *bottomWidget = nullptr;
    QHBoxLayout *bottomLayout = nullptr;


    QLabel *successLabel = nullptr;
    QLabel *failLabel = nullptr;
    QLabel *snLabel = nullptr;
    QLabel *versionLabel = nullptr;

#ifdef Q_OS_WIN
    QCamera *camera = nullptr;
    QVideoWidget *videoWidget = nullptr;
#else
    QLabel *camera = nullptr;
    QImage* imageCache = nullptr;
#endif
};


#endif //CAMERA_FRAME_H
