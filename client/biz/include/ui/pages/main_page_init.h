#ifndef MAIN_PAGE_INIT_H
#define MAIN_PAGE_INIT_H

#include <QLabel>
#include <QVBoxLayout>

class MainPageInit final : public QWidget {
public:
    explicit MainPageInit(QWidget *parent = nullptr);

    ~MainPageInit() override;

private:
    QVBoxLayout *mainLayout = nullptr;
    QWidget *header = nullptr;
    QWidget *body = nullptr;
};


#endif //MAIN_PAGE_INIT_H
