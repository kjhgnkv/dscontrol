// interface_store_model.cpp

#pragma once

#include <QAbstractItemModel>

struct interface_data;

/**\brief node for interface_store_model*/
struct interface_node
{
    // type of nodes. Invalid - interface node which we can not inheritance,
    // because we have not description
    enum class node_type
    {
        interface,
        functions,
        func,
        invalid
    };

    interface_node(const ::QString &name_, node_type type_, interface_node* parent_ = nullptr)
    : parent {parent_}
    , name {name_}
    , type {type_}
    {
    }

    ~interface_node()
    {
        for (auto &i : children)
        {
            delete i;
        }
    }

    interface_node* parent;
    std::vector<interface_node*> children;
    /**\brief iterator to data in list*/
    std::list<interface_data>::const_iterator iterator;
    QString name;
    node_type type;
};

/**\brief tree model*/
class interface_store_model : public QAbstractItemModel
{
Q_OBJECT
public:
    /**\param interfaces reference to list of interfaces, which you can receive
     * from component_creator class*/
    interface_store_model(std::list<interface_data> &interfaces, QObject* parent = nullptr);
    ~interface_store_model();
    /**\return always two: first - name of interface or method, second -
     * package*/
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /**\brief find index of node in tree. This function use find method parent*/
    int find_row(const interface_node*) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    /**\warning you can insert only top level rows*/
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    /**\brief if interface is parent of another interface, than you can not
     * remove this, while its child exists. Also this method can not remove
     * items, which have QModelIndex parent
     * */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    /**\brief this function intended for insert data for empty node*/
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<interface_node*> roots_;
    std::list<interface_data> &interfaces_;
};
