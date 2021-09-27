#pragma once

#include <QDialog>

namespace Ui
{
class ProjectInfo;
}

namespace dscontrol
{
/*!
\brief Dialog shows project information
 */
class ProjectInfo : public QDialog
{
Q_OBJECT

public:
    /*!
    \param data project info data that is presented by pairs: property name
     and value
     */
    explicit ProjectInfo(const std::vector<std::pair<QString, QString>> &data, QWidget* parent = nullptr);
    ~ProjectInfo();

    /*!
    \brief setup project info data that is presented by pairs: property name
     and value
     */
    void setInfo(const std::vector<std::pair<QString, QString>> &data);

private slots:
    void on_btnOK_clicked();

private:
    Ui::ProjectInfo* ui_;
};
} // namespace dscontrol
