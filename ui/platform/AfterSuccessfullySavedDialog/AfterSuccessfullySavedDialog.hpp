#pragma once

#include <QDialog>

namespace Ui
{
class AfterSuccessfullySavedDialog;
}

namespace dscontrol
{
/*!
\brief Project success fully saved message class
 */
class AfterSuccessfullySavedDialog : public QDialog
{
Q_OBJECT

public:
    explicit AfterSuccessfullySavedDialog(QWidget* parent = nullptr);
    ~AfterSuccessfullySavedDialog();

private:
    Ui::AfterSuccessfullySavedDialog* ui;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
