#pragma once

#include <QDialog>

namespace Ui
{
class NewProjectDialog;
}

/*!
\brief New project dialog class
 */
namespace dscontrol
{
class NewProjectDialog : public QDialog
{
Q_OBJECT

public:
    explicit NewProjectDialog(QWidget* parent = 0);
    ~NewProjectDialog();

    /*!
    \return project name
     */
    QString getProjectName();

    /*!
    \return project path
     */
    QString getProjectPath();

private slots:
    void createProject();

    void setPath();

private:
    Ui::NewProjectDialog* ui_;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
