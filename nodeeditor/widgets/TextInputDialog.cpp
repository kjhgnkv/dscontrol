#include "widgets/TextInputDialog.h"
#include "ui_TextInputDialog.h"

TextInputDialog::TextInputDialog(QWidget* parent)
: QDialog(parent)
, ui(new Ui::TextInputDialog)
{
    ui->setupUi(this);

    ui->textEdit->setFocus();
}

TextInputDialog::~TextInputDialog()
{
    delete ui;
}

void TextInputDialog::setParams(const QString &title, const QString &text)
{
    setWindowTitle(title);
    ui->textEdit->setText(text);
    if (ui->textEdit->toPlainText() == title)
    {
        ui->textEdit->selectAll();
    }
}

QString TextInputDialog::getText(const QString &title, const QString &text)
{
    static TextInputDialog dlg;
    dlg.setParams(title, text);
    dlg.exec();
    if (dlg.result() == Accepted)
    {
        return dlg.curText();
    }
    return text;
}

QString TextInputDialog::curText()
{
    return ui->textEdit->toPlainText();
}

void TextInputDialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
        QDialog::changeEvent(event);
    }
}

void TextInputDialog::on_okButton_clicked()
{
    accept();
}

void TextInputDialog::on_cancelButton_clicked()
{
    reject();
}
