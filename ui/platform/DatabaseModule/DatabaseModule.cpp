#include "DatabaseModule.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

using namespace dscontrol;

void DatabaseModule::fillModel(const QString &connName, QSqlQueryModel *queryModel, const QString &query)
{
    auto db = QSqlDatabase::database(connName);

    if (db.isOpen())
    {
        queryModel->setQuery(query);
        db.close();
    }
}

QList<QList<QVariant>> DatabaseModule::query(const QString &query, int columnCount)
{
    auto db = createDatabase();
    if (db.open())
    {
        auto sqlQuery = QSqlQuery{query};

        QList<QList<QVariant>> records;
        while (sqlQuery.next())
        {
            QList<QVariant> fields;
            for (auto i = 0; i < columnCount; i++)
            {
                fields.append(sqlQuery.value(i));
            }
            records.append(fields);
        }
        db.close();

        return records;
    }
    return {};
}

QSqlDatabase DatabaseModule::createDatabase()
{
    auto db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("Cetc_2021");
    db.setDatabaseName("Platform");

    return db;
}
