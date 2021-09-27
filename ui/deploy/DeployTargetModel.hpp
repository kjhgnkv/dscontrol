#include <QAbstractItemModel>

namespace dscontrol
{
namespace models
{

class DeployTargetItem;

class DeployTargetModel : public QAbstractItemModel
{
public:
    enum Column
    {
        Name,
        Ip,
        Size
    };

    explicit DeployTargetModel(QObject* parent = nullptr);
    ~DeployTargetModel();

    void setupModelData(const std::vector<std::tuple<int, QString, QString>>& data);

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //<app, ip>
    std::map<QString, QString> selectedApplications() const;
private:
    DeployTargetItem* parentItem(const QModelIndex &index) const;

private:
    DeployTargetItem* root_;
};
}
}
