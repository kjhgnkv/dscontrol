#pragma once

#include <QDialog>

class QTableWidget;

class ComponentsDialog : public QDialog
{
public:
    ComponentsDialog();

private:
    void fillComponentsTable(QTableWidget *tableWidget, const QString &query, int columnCount);

private:
    QTableWidget *сomponentsTableWidget_;
};
