#pragma once

#include <QDialog>

namespace Ui
{
class PropertiesDialog;
}

namespace dscontrol
{
/*!
\brief Dialog showing properties of an item
 */
class PropertiesDialog : public QDialog
{
public:
    /*!
    \param data Standard mime store model Json component data
     */
    PropertiesDialog(const QJsonObject &data, QWidget* parent = nullptr);

private:
    void setData(const std::vector<std::pair<QString, QString>> &data);
    std::vector<std::pair<QString, QString>> parseJson(const QJsonObject &data);

private:
    Ui::PropertiesDialog* ui;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
