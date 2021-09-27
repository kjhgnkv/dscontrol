#pragma once

#include <QProgressBar>

class QLabel;

/*!
\brief Custom progress bar for project deploy progress dialog
*/
class DSDeployProgressBar : public QProgressBar
{
Q_OBJECT
public:
    enum class DeployState
    {
        None = -1,
        Progress = 0,
        Complete,
        Error
    };

    explicit DSDeployProgressBar(const QString &title, QWidget* parent);
    ~DSDeployProgressBar() override = default;

    void setProgressErrorFinish();
    void setStateLabel(QLabel* stateLbl);
    QString title() const;
    void setOldValue(const int &value);
    int oldValue() const;
    DeployState state() const;

signals:
    void stateChanged(const DeployState &state);

private:
    void initQSS(const DeployState &state);

private:
    DeployState state_;
    QLabel* stateLbl_;
    QString title_;
    int oldValue_;

    DSDeployProgressBar* parent_;
};
