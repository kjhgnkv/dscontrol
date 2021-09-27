#pragma once

#include <QAbstractItemModel>
#include <QDialog>

namespace Ui
{
class MessagesDialog;
}
namespace dscontrol
{
/*!
\brief All-messages dialog class
 */
class MessagesDialog : public QDialog
{
Q_OBJECT

public:
    explicit MessagesDialog(QAbstractItemModel* model, QWidget* parent = nullptr);
    ~MessagesDialog();

protected:
    void initQSS();

private:
    Ui::MessagesDialog* ui_;
    QAbstractItemModel* model_;

signals:
    void closed();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

    void keyPressEvent(QKeyEvent* event);

    // QWidget interface
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
