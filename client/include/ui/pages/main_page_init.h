#ifndef MAIN_PAGE_INIT_H
#define MAIN_PAGE_INIT_H

#include <QPushButton>
#include <QLabel>

class MainPageInit : public QWidget {
public:
    MainPageInit(const MainPageInit &) = delete;

    MainPageInit &operator=(const MainPageInit &) = delete;

    static MainPageInit *getInstance() {
        static MainPageInit instance;
        return &instance;
    }

private:
    explicit MainPageInit(QWidget *parent = nullptr);

    ~MainPageInit() override;


    std::unique_ptr<QPushButton> button;
    std::unique_ptr<QLabel> label;
};


#endif //MAIN_PAGE_INIT_H
