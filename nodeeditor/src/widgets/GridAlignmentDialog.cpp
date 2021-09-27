#include "GridAlignmentDialog.h"
#include "ui_GridAlignmentDialog.h"

#include <QtMath>

GridAlignmentDialog::GridAlignmentDialog(qint32 totalCount)
: QDialog {nullptr}
, ui_ {new Ui::GridAlignmentDialog}
, totalCount_ {totalCount}
{
    ui_->setupUi(this);

    auto rowCount = qRound(std::sqrt(totalCount));
    auto columnCount = qRound(std::ceil(qreal(totalCount) / rowCount));

    ui_->rowCountSpin->setValue(rowCount);
    ui_->columnCountSpin->setValue(columnCount);

    connect(ui_->rowCountSpin, qOverload<int>(&QSpinBox::valueChanged), this, &GridAlignmentDialog::correctColumnCount);
    connect(ui_->columnCountSpin, qOverload<int>(&QSpinBox::valueChanged), this, &GridAlignmentDialog::correctRowCount);

    connect(ui_->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui_->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

GridAlignmentDialog::~GridAlignmentDialog()
{
    delete ui_;
}

qint32 GridAlignmentDialog::rowCount() const
{
    return ui_->rowCountSpin->value();
}

qint32 GridAlignmentDialog::columnCount() const
{
    return ui_->columnCountSpin->value();
}

void GridAlignmentDialog::correctRowCount(qint32 columnCount)
{
    qint32 rowCount = qRound(std::ceil(qreal(totalCount_) / columnCount));
    ui_->rowCountSpin->setValue(rowCount);
}

void GridAlignmentDialog::correctColumnCount(qint32 rowCount)
{
    qint32 columnCount = qRound(std::ceil(qreal(totalCount_) / rowCount));
    ui_->columnCountSpin->setValue(columnCount);
}
