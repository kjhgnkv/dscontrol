#pragma once

#include <QString>

struct DestData
{
    explicit DestData(const QString &host, const QString &ip, const QString &productName, const QString &architecture)
    : host_(host)
    , ip_(ip)
    , productName_(productName)
    , architecture_(architecture)
    {
    }

    QString host_ {};
    QString ip_ {};
    QString productName_ {};
    QString architecture_ {};
};
