#pragma once

#include <QDialog>

namespace Ui
{
class IDEPath;
}

/*!
    \brief class represents dialog for getting IDE executable paths
    */
class IDEPath : public QDialog
{
Q_OBJECT

public:
    /*!
    \param vsPath initiate path for Visual Studio executable
    \param qtPath initiate path foe Qt Creator executable
    */
    explicit IDEPath(const QString &vsPath, const QString &qtPath, QWidget* parent = nullptr);
    ~IDEPath();

    /*!
    \return Qt Creator executable path
    */
    QString getQtPath();
    /*!
    \return Visual Studio executable path
    */
    QString getVSPath();

private slots:
    void on_btnQtPath_clicked();

    void on_btnOk_clicked();

    void on_btnVSPath_clicked();

    void on_btnCancel_clicked();

private:
    Ui::IDEPath* ui;

    QString vsPath_;
    QString qtPath_;
};
