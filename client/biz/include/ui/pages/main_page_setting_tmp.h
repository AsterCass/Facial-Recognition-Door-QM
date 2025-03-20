#ifndef MAIN_PAGE_SETTING_TMP_H
#define MAIN_PAGE_SETTING_TMP_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class MainSettingTmp final : public QWidget {
public:
    explicit MainSettingTmp(QWidget *parent = nullptr);

    ~MainSettingTmp() override;

private:
    QVBoxLayout *mainLayout = nullptr;

    QPushButton *cancelBtn = nullptr;
};


#endif //MAIN_PAGE_SETTING_TMP_H
