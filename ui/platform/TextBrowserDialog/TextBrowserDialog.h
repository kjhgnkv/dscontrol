#pragma once

#include <QDialog>

namespace Ui
{
class TextBrowserDialog;
}

class TextBrowserDialog : public QDialog
{
public:
    TextBrowserDialog(const QString &text);
    ~TextBrowserDialog();

private:
    Ui::TextBrowserDialog* ui_;
};
