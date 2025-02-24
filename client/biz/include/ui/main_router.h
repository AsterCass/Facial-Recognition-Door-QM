#ifndef MAIN_ROUTER_H
#define MAIN_ROUTER_H

#include <QStackedWidget>
#include <QVBoxLayout>


class MainRouter : public QWidget {
public:
    MainRouter(const MainRouter &) = delete;

    MainRouter &operator=(const MainRouter &) = delete;

    static MainRouter *getInstance() {
        static MainRouter instance;
        return &instance;
    }

private:
    explicit MainRouter(QWidget *parent = nullptr);

    ~MainRouter() override;
};


#endif //MAIN_ROUTER_H
