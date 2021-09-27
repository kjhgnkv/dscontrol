#include "RepositorySettings.h"

#include "common/globals.h"
#include "ui_RepositorySettings.h"

#include <QFileDialog>

RepositorySettings::RepositorySettings(QWidget* parent)
: QGroupBox(parent)
, ui_(new Ui::RepositorySettings)
{
    ui_->setupUi(this);

    connect(ui_->localPathBtn, &QPushButton::clicked, this, &RepositorySettings::localButtonClicked);
}

RepositorySettings::~RepositorySettings()
{
    delete ui_;
}

void RepositorySettings::setValues(const QString &local, const QString &remote, const QString &filter)
{
    ui_->localPathEdit->setText(local);
    ui_->remotePathEdit->setText(remote);
    ui_->filterEdit->setText(filter);
}

void RepositorySettings::localButtonClicked()
{
    auto curPath = ui_->localPathEdit->text();
    QString path = QFileDialog::getOpenFileName(this, tr("Select local Database"), curPath, tr("Databases (*.db)"));
    if (!path.isEmpty())
    {
        ui_->localPathEdit->setText(path);
    }
}

QString RepositorySettings::localPath()
{
    return ui_->localPathEdit->text();
}

QString RepositorySettings::remotePath()
{
    return ui_->remotePathEdit->text();
}

QString RepositorySettings::filter()
{
    return ui_->filterEdit->text();
}

