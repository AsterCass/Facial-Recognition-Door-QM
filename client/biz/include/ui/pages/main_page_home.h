#ifndef MAIN_PAGE_HOME_H
#define MAIN_PAGE_HOME_H

#include <QLabel>
#include <QVBoxLayout>

class MainPageHome final : public QWidget {
public:
    MainPageHome(const MainPageHome &) = delete;

    MainPageHome &operator=(const MainPageHome &) = delete;

    static MainPageHome *getInstance(QWidget *parent) {
        static MainPageHome instance(parent);
        return &instance;
    }

private:
    explicit MainPageHome(QWidget *parent = nullptr);

    void showEvent(QShowEvent *event) override;

    void setElement();

    ~MainPageHome() override;


    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;
};


#endif //MAIN_PAGE_HOME_H
