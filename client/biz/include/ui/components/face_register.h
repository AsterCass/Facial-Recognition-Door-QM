#ifndef FACE_REGISTER_H
#define FACE_REGISTER_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <opencv2/opencv.hpp>

#include "ui/components/common_components.h"

class FaceRegister final : public QWidget {
public:
    explicit FaceRegister(QWidget *parent = nullptr);

    ~FaceRegister() override;

    void setLastFrame(const cv::Mat &frame) {
        lastFrame = frame.clone();
    }

    void enableRegisterBtn(const bool isEnable) const {
        if (registerBtn) {
            if (isEnable) {
                registerBtn->setStyleSheet("background-color: rgb(13, 133, 255);");
                registerBtn->setDisabled(false);
            } else {
                registerBtn->setDisabled(true);
                registerBtn->setStyleSheet("background-color: rgba(13, 133, 255, 0.5);");
            }
        }
    }

    void loadingApi(const bool isStart) const {
        if (errorTips && loadGif) {
            if (isStart) {
                errorTips->setMovie(loadGif);
                loadGif->start();
            } else {
                loadGif->stop();
                errorTips->setText(" ");
            }
        }
    }

    void resetTips(const bool isPositive, const std::string &data) const {
        if (errorTips) {
            if (isPositive) {
#ifdef WIN32
                errorTips->setStyleSheet("font-size: 12px; color: green");
#else
                errorTips->setStyleSheet("font-size: 24px; color: green");
#endif
            } else {
#ifdef WIN32
                errorTips->setStyleSheet("font-size: 12px; color: red");
#else
                errorTips->setStyleSheet("font-size: 24px; color: red");
#endif
            }
            errorTips->setText(QString::fromStdString(data));
        }
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

    QWidget* errorTipsWidget = nullptr;
    QVBoxLayout* errorTipsLayout = nullptr;
    QLabel *errorTips = nullptr;
    QMovie *loadGif = nullptr;


    cv::Mat lastFrame = {};
};


#endif // FACE_REGISTER_H
