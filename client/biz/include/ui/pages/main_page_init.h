#ifndef MAIN_PAGE_INIT_H
#define MAIN_PAGE_INIT_H

#include <QLabel>
#include <QVBoxLayout>

class MainPageInit final : public QWidget {
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

    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;
};


#endif //MAIN_PAGE_INIT_H
