#pragma once

#include <QSqlDatabase>

class QObject;
class QString;
class QSqlQueryModel;
class QVariant;

namespace dscontrol
{

class DatabaseModule
{
public:
    static void fillModel(const QString &connName, QSqlQueryModel *queryModel, const QString &query);
    static QList<QList<QVariant>> query(const QString &query, int columnCount);

private:
    static QSqlDatabase createDatabase();
};
}
