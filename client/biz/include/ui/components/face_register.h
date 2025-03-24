#ifndef FACE_REGISTER_H
#define FACE_REGISTER_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "ui/components/common_components.h"

class FaceRegister final : public QWidget {
public:
    explicit FaceRegister(QWidget *parent = nullptr);

    ~FaceRegister() override;

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
};


#endif // FACE_REGISTER_H
