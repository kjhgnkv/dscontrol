#pragma once

#include "ProjectModel.hpp"
#include <QUuid>
#include <QVariant>
#include <QVector>

namespace dscontrol
{
/*!
\brief class representing node item in ProjectTreeModel model
 */
class ProjectTreeItem
{
public:
    /*!
    \param data data represented in tree view (generally name of the node in
     nodes model)
    \param id Uuid of the node in node model
    \param type type of the node (i.e. System, Subsystem, Messages etc.)
    \param parentItem parent item in model
     */
    explicit ProjectTreeItem(const QVector<QVariant> &data, const QUuid &id, ProjectModel::MainProjectItems type, ProjectTreeItem *parentItem = nullptr);
    ~ProjectTreeItem();

    /*!
    \brief Adds new child to item
     */
    void appendChild(ProjectTreeItem *child);

    /*!
    \return Child item on target row, nullptr if has no row
     */
    ProjectTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;

    /*!
    \return parent item of this item
     */
    ProjectTreeItem *parentItem();

    /*!
    \return items type
     */
    ProjectModel::MainProjectItems type();

    /*!
    \return items Uuid
     */
    QUuid id() const;

    /*!
    \return Caption for mime data that is the string item type
     */
    QString mimeCaption();

    /*!
    \brief Set new caption
    \param new caption
     */
    void setCaption(QString caption);

private:
    ProjectModel::MainProjectItems type_;
    QUuid id_;
    QVector<ProjectTreeItem *> childItems_;
    QVector<QVariant> itemData_;
    ProjectTreeItem *parentItem_;
};
} // namespace dscontrol
