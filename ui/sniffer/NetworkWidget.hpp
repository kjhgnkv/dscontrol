#pragma once

#include <QDialog>

class AbstractItemModel;
class NodeItemSniffer;

namespace Ui
{
class NetworkWidget;
}

class NetworkWidget : public QDialog
{
Q_OBJECT
protected:
    explicit NetworkWidget(QWidget* parent = nullptr);

public:
    ~NetworkWidget() override;
    static NetworkWidget* init(QWidget* parent = nullptr);

    void updateData(const QString &array);

private:
    Ui::NetworkWidget* ui_;
    static NetworkWidget* networkWidget_;

    AbstractItemModel* model_;
};
