#ifndef MAIN_ROUTER_H
#define MAIN_ROUTER_H

#include <mutex>
#include <QStackedWidget>

#include "enums/general_enums.h"


class MainRouter final : public QWidget {
public:
    MainRouter(const MainRouter &) = delete;

    MainRouter &operator=(const MainRouter &) = delete;

    static MainRouter *getInstance() {
        static MainRouter instance;
        return &instance;
    }

    void addPage(MainPage page) const;

    void removePageFromTop(MainPage page) const;

    void removePageAll(MainPage page) const;

    void backPage() const;

    void backUntilPage(MainPage page) const;

private:
    explicit MainRouter(QWidget *parent = nullptr);

    ~MainRouter() override;

    QStackedWidget *stackedWidget = nullptr;
};


#endif //MAIN_ROUTER_H
