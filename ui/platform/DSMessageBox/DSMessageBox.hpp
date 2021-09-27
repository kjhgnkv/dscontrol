#pragma once

#include <QDialog>

namespace Ui
{
class DSMessageBox;
}

namespace dscontrol
{
class DSMessageBox : public QDialog
{
Q_OBJECT

public:
    enum StandardButtons
    {
        NoButton = 0x0,
        Ok = 0x01,
        Cancel = 0x02,
        Yes = 0x04,
        No = 0x08,
        Abort = 0x10,
        Retry = 0x20,
        Ignore = 0x40,
        YesAll = 0x80,
        NoAll = 0x100,
        File = 0x200,
        Clipboard = 0x400,
        Pdf = 0x800,
        Odf = 0x1000
    };

    enum Icon
    {
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };

    explicit DSMessageBox(DSMessageBox::Icon icon, const QString &title, const QString &text, int buttons = NoButton,
                          QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ~DSMessageBox();

    static void about(QWidget* parent, const QString &title, const QString &text);
    static int critical(QWidget* parent, const QString &title, const QString &text,
                        DSMessageBox::StandardButtons buttons = Ok);
    static int information(QWidget* parent, const QString &title, const QString &text,
                           DSMessageBox::StandardButtons buttons = Ok);
    static int question(QWidget* parent, const QString &title, const QString &text,
                        DSMessageBox::StandardButtons buttons = StandardButtons(Yes | No));
    static int warning(QWidget* parent, const QString &title, const QString &text,
                       DSMessageBox::StandardButtons buttons = Ok);

    void setText(const QString &);

    void setIcon(Icon);
    void setIcon(const QPixmap &);

    void setup();

private:
    void addButtons(int buttons);
    void addButton(QPushButton* button, int type);

private slots:
    void keyPressed(int type);

private:
    Ui::DSMessageBox* ui_;
};
} // namespace dscontrol
