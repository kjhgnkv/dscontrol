#pragma once

#include <QDialog>
#include <QModelIndex>
#include <memory>

class StoredItem;

namespace Ui
{
class UpdateWidget;
}

class AbstractItemModel;
class NodeItemUpdateComputer;
class Confirmation;

typedef std::vector<std::pair<QString, std::vector<std::pair<QString, std::vector<QString>>>>> updateWidgetData;

typedef std::map<QString, std::vector<std::pair<QString, QString>>> updateWidgetApps;

class UpdateWidget : public QDialog
{
Q_OBJECT

protected:
    explicit UpdateWidget(QWidget* parent = nullptr);

public:
    ~UpdateWidget();
    static UpdateWidget* init(QWidget* parent = nullptr);
    void updateData(const updateWidgetData &data, const std::vector<StoredItem*> &listComp);
    void getInfoApps();

private:
    void prepareControlEvents();
    void addApps(const QString &app, const QString &oldComp, const QString &newComp);
    bool insertComputer(const updateWidgetData &computers, const std::vector<StoredItem*> &listComp);
    bool insertComponent(const StoredItem &xmls, const QModelIndex &indexXML);

private:
    static UpdateWidget* updateWidget_;
    Ui::UpdateWidget* ui_;
    AbstractItemModel* modelComputer_;
    QModelIndex selectModelIndex;
    updateWidgetApps infoApps_;
    Confirmation* confirmation_;
};
