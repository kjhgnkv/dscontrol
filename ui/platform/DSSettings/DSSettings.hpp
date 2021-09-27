#pragma once

#include <QDialog>

namespace Ui
{
class DSSettings;
}

class QItemSelection;

namespace dscontrol
{
class DSSettings : public QDialog
{
Q_OBJECT

    enum class OptionTargetObject
    {
        CodeExtraction,
        Item,
        Arrow,
        Repository,
        Scene
    };

public:
    explicit DSSettings(QWidget* parent = nullptr);
    ~DSSettings() override;

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_btnApply_clicked();
    void on_btnVSPath_clicked();
    void on_btnQtPath_clicked();

private:
    void saveAll();
    void loadAll();
    qint32 optionTargetObject(OptionTargetObject row) const;
    OptionTargetObject optionTargetObject(qint32 row) const;

private:
    Ui::DSSettings* ui_;
};
}
