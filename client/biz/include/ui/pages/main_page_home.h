#ifndef MAIN_PAGE_HOME_H
#define MAIN_PAGE_HOME_H

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class MainPageHome : public QWidget {
public:
    MainPageHome(const MainPageHome &) = delete;

    MainPageHome &operator=(const MainPageHome &) = delete;

    static MainPageHome *getInstance() {
        static MainPageHome instance;
        return &instance;
    }

private:
    explicit MainPageHome(QWidget *parent = nullptr);

    ~MainPageHome() override;


    std::unique_ptr<QPushButton> button;
    std::unique_ptr<QLabel> label;


    std::unique_ptr<QLineEdit> edit;
};


#endif //MAIN_PAGE_HOME_H
