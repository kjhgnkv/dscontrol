#include "ProjectInfo.hpp"

#include "ui_ProjectInfo.h"
#include <QLabel>

namespace dscontrol
{
ProjectInfo::ProjectInfo(const std::vector<std::pair<QString, QString>> &data, QWidget* parent)
: QDialog(parent)
, ui_(new Ui::ProjectInfo)
{
    ui_->setupUi(this);
    setFixedSize(size());
    setInfo(data);
}

ProjectInfo::~ProjectInfo()
{
    delete ui_;
}

void ProjectInfo::setInfo(const std::vector<std::pair<QString, QString>> &data)
{
    // ui_->lblInfo->setText(info);
    // Clear content layout. it doesn`t works??
    int count = ui_->contentLay->rowCount();
    for (int i = 0; i < count; i++)
    {
        QFormLayout::TakeRowResult row = ui_->contentLay->takeRow(i);
        delete row.labelItem;
        delete row.fieldItem;
    }

    // Add records
    for (auto it : data)
    {
        const QString caption = it.first + ":";
        const QString content = it.second;

        QLabel* captionLabel = new QLabel {caption, this};
        QLabel* contentLabel = new QLabel {content, this};

        captionLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
        contentLabel->setStyleSheet("font-size: 14px; color: #ffffff;");

        captionLabel->setAlignment(Qt::AlignRight);
        contentLabel->setAlignment(Qt::AlignLeft);

        contentLabel->setWordWrap(true);

        ui_->contentLay->addRow(captionLabel, contentLabel);
    }
}

void ProjectInfo::on_btnOK_clicked()
{
    close();
}
} // namespace dscontrol
