#include "UpdateWidget.hpp"

#include "Confirmation.hpp"
#include "models/AbstractItemModel.hpp"
#include "models/StoredItem.hpp"
#include "node/NodeItemUpdateComponent.hpp"
#include "node/NodeItemUpdateComputer.hpp"

#include <QDebug>
#include <QTreeView>

#include "ui_UpdateWidget.h"

UpdateWidget* UpdateWidget::updateWidget_ = nullptr;

UpdateWidget::UpdateWidget(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::UpdateWidget)
{
    ui_->setupUi(this);

    confirmation_ = new Confirmation;

    modelComputer_ = new AbstractItemModel(new NodeItemUpdateComputer());
    modelComputer_->appendTitle("Computer");
    modelComputer_->appendQtFlag(Qt::ItemIsUserCheckable);
    ui_->treeViewComputer->setModel(modelComputer_);

    prepareControlEvents();
}

UpdateWidget* UpdateWidget::init(QWidget* parent)
{
    if (updateWidget_ == nullptr)
    {
        updateWidget_ = new UpdateWidget {parent};
    }

    return updateWidget_;
}

UpdateWidget::~UpdateWidget()
{
    delete modelComputer_;
    delete ui_;
}

void UpdateWidget::updateData(const updateWidgetData &data, const std::vector<StoredItem*> &listComp)
{
    insertComputer(data, listComp);
}

void UpdateWidget::prepareControlEvents()
{
    connect(ui_->pushButtonClose, &QPushButton::clicked, [this]()
    {
        QDialog::reject();
    });

    connect(ui_->pushButtonUpdate, &QPushButton::clicked, [this]()
    {
        infoApps_.clear();
        NodeItemUpdateComputer* childRoot = dynamic_cast<NodeItemUpdateComputer*>(modelComputer_->root());
        for (const auto &device : childRoot->childs())
        {
            NodeItemUpdateComputer* childComp = dynamic_cast<NodeItemUpdateComputer*>(device);
            for (const auto &app : childComp->childs())
            {
                NodeItemUpdateComputer* childApp = dynamic_cast<NodeItemUpdateComputer*>(app);
                for (const auto &comp : childApp->childs())
                {
                    NodeItemUpdateComputer* childXML = dynamic_cast<NodeItemUpdateComputer*>(comp);
                    if (childXML->getCheckState() == Qt::CheckState::Checked)
                    {
                        NodeItemUpdateComponent* component = dynamic_cast<NodeItemUpdateComponent*>(childXML->ItemModelComponent()->child(
                        childXML->getSelectModelIndexXml()));
                        if (component->getCheckState() == Qt::CheckState::Checked)
                        {
                            addApps(childApp->getValue().value(0), childComp->getValue().value(0),
                                    component->getValue().value(0));
                        }
                    }
                }
            }
        }

        if (!infoApps_.empty())
        {
            getInfoApps();
            if (confirmation_->exec())
            {
                close();
            }
        }
    });

    connect(ui_->treeViewComputer, &QTreeView::clicked, [=](const QModelIndex &selectIndex)
    {
        if (selectIndex.isValid())
        {
            NodeItemUpdateComputer* itemComputer = dynamic_cast<NodeItemUpdateComputer*>(modelComputer_->child(
            selectIndex));
            if (itemComputer != nullptr)
            {
                ui_->treeViewComputer->setCurrentIndex(selectIndex);
                if (itemComputer->getLevel() == UpdateComputerLevel::Component)
                {
                    ui_->treeViewComponent->setModel(itemComputer->ItemModelComponent());
                    selectModelIndex = selectIndex;
                }
                else
                {
                    ui_->treeViewComponent->setModel(nullptr);
                }
            }
        }
    });

    connect(ui_->treeViewComponent, &QTreeView::clicked, [&](const QModelIndex &selectIndex)
    {
        if (selectIndex.isValid())
        {
            NodeItemUpdateComputer* itemComputer = dynamic_cast<NodeItemUpdateComputer*>(modelComputer_->child(
            selectModelIndex));
            NodeItemUpdateComponent* selectItem = dynamic_cast<NodeItemUpdateComponent*>(itemComputer->ItemModelComponent()->child(
            selectIndex));

            if (selectItem != nullptr)
            {
                if (selectItem->getLevel() == UpdateComponentLevel::Component)
                {
                    ui_->treeViewComponent->setCurrentIndex(selectIndex);

                    if (selectItem->getLevel() == UpdateComponentLevel::Component)
                    {
                        if (itemComputer->getSelectModelIndexXml() != selectIndex && selectItem->getCheckState() == Qt::CheckState::Checked)
                        {
                            itemComputer->ItemModelComponent()->update(itemComputer->getSelectModelIndexXml(),
                                                                       Qt::CheckState::Unchecked,
                                                                       Qt::ItemDataRole::CheckStateRole);
                            itemComputer->setSelectModelIndexXml(selectIndex);
                        }
                    }
                }
            }
        }
    });
}

void UpdateWidget::addApps(const QString &app, const QString &oldComp, const QString &newComp)
{
    auto it = infoApps_.find(app);
    if (it != infoApps_.end())
    {
        it->second.emplace_back(oldComp, newComp);
    }
    else
    {
        std::vector<std::pair<QString, QString>> vec;
        vec.emplace_back(oldComp, newComp);
        infoApps_.insert(std::pair<QString, std::vector<std::pair<QString, QString>>>(app, vec));
    }
}

void UpdateWidget::getInfoApps()
{
    for (const auto &app : infoApps_)
    {
        qDebug() << "\napp" << app;
        for (const auto &comp : app.second)
        {
            qDebug() << "\tcomp: old(" << comp.first << "), new(" << comp.second << ")";
        }
    }
}

bool UpdateWidget::insertComputer(const updateWidgetData &computers, const std::vector<StoredItem*> &listComp)
{
    for (const auto &itemComputer : computers)
    {
        AbstractNodeItem* childComp = new NodeItemUpdateComputer(itemComputer.first, UpdateComputerLevel::Computer);
        QModelIndex indexParent = modelComputer_->insert(childComp);

        for (const auto &app : itemComputer.second)
        {
            AbstractNodeItem* childApp = new NodeItemUpdateComputer(app.first, UpdateComputerLevel::Application);
            QModelIndex indexApp = modelComputer_->insert(childApp, indexParent);
            for (const auto &comp : app.second)
            {
                AbstractNodeItem* childXML = new NodeItemUpdateComputer(comp, UpdateComputerLevel::Component);
                childXML->appendItemDataRole(0, Qt::CheckStateRole);
                QModelIndex indexXML = modelComputer_->insert(childXML, indexApp);

                for (const auto &compGroup : listComp)
                {
                    insertComponent(*compGroup, indexXML);
                }
            }
        }
    }

    return true;
}

bool UpdateWidget::insertComponent(const StoredItem &xmls, const QModelIndex &indexXML)
{
    NodeItemUpdateComputer* rootXML = dynamic_cast<NodeItemUpdateComputer*>(modelComputer_->child(indexXML));

    AbstractNodeItem* itemClass = new NodeItemUpdateComponent(xmls.caption(), xmls.type(), UpdateComponentLevel::Class);
    QModelIndex indexRoot = rootXML->ItemModelComponent()->insert(itemClass);

    for (const auto &child : xmls.children())
    {
        AbstractNodeItem* itemComponent = new NodeItemUpdateComponent(child->caption(), child->type(),
                                                                      UpdateComponentLevel::Component);
        itemComponent->appendItemDataRole(0, Qt::CheckStateRole);
        rootXML->ItemModelComponent()->insert(itemComponent, indexRoot);
    }

    return true;
}
