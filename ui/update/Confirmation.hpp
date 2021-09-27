#pragma once

#include <QDialog>

namespace Ui
{
class Confirmation;
}

class Confirmation : public QDialog
{
Q_OBJECT

public:
    explicit Confirmation(QWidget* parent = nullptr);
    ~Confirmation();

private:
    void prepareControlEvents();

signals:
    void getUpdate();

private:
    Ui::Confirmation* ui_;
};
