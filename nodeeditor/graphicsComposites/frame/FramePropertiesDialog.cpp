#include "FramePropertiesDialog.h"
#include "ui_FramePropertiesDialog.h"

FramePropertiesDialog::FramePropertiesDialog(const QString &title, int verSwimlaneCount, int horSwimlaneCount,
                                             QWidget* parent)
: QDialog(parent)
, ui_ {new Ui::FramePropertiesDialog}
{
    ui_->setupUi(this);

    ui_->titleEdit->setText(title);
    ui_->verSwimlaneCountSpin->setValue(verSwimlaneCount);
    ui_->horSwimlaneCountSpin->setValue(horSwimlaneCount);

    connect(ui_->okBtn, &QPushButton::clicked, this, &FramePropertiesDialog::accept);
    connect(ui_->cancelBtn, &QPushButton::clicked, this, &FramePropertiesDialog::reject);
}

QString FramePropertiesDialog::getTitle() const
{
    return ui_->titleEdit->text();
}

int FramePropertiesDialog::getVerticalSwimlaneCount() const
{
    return ui_->verSwimlaneCountSpin->value();
}

int FramePropertiesDialog::getHorizontalSwimlaneCount() const
{
    return ui_->horSwimlaneCountSpin->value();
}
