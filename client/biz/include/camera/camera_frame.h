#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H
#include <QWidget>
#include <QVBoxLayout>
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

    void updateFrameRK(uchar *data, int height, int width);

private:
    explicit CameraFrame(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    ~CameraFrame() override;

    QWidget *mask = nullptr;
    QVBoxLayout *mainLayout = nullptr;

#ifdef Q_OS_WIN
    QCamera *camera = nullptr;
    QVideoWidget *videoWidget = nullptr;
#else
    QLabel *camera = nullptr;
    QImage* imageCache = nullptr;
#endif
};


#endif //CAMERA_FRAME_H
