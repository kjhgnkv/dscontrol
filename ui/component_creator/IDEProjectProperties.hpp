#pragma once

#include <QDialog>

namespace Ui
{
class IDEProjectProperties;
}

namespace dscontrol
{
/*!
\brief class represents dialog for getting parameters needed during IDE projects
generation
*/
class IDEProjectProperties : public QDialog
{
Q_OBJECT

public:
    explicit IDEProjectProperties(QWidget* parent = nullptr);
    ~IDEProjectProperties();

    enum IDEType
    {
        Unknown,
        QtCreator,
        VisualStudio
    };

    /*!
    \return type of IDE for which project should be generated
    */
    int IDEType();
    /*!
    \return name of new project
    */
    QString Name();
    /*!
    \return path of new project folder
    */
    QString Path();

private slots:
    void on_rbQtCreator_clicked();

    void on_btnPath_clicked();

    void on_btnOk_clicked();

    void on_pushButton_clicked();

private:
    Ui::IDEProjectProperties* ui_;
    int type_;
    QString name_;
    QString path_;
};
} // namespace dscontrol
