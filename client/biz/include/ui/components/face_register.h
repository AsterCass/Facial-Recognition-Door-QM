#ifndef FACE_REGISTER_H
#define FACE_REGISTER_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <opencv2/opencv.hpp>

#include "ui/components/common_components.h"

class FaceRegister final : public QWidget {
public:
    explicit FaceRegister(QWidget *parent = nullptr);

    ~FaceRegister() override;

    void setLastFrame(const cv::Mat &frame) {
        lastFrame = frame;
    }

private:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;
    QWidget *faceRegisterWidget = nullptr;

    QVBoxLayout *faceRegisterLayout = nullptr;
    QLabel *faceRegisterTitle = nullptr;

    QWidget *phoneNumberWidget = nullptr;
    QHBoxLayout *phoneNumberLayout = nullptr;
    QLineEditProNumber *phoneNumberFirst = nullptr;
    QLineEditProNumber *phoneNumberSecond = nullptr;
    QLineEditProNumber *phoneNumberThird = nullptr;

    QWidget *btnWidget = nullptr;
    QHBoxLayout *btnLayout = nullptr;
    QPushButton *cancelBtn = nullptr;
    QPushButton *registerBtn = nullptr;

    QLabel *errorTips = nullptr;


    cv::Mat lastFrame = {};
};


#endif // FACE_REGISTER_H
