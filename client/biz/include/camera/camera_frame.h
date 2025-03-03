#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H
#include <QVBoxLayout>
#ifdef Q_OS_WIN
#include <QCamera>
#include <QVideoWidget>
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

private:
    explicit CameraFrame(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    ~CameraFrame() override;

    QWidget *mask = nullptr;
    QVBoxLayout *mainLayout = nullptr;

    QCamera *camera = nullptr;
    QVideoWidget *videoWidget = nullptr;
};


#endif //CAMERA_FRAME_H
