#include "DeployProgressBarDialog.hpp"

#include "DSMessageBox/DSMessageBox.hpp"
#include "ui/platform/custom/DSDeployProgressBar.hpp"
#include "ui_DeployProgressBarDialog.h"

#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QSet>
#include <QSpacerItem>
#include <QVBoxLayout>

DeployProgressBarDialog::DeployProgressBarDialog(QWidget* parent)
: QDialog(parent)
, ui(new Ui::DeployProgressBarDialog)
, computersCount_ {0}
, applicationsCount_ {0}
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->frame->setLayout(new QGridLayout(this));
    reinterpret_cast<QGridLayout*>(ui->frame->layout())->addLayout(createMainProgressLayout(tr("Progress deployment")),
                                                                   0, 0);
    ui->frameComputers_->setLayout(new QVBoxLayout(this));

    connect(ui->btnRight_, &QPushButton::clicked, this, &DeployProgressBarDialog::onCancelBtnClicked);

    setState(DSDeployProgressBar::DeployState::Progress);

    connect(mainBar_, &DSDeployProgressBar::stateChanged, this, &DeployProgressBarDialog::setState);
}

DeployProgressBarDialog::~DeployProgressBarDialog()
{
    delete ui;
}

void DeployProgressBarDialog::setShowingData(const std::set<dscontrol::ProgressDataObject> &data)
{
    for (const auto &[computer, apps, ipAddress] : data)
    {
        std::list<ApplicationBarData> allData;
        for (const auto &[app, components] : apps)
        {
            ApplicationBarData barData;
            barData.applicationName_ = app;
            for (const auto &component : components)
            {
                barData.componentsPathes_.push_back(component.filePath);
                barData.size_ += component.size_;
            }
            barData.ipAddress_ = ipAddress;
            allData.push_back(barData);
        }

        addNewComputer(computer, allData);
    }
}

void DeployProgressBarDialog::updateShowingData(const QString &filePath, const QString &ipAddress, const double &value)
{
    if (state_ == DSDeployProgressBar::DeployState::Error)
    {
        return;
    }

    std::map<ApplicationBarData, DSDeployProgressBar*> founded;
    for (auto &[data, bar] : applicationsBars_)
    {
        if (data.componentsPathes_.contains(filePath) && data.ipAddress_ == ipAddress)
        {
            bar->setValue(bar->value() + (int)value);
        }
    }
}

void DeployProgressBarDialog::uploadTitleBarProcess(Poco::UInt32 msgId)
{
    qDebug() << "DeployProgressBarDialog::uploadProcessTitleBar";
    qDebug() << "msgId: " << msgId;
}

void DeployProgressBarDialog::setProgressError(const QString &filePath, const QString &ip, const QString &error)
{
    if (mainBar_)
    {
        std::map<ApplicationBarData, DSDeployProgressBar*> founded;
        mainBar_->setProgressErrorFinish();
        for (auto &[data, bar] : applicationsBars_)
        {
            if (data.componentsPathes_.contains(filePath) && data.ipAddress_ == ip)
            {
                bar->setProgressErrorFinish();
                dscontrol::DSMessageBox::critical(this, tr("Error"), error);
            }
        }
    }
}

void DeployProgressBarDialog::addNewComputer(const QString &computerName, const std::list<ApplicationBarData> &data)
{
    auto layout = createComputerProgressLayout(computerName, data, BarType::Computer);
    reinterpret_cast<QVBoxLayout*>(ui->frameComputers_->layout())->addLayout(layout);
    if (computersCount_ + applicationsCount_ < 16 && computersCount_ > 1)
    {
        this->resize(this->width(), this->height() + layout->sizeHint().height());
    }
}

QLayout* DeployProgressBarDialog::createMainProgressLayout(const QString &title)
{
    return createComputerProgressLayout(title, {});
}

QLayout* DeployProgressBarDialog::createComputerProgressLayout(const QString &computerName,
                                                               const std::list<ApplicationBarData> &data,
                                                               const BarType &type)
{
    unsigned int computerSize = 0;

    QVBoxLayout* mainComputerLayout = new QVBoxLayout(this);

    QVBoxLayout* computerLayout = new QVBoxLayout(this);

    QHBoxLayout* barLayout = new QHBoxLayout(this);
    QLabel* computerLbl = new QLabel(computerName, this);
    computerLbl->setMinimumHeight(20);
    QLabel* stateLbl = new QLabel(this);
    stateLbl->setMinimumSize(16, 16);
    DSDeployProgressBar* bar;

    if (type == BarType::Main)
    {
        bar = new DSDeployProgressBar(computerName, this);
        mainBar_ = bar;
        computerLbl->setStyleSheet(R"(QLabel
                                {
                                font-size: 18px;
                                line-height: 23px;
                                

                                color: #FFFFFF;
                                })");
    }
    else
    {
        bar = new DSDeployProgressBar(computerName, mainBar_);
        computerLbl->setStyleSheet(R"(QLabel
                                {
                                font-size: 18px;
                                line-height: 23px;
                                

                                color: #FFFFFF;
                                })");
    }
    computerLbl->setAlignment(Qt::AlignCenter);

    bar->setStateLabel(stateLbl);

    bar->setMinimumWidth(864);
    QSpacerItem* leftBarSpacer = new QSpacerItem(72, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSpacerItem* middleNextBarSpacer = new QSpacerItem(12, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSpacerItem* rightBarSpacer = new QSpacerItem(44, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);

    barLayout->addItem(leftBarSpacer);
    barLayout->addWidget(bar);
    barLayout->addItem(middleNextBarSpacer);
    barLayout->addWidget(stateLbl);
    barLayout->addItem(rightBarSpacer);

    computerLayout->addWidget(computerLbl);
    computerLayout->addLayout(barLayout);
    computerLayout->setSpacing(10);

    mainComputerLayout->addLayout(computerLayout);
    mainComputerLayout->setSpacing(8);

    computersCount_++;

    for (const auto &application : data)
    {
        QHBoxLayout* applicationLayout = new QHBoxLayout(this);

        QSpacerItem* leftBarSpacer = new QSpacerItem(72, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);
        QSpacerItem* middleBarSpacer = new QSpacerItem(30, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);
        QSpacerItem* middleNextBarSpacer = new QSpacerItem(12, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);
        QSpacerItem* rightBarSpacer = new QSpacerItem(44, 20, QSizePolicy::Preferred, QSizePolicy::Preferred);

        QLabel* applicationLbl = new QLabel(application.applicationName_, this);
        applicationLbl->setMinimumSize(80, 20);
        QLabel* stateLbl = new QLabel(this);
        stateLbl->setMinimumSize(16, 16);
        applicationLbl->setStyleSheet(R"(
                                font-weight: normal;
                                font-size: 14px;
                                line-height: 18px;
                                color: #FFFFFF;
                                )");
        DSDeployProgressBar* applicationBar = new DSDeployProgressBar(application.applicationName_, bar);
        applicationBar->setStateLabel(stateLbl);
        applicationBar->setMaximum(application.size_);

        computerSize += application.size_;

        applicationLayout->addItem(leftBarSpacer);
        applicationLayout->addWidget(applicationLbl);
        applicationLayout->addItem(middleBarSpacer);
        applicationLayout->addWidget(applicationBar);
        applicationLayout->addItem(middleNextBarSpacer);
        applicationLayout->addWidget(stateLbl);
        applicationLayout->addItem(rightBarSpacer);

        mainComputerLayout->addLayout(applicationLayout);

        applicationsCount_++;

        applicationsBars_.insert({application, applicationBar});
    }

    bar->setMaximum(computerSize);
    mainBar_->setMaximum(mainBar_->maximum() + computerSize);

    qDebug() << mainComputerLayout->sizeHint();

    return mainComputerLayout;
}

void DeployProgressBarDialog::setState(const DSDeployProgressBar::DeployState &state)
{
    // QMessageBox::information(this, "setState", "setState");
    // TODO change state in main bar checking
    // if (state_ != state && mainBar_->state() == state)
    {
        state_ = state;
        switch (state_)
        {
            case DSDeployProgressBar::DeployState::Progress:
            {
                ui->btnLeft_->hide();
                ui->btnRight_->show();
                ui->btnRight_->setStyleSheet(R"(
                                         QPushButton
                                         {
                                         font-size: 18px;
                                         line-height: 23px;
                                         
                                         
                                         

                                         color: #FFFFFF;

                                         background: #EB5757;
                                         border: 1px solid #3B3E3D;
                                         box-sizing: border-box;
                                         border-radius: 10px;
                                         }
                                         QPushButton::hover
                                         {
                                         background: #F87E7E;
                                         border: 1px solid #3B3E3D;
                                         box-sizing: border-box;
                                         border-radius: 10px;
                                         }
                                         QPushButton::pressed
                                         {
                                         background: #EB5757;
                                         border: 1px solid #3B3E3D;
                                         box-sizing: border-box;
                                         border-radius: 10px;
                                         }
                                         )");
                connect(ui->btnRight_, &QPushButton::clicked, this, &DeployProgressBarDialog::onCancelBtnClicked);
                break;
            }
            case DSDeployProgressBar::DeployState::Complete:
            {
                ui->btnRight_->hide();
                ui->btnLeft_->show();
                ui->btnLeft_->setStyleSheet(R"(
                                         QPushButton
                                         {
                                         font-size: 18px;
                                         line-height: 23px;
                                         
                                         
                                         

                                         color: #FFFFFF;

                                         background: #27AE60;
                                         border: 1px solid #3B3E3D;
                                         box-sizing: border-box;
                                         border-radius: 10px;
                                         }
                                         QPushButton::hover
                                         {
                                        background: #46CE7F;
                                        border: 1px solid #3B3E3D;
                                        box-sizing: border-box;
                                        border-radius: 10px;
                                         }
                                         )");
                ui->btnLeft_->setText(tr("Ok"));
                disconnect(ui->btnLeft_, 0, this, 0);
                connect(ui->btnLeft_, &QPushButton::clicked, this, &DeployProgressBarDialog::onOkBtnClicked);
                break;
            }
            case DSDeployProgressBar::DeployState::Error:
            {
                // ui->btnRight_->show();
                ui->btnLeft_->show();
                ui->btnLeft_->setStyleSheet(R"(
                                         QPushButton
                                         {
                                         font-size: 18px;
                                         line-height: 23px;
                                         
                                         
                                         

                                         color: #FFFFFF;

                                        background: #605F6E;
                                        border: 1px solid #3B3E3D;
                                        box-sizing: border-box;
                                        border-radius: 10px;
                                         }
                                         QPushButton::hover
                                         {
                                        background: #605F6E;
                                        border: 1px solid #7B59FF;
                                        box-sizing: border-box;
                                        border-radius: 10px;
                                         }
                                         )");
                ui->btnLeft_->setText(tr("Repeat"));
                disconnect(ui->btnLeft_, 0, this, 0);
                connect(ui->btnLeft_, &QPushButton::clicked, this, &DeployProgressBarDialog::onRepeatBtnClicked);
                break;
            }
            case DSDeployProgressBar::DeployState::None:
            {
                break;
            }
        }
    }
}
