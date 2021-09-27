// interface_store_model.cpp

#include "interface_store_model.hpp"
#include "interface_data.hpp"

/**\brief this function recursively search interface with input name in model
 * \return true if find, false - otherwise
 */
bool recurs_search_inheritance(const QString &input_interfase_name, const interface_node* node);

/**\create tree for model*/
interface_node* create_interface_node(const std::list<interface_data>::const_iterator &data,
                                      const std::list<interface_data> &interfaces);

/**\brief this function needed for get all pure virtual methods from
 * inheritances classes*/
QStringList get_all_pure_virtual_methods(const interface_node* node);

interface_store_model::interface_store_model(std::list<interface_data> &interfaces, QObject* parent)
: ::QAbstractItemModel {parent}
, interfaces_ {interfaces}
{
    for (auto i = interfaces_.begin(); i != interfaces_.end(); ++i)
    {
        auto interface_node = create_interface_node(i, interfaces_);
        roots_.push_back(interface_node);
    }
}

interface_store_model::~interface_store_model()
{
    for (auto i : roots_)
    {
        delete i;
    }
}

int interface_store_model::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant interface_store_model::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (index.column())
        {
            // if column one - return name of interface or method
            case 0:
                switch (role)
                {
                    case Qt::DisplayRole:
                        return static_cast<interface_node*>(index.internalPointer())->name;
                        break;
                        // when we set edit role, then we return information about interface
                    case Qt::EditRole:
                    {
                        auto item = static_cast<interface_node*>(index.internalPointer());
                        if (item->type == interface_node::node_type::interface && item->iterator != interfaces_.end())
                        {
                            // here we merge selected interface with parent classes, for
                            // get all abstract methods
                            interface_data selected_interfase = *item->iterator;
                            selected_interfase.abstract_methods = get_all_pure_virtual_methods(item);

                            return QVariant::fromValue(selected_interfase);
                        }
                        return QVariant();
                    }
                    case Qt::TextAlignmentRole:
                    {
                        return Qt::AlignCenter;
                    }
                    default:
                        return QVariant {};
                }
                break;
                // if column two return package name
            case 1:
            {
                if (role == Qt::DisplayRole && static_cast<interface_node*>(index.internalPointer())->type == interface_node::node_type::interface)
                {
                    return static_cast<interface_node*>(index.internalPointer())->iterator->packages;
                }

                if (role == Qt::TextAlignmentRole)
                {
                    return Qt::AlignCenter;
                }

                return QVariant {};
            }
                break;
            default:
                break;
        }
    }
    return QVariant {};
}

QModelIndex interface_store_model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    if (!parent.isValid())
    {
        return createIndex(row, column, static_cast<interface_node*>(roots_[row]));
    }

    interface_node* parent_node = static_cast<interface_node*>(parent.internalPointer());
    return createIndex(row, column, parent_node->children[row]);
}

QModelIndex interface_store_model::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return ::QModelIndex();
    }

    interface_node* parent_node = static_cast<interface_node*>(index.internalPointer())->parent;
    if (parent_node == nullptr)
    {
        return ::QModelIndex();
    }
    else
    {
        return createIndex(find_row(parent_node), 0, parent_node);
    }
}

int interface_store_model::find_row(const interface_node* node) const
{
    const std::vector<interface_node*> &siblings = (node->parent == nullptr) ? roots_ : node->parent->children;
    auto found = std::find(std::begin(siblings), std::end(siblings), node);
    int pos = (int)std::distance(std::begin(siblings), found);
    return pos;
}

int interface_store_model::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return (int)static_cast<interface_node*>(parent.internalPointer())->children.size();
    }
    else
    {
        return (int)roots_.size();
    }
}

Qt::ItemFlags interface_store_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return 0;
    }
    else
    {
        return ::QAbstractItemModel::flags(index);
    }
}

bool interface_store_model::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return false;
    }
    beginInsertRows(QModelIndex(), row, row + count - 1);
    roots_.insert(roots_.begin() + row, count, nullptr);
    endInsertRows();
    return true;
}

bool interface_store_model::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return false;
    }

    for (int n {}; n < count; ++n)
    {
        for (const auto &i : roots_)
        {
            if (recurs_search_inheritance(index(row + n, 0, parent).data().toString(), i))
            {
                return false;
            }
        }
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i {}; i < count; ++i)
    {
        auto iterator = static_cast<interface_node*>(this->index(row, 0).internalPointer())->iterator;
        if (iterator != interfaces_.end())
        {
            interfaces_.erase(iterator);
        }
    }
    roots_.erase(roots_.begin() + row, roots_.begin() + row + count);
    endRemoveRows();
    return true;
}

bool interface_store_model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && value.isValid())
    {
        interfaces_.push_back(value.value<interface_data>());
        roots_[index.row()] = create_interface_node(--interfaces_.end(), interfaces_);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant interface_store_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    {
        return QVariant {};
    }
    switch (section)
    {
        case 0:
            return tr("INTERFACES");
        case 1:
            return tr("PACKAGE");
        default:
            return QVariant {};
    }
}

interface_node* create_interface_node(const std::list<interface_data>::const_iterator &data,
                                      const std::list<interface_data> &interfaces)
{
    auto interface_node = new struct interface_node(data->interface_class.first(), interface_node::node_type::interface,
                                                    nullptr);
    for (const auto &j : data->inheritance_classes)
    {
        // if class with same name already exist in the list, create full node,
        // else only name of class
        auto found = std::find_if(std::begin(interfaces), std::end(interfaces), [&](const interface_data &input)
        {
            // here I give only name of interface, if
            // interface is a template. Because instance of
            // template will be have different symbols after
            // <
            if (input.interface_class.first().section('<', 0, 0) == j.section('<', 0, 0))
            {
                return true;
            }
            return false;
        });
        struct interface_node* base_class {};
        if (found != interfaces.end())
        {
            // if we have description of this interface, then we recursively
            // create child node
            base_class = create_interface_node(found, interfaces);
        }
        else
        {
            base_class = new struct interface_node(j, interface_node::node_type::invalid, interface_node);
            interface_node->iterator = interfaces.end();
        }
        base_class->parent = interface_node;
        interface_node->children.push_back(base_class);
    }
    // create node which intake all functions of this interface
    auto functions = new struct interface_node("functions", interface_node::node_type::functions, interface_node);
    functions->iterator = interfaces.end();
    for (const auto &j : data->realized_methods)
    {
        auto func = new struct interface_node(j.arg(" "), interface_node::node_type::func, functions);
        func->iterator = interfaces.end();
        functions->children.push_back(func);
    }
    for (const auto &j : data->abstract_methods)
    {
        auto func = new struct interface_node(j.arg(" ") + " = 0", interface_node::node_type::func, functions);
        func->iterator = interfaces.end();
        functions->children.push_back(func);
    }
    interface_node->children.push_back(functions);
    interface_node->iterator = data;
    return interface_node;
}

QStringList get_all_pure_virtual_methods(const interface_node* node)
{
    QStringList abs_methods = node->iterator->abstract_methods;

    for (const auto &i : node->children)
    {
        if (i->type == interface_node::node_type::interface)
        {
            // here we recursively get parent pure virtual methods
            QStringList parent_abs_methods = get_all_pure_virtual_methods(i);
            // here we check: have override abs method or not
            for (const auto &j : parent_abs_methods)
            {
                if (!node->iterator->realized_methods.contains(j) && !abs_methods.contains(j))
                {
                    abs_methods << j;
                }
            }
        }
    }

    return abs_methods;
}

bool recurs_search_inheritance(const QString &input_interfase_name, const interface_node* node)
{
    if (node)
    {
        for (const auto &i : node->children)
        {
            if (i->type == interface_node::node_type::interface)
            {
                if (input_interfase_name == i->name)
                {
                    return true;
                }
                else
                {
                    if (recurs_search_inheritance(input_interfase_name, i))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
