#include <QJsonObject>
#include <QString>
#include <QUuid>
#include <QVector>
#include <vector>

class StoredItem
{
public:
    /*!
    \brief if you add first level item - parent have to be set to nullptr,
     otherwise it have to contain pointer to valid item of first level
     */
    StoredItem(StoredItem *       parent,
               const QString &    type,
               const QString &    caption,
               const QJsonObject &data);

    ~StoredItem();

    void addChildren(const QString &    type,
                     const QString &    caption,
                     const QJsonObject &data);

    /**\brief remove children by row
     */
    void removeChildren(int row);

    const QString &                  type() const;
    const QString &                  caption() const;
    const QJsonObject &              data() const;
    const std::vector<StoredItem *> &children() const;
    StoredItem *                     parent() const;

private:
    StoredItem *              parent_;
    std::vector<StoredItem *> children_;
    QJsonObject               data_;
    QString                   type_;
    QString                   caption_;
};

// class StoredItem
//{
// public:
//    /**\brief if you add first level item - parent have to be set to nullptr,
//     * otherwise it have to contain pointer to valid item of first level
//     */
//    StoredItem(StoredItem* parent, const QString& type, const QString caption,
//    const QString& version,
//               const QJsonObject& data);
//
//    ~StoredItem();
//    StoredItem* addChildren(const QString& type, const QString caption, const
//    QString& version,
//                            const QJsonObject& data);
//
//    /**\brief remove children by row
//     */
//    void removeChildren(int row);
//
//    const QString& type() const;
//    const QString& caption() const;
//    const QString& version() const;
//    const QJsonObject& data() const;
//    const QVector<StoredItem*>& children() const;
//    StoredItem* parent() const;
//
//    QUuid id() const;
//    int row();
//
//    bool operator==(const StoredItem& lhs);
//
// private:
//    StoredItem* parent_;
//    QVector<StoredItem*> children_;
//    QJsonObject data_;
//    QString type_;
//    QString caption_;
//    QString version_;
//    QUuid id_;
//};