#pragma once

#include <QGroupBox>

namespace Ui
{
class RepositorySettings;
}

class RepositorySettings : public QGroupBox
{
Q_OBJECT

public:
    explicit RepositorySettings(QWidget* parent = nullptr);
    ~RepositorySettings();

    void setValues(const QString &local, const QString &remote, const QString &filter);

    QString localPath();
    QString remotePath();
    QString filter();

private slots:
    void localButtonClicked();

private:
    Ui::RepositorySettings* ui_;
};

