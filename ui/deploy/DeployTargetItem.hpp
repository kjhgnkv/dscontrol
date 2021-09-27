#include <QString>
#include <QSet>

namespace dscontrol
{
namespace models
{
class DeployTargetItem final
{
public:
    explicit DeployTargetItem(const QString &name, const QString &ip
        , bool ipVisible, DeployTargetItem* parent = nullptr);
    ~DeployTargetItem();

    void addChildren(DeployTargetItem* child);

    void setIp(const QString &ip);
    void setIpVisible(bool ipVisible);
    void setChecked(Qt::CheckState check);

    void clear();
    QString name() const;
    QString ip() const;
    bool isIpVisible() const;
    Qt::CheckState isChecked() const;

    DeployTargetItem* parent() const;
    QList<DeployTargetItem*> children() const;
    DeployTargetItem* child(int row) const;
    int childIndex(DeployTargetItem* child) const;

private:
    QString name_;
    QString ip_;
    bool ipVisible_;
    Qt::CheckState checkState_;

    DeployTargetItem* parent_;
    QList<DeployTargetItem*> children_;
};

}
}
