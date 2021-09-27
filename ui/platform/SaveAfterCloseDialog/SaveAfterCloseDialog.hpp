#pragma once

#include <QDialog>

namespace Ui
{
class SaveAfterCloseDialog;
}
/*!
\brief Save project dialog, appears when project is about to be closed
 */
class SaveAfterCloseDialog : public QDialog
{
Q_OBJECT
public:
    /*!
    \brief Possible results
    */
    enum class ExecResult
    {
        Save,
        SaveAs,
        DoNotSave,
        Cancel,
        Unknown
    };
    explicit SaveAfterCloseDialog(QWidget* parent = nullptr);
    ~SaveAfterCloseDialog();

private:
    Ui::SaveAfterCloseDialog* ui;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
