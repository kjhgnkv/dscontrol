#pragma once

#include <QDialog>

namespace Ui
{
class TextInputDialog;
}

class TextInputDialog : public QDialog
{
Q_OBJECT
private:
    Ui::TextInputDialog* ui;

public:
    explicit TextInputDialog(QWidget* parent = nullptr);
    ~TextInputDialog();

    static QString getText(const QString &title, const QString &text);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    void setParams(const QString &title, const QString &text);
    QString curText();

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
