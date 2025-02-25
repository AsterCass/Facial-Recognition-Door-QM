#ifndef COMMON_COMPONENTS_H
#define COMMON_COMPONENTS_H

#include <QLineEdit>

class QLineEditPro final : public QLineEdit {
public:
    explicit QLineEditPro(QWidget *parent = nullptr) : QLineEdit(parent) {
    }

protected:
    void mousePressEvent(QMouseEvent *) override;
};


#endif // COMMON_COMPONENTS_H
