#pragma once

#include <QAbstractTableModel>
#include <QDialog>

namespace Ui
{
class CommunicationsDialog;
}

namespace dscontrol
{
class CommunicationModel;

/*!
\brief Communications dialog class
 */
class CommunicationsDialog : public QDialog
{
Q_OBJECT

public:
    /*!
    \param model model for dialog table view
     */
    explicit CommunicationsDialog(CommunicationModel* model, QWidget* parent = nullptr);
    ~CommunicationsDialog();

protected:
    void initQSS();

signals:
    void closed();

private slots:
    void editError(const QString &error);

private:
    Ui::CommunicationsDialog* ui_;
    QAbstractItemModel* model_;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

    void keyPressEvent(QKeyEvent* event);

    // QWidget interface
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
