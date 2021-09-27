#include "ComponentsDialog.h"
#include "DatabaseModule/DatabaseModule.h"
#include "ComponentsDialog/Models/AllComponentsModel.h"

#include <QSqlQueryModel>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>

using namespace dscontrol;

ComponentsDialog::ComponentsDialog()
    : сomponentsTableWidget_{new QTableWidget{this}}
{
    setFixedSize(900, 500);
    setWindowTitle(tr("Components"));

    auto mainLayout = new QHBoxLayout{this};
    setLayout(mainLayout);
    mainLayout->addWidget(сomponentsTableWidget_);

    сomponentsTableWidget_->setColumnCount(3);
    сomponentsTableWidget_->setHorizontalHeaderLabels({tr("Component Name"), tr("Version"), tr("Platform")});
    сomponentsTableWidget_->setColumnWidth(0, 282);
    сomponentsTableWidget_->setColumnWidth(1, 282);
    сomponentsTableWidget_->setColumnWidth(2, 282);
    сomponentsTableWidget_->verticalHeader()->setFixedWidth(30);
    сomponentsTableWidget_->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

    fillComponentsTable(сomponentsTableWidget_, "SELECT name, version, platform FROM Components", 3);
}

void ComponentsDialog::fillComponentsTable(QTableWidget *tableWidget, const QString &query, int columnCount)
{
    auto records = DatabaseModule::query(query, columnCount);
    for (auto i = 0; i < records.size(); i++)
    {
        tableWidget->insertRow(i);

        for (auto j = 0; j < columnCount; j++)
        {
            auto item = new QTableWidgetItem{records[i][j].toString()};
            tableWidget->setItem(i, j, item);
        }
    }
}
