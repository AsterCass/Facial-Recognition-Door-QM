#ifndef MAIN_PAGE_INIT_H
#define MAIN_PAGE_INIT_H

#include <QLabel>
#include <QVBoxLayout>

class MainPageInit final : public QWidget {
public:
    MainPageInit(const MainPageInit &) = delete;

    MainPageInit &operator=(const MainPageInit &) = delete;

    static MainPageInit *getInstance(QWidget *parent) {
        static MainPageInit instance(parent);
        return &instance;
    }

private:
    explicit MainPageInit(QWidget *parent = nullptr);

    void showEvent(QShowEvent *event) override;

    void setElement();

    ~MainPageInit() override;

    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;
};


#endif //MAIN_PAGE_INIT_H
