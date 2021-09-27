#pragma once

#include <QDialog>
#include <memory>
#include <set>

//#include "DSServerHost.hpp"
#include "DestData.hpp"

namespace Ui
{
class DeployWidget;
}

namespace dscontrol
{
class CommComboBoxDelegate;

namespace models
{
class DeployTargetModel;
class DeployDestModel;
}
}

class DeployWidget : public QDialog
{
Q_OBJECT
private:
    explicit DeployWidget(QWidget* parent = nullptr);

public:
    ~DeployWidget() override;
    static DeployWidget* init();

    void setupData(const std::vector<std::tuple<int, QString, QString>> &targetData,
                   const std::vector<DestData> &listDSServer);

signals:
    void deploySelectedBtn(bool isAutomatic, const std::map<QString, QString> &applications);

private slots:
    void on_deployBtn_clicked();

private:
    static DeployWidget* deployWidget_;

    Ui::DeployWidget* ui_;

    dscontrol::models::DeployTargetModel* targetModel_;
    dscontrol::models::DeployDestModel* destModel_;
    dscontrol::CommComboBoxDelegate* delegate_;
};
