#ifndef MAIN_PAGE_HOME_H
#define MAIN_PAGE_HOME_H

#include <QLabel>
#include <QVBoxLayout>

class MainPageHome final : public QWidget {
public:
    explicit MainPageHome(QWidget *parent = nullptr);

    ~MainPageHome() override;

private:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;
};


#endif //MAIN_PAGE_HOME_H
