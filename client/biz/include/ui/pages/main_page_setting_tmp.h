#ifndef MAIN_PAGE_SETTING_TMP_H
#define MAIN_PAGE_SETTING_TMP_H

#include <QVBoxLayout>
#include <QWidget>


class MainSettingTmp final : public QWidget {
public:
    MainSettingTmp(const MainSettingTmp &) = delete;

    MainSettingTmp &operator=(const MainSettingTmp &) = delete;

    static MainSettingTmp *getInstance(QWidget *parent) {
        static MainSettingTmp instance(parent);
        return &instance;
    }

private:
    explicit MainSettingTmp(QWidget *parent = nullptr);

    void showEvent(QShowEvent *event) override;

    void setElement();

    ~MainSettingTmp() override;

    QVBoxLayout *mainLayout = nullptr;
};


#endif //MAIN_PAGE_SETTING_TMP_H
