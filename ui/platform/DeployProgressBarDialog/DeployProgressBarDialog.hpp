#pragma once

#include "manager/OutXmlGenerator.hpp"
#include "ui/platform/custom/DSDeployProgressBar.hpp"

#include <QDialog>
#include <set>
#include <Poco/Types.h>

namespace Ui
{
class DeployProgressBarDialog;
}

struct ApplicationBarData
{
    QString applicationName_ = "";
    unsigned int size_ = 0;
    QStringList componentsPathes_ {};
    QString ipAddress_ = "";

    inline friend bool operator<(const ApplicationBarData &lhs, const ApplicationBarData &rhs)
    {
        return lhs.applicationName_ < rhs.applicationName_ || lhs.size_ < rhs.size_ || lhs.componentsPathes_ < rhs.componentsPathes_ || lhs.ipAddress_ < rhs.ipAddress_;
    }
};

/*!
\brief Class of deploy progress dialog
*/
class DeployProgressBarDialog : public QDialog
{
Q_OBJECT

    enum class BarType
    {
        Main,
        Computer,
        Application
    };

public:
    explicit DeployProgressBarDialog(QWidget* parent = nullptr);
    ~DeployProgressBarDialog();

    void setShowingData(const std::set<dscontrol::ProgressDataObject>& data);
    void updateShowingData(const QString &filePath, const QString &ipAddress, const double &value);
    // void setProgressError(const QString &filePath, const QString &ip, const QString &error);

private:
    void addNewComputer(const QString &computerName, const std::list<ApplicationBarData> &data);
    QLayout* createMainProgressLayout(const QString &title);
    QLayout* createComputerProgressLayout(const QString &computerName, const std::list<ApplicationBarData> &data,
                                          const BarType &type = BarType::Main);

signals:
    void onOkBtnClicked();
    void onCancelBtnClicked();
    void onRepeatBtnClicked();

public slots:
    void setState(const DSDeployProgressBar::DeployState &state);
    void uploadTitleBarProcess(Poco::UInt32 msgId);
    void setProgressError(const QString &filePath, const QString &ip, const QString &error);

private:
    Ui::DeployProgressBarDialog* ui;

    std::map<ApplicationBarData, DSDeployProgressBar*> applicationsBars_;
    DSDeployProgressBar* mainBar_;

    int computersCount_;
    int applicationsCount_;

    DSDeployProgressBar::DeployState state_;
};
