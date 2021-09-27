#include "DSDeployProgressBar.hpp"

#include <QLabel>

DSDeployProgressBar::DSDeployProgressBar(const QString &title, QWidget* parent)
: QProgressBar(parent)
, title_ {title}
{
    setMinimum(0);
    setMaximum(100);
    setValue(100);
    oldValue_ = 100;
    state_ = DeployState::Complete;

    setMinimumWidth(744);
    setMaximumHeight(4);

    parent_ = dynamic_cast<DSDeployProgressBar*>(parent);

    initQSS(DeployState::Complete);

    connect(this, &DSDeployProgressBar::valueChanged, [this](const int &value)
    {
        if (value == maximum())
            this->initQSS(DeployState::Complete);
        else
            this->initQSS(DeployState::Progress);

        if (parent_)
        {
            parent_->setValue(parent_->value() + value - oldValue_);
        }
        oldValue_ = value;
    });
}

void DSDeployProgressBar::setProgressErrorFinish()
{
    initQSS(DeployState::Error);
}

void DSDeployProgressBar::setStateLabel(QLabel* stateLbl)
{
    stateLbl_ = stateLbl;
}

QString DSDeployProgressBar::title() const
{
    return title_;
}

void DSDeployProgressBar::setOldValue(const int &value)
{
    oldValue_ = value;
}

int DSDeployProgressBar::oldValue() const
{
    return oldValue_;
}

DSDeployProgressBar::DeployState DSDeployProgressBar::state() const
{
    return state_;
}

void DSDeployProgressBar::initQSS(const DSDeployProgressBar::DeployState &state)
{
    if (state_ != state)
    {
        state_ = state;

        switch (state_)
        {
            case DeployState::Progress:
            {
                setStyleSheet(QString(R"(
                               QProgressBar {
                                 background-color: #605F6E;
                                 border-radius :2px;
                               }
                               QProgressBar::chunk {
                               background-color: #F2C94C;
                               border-radius :2px;
                               }
                               )"));
                emit stateChanged(state_);
                break;
            }
            case DeployState::Complete:
            {
                setStyleSheet(QString(R"(
                               QProgressBar {
                                 background-color: #605F6E;
                                 border-radius :2px;
                               }
                               QProgressBar::chunk {
                               background-color: #27AE60;
                               border-radius :2px;
                               }
                               )"));

                this->stateLbl_->setPixmap({":/images/resources/images/Complete.png"});
                emit stateChanged(state_);
                break;
            }
            case DeployState::Error:
            {
                setStyleSheet(QString(R"(
                               QProgressBar {
                                 background-color: #605F6E;
                                 border-radius :2px;
                               }
                               QProgressBar::chunk {
                               background-color: #EB5757;
                               border-radius :2px;
                               }
                               )"));

                this->stateLbl_->setPixmap({":/images/resources/images/Failed.png"});
                if (parent_)
                {
                    parent_->setProgressErrorFinish();
                }
                emit stateChanged(state_);
                break;
            }
            case DeployState::None:
            {
                break;
            }
        }
    }
}
