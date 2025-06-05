#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <opencv2/core.hpp>
#ifdef Q_OS_WIN
#include <QCamera>
#include <QVideoWidget>
#else
#include <QLabel>
#include <QImage>
#endif


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

    void positiveMessage(const std::string &extraStr = "") const {
        if (nullptr == successLabel) {
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

    void negativeMessage() const {
        if (nullptr == failLabel) {
            return;
        }
        successLabel->hide();
        failLabel->show();
    }

    void hideAllMessage() const {
        if (nullptr == failLabel || nullptr == successLabel) {
            return;
        }
        successLabel->hide();
        failLabel->hide();
    }

private:
    explicit CameraFrame(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    ~CameraFrame() override;

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
