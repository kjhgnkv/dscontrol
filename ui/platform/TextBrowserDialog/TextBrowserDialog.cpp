#include "TextBrowserDialog.h"

#include "ui_TextBrowserDialog.h"
#include "UiHighlighter.h"

TextBrowserDialog::TextBrowserDialog(const QString &text)
: QDialog {nullptr}
, ui_ {new Ui::TextBrowserDialog}
{
    ui_->setupUi(this);

    ui_->textBrowser->setText(text);

    connect(ui_->closeButton, &QPushButton::clicked, this, &QDialog::accept);

    new UiHighlighter(ui_->textBrowser->document());
}

TextBrowserDialog::~TextBrowserDialog()
{
    delete ui_;
}
