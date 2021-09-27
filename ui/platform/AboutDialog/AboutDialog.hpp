#pragma once

#include <QDialog>

namespace Ui
{
class AboutDialog;
}

namespace dscontrol
{
/*!
\brief About VIDE dialog class
 */
class AboutDialog : public QDialog
{
Q_OBJECT

public:
    explicit AboutDialog(int type, QWidget* parent = nullptr);
    ~AboutDialog();

public:
    enum Type
    {
        VIDE,
        CETC
    };

protected:
    void changeEvent(QEvent* event);

private:
    void setVIDEText();
    void setCETCText();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AboutDialog* ui;
};
} // namespace dscontrol
