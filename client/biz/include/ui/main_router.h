#ifndef MAIN_ROUTER_H
#define MAIN_ROUTER_H

#include <QStackedWidget>
#include "components/face_register.h"
#include "components/notification.h"
#include "enums/general_enums.h"


class MainRouter final : public QWidget {
public:
    MainRouter(const MainRouter &) = delete;

    MainRouter &operator=(const MainRouter &) = delete;

    static MainRouter *getInstance() {
        static MainRouter instance;
        return &instance;
    }

    void addPage(MainPage page);

    void removePageFromTop(MainPage page) const;

    void removePageAll(MainPage page) const;

    void backPage() const;

    void backUntilPage(MainPage page) const;

    void showFaceRegister(const cv::Mat &frame);

    void hideFaceRegister();

    void mainNotificationShow(const std::string &text, const std::function<void(bool)> &callback);

private:
    explicit MainRouter(QWidget *parent = nullptr);

    ~MainRouter() override;

    QStackedWidget *stackedWidget = nullptr;

    FaceRegister *faceRegister = nullptr;
    Notification *notification = nullptr;
};


#endif //MAIN_ROUTER_H
