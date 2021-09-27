#pragma once

#include <QDialog>
#include <QButtonGroup>
#include "RecipeManager.h"
namespace Ui {
class ComponentCreateConfig;
}

#define PATH_SEPARATOR ";"
enum class ComponentType
{
    Blank,
    General,
    Derived,
    Protocol,
    Transport
};

namespace dscontrol
{

class ComponentCreateConfig : public QDialog
{
    Q_OBJECT
    enum DerivedType{
        general=0,
        transport=1,
        protocol=2
    };

public:
    explicit ComponentCreateConfig(QWidget *parent = nullptr);
    ~ComponentCreateConfig();
    QString getConfig();
    QString getComponentTypeName();

private slots:
    void on_buttonOk_clicked();

    void on_buttonCancel_clicked();

    void on_comboBoxType_currentIndexChanged(int index);

    void on_buttonDirectory_clicked();

    void on_buttonAddInclude_clicked();

    void on_buttonAddLibary_clicked();

    void on_pushAddLibDir_clicked();

    void on_pushButton_baseHeader_clicked();

    void on_pushButton_baseFactoryHeader_clicked();

private:
    Ui::ComponentCreateConfig *ui_;
    QButtonGroup* _pGroup;
    RecipeManager::Ptr _recipeManager;
    void setVisibleForDerived(bool visable=false);
    bool checkCommentConfig();
    bool checkDerivedConfig();
    bool findStringBySeparator(QString str, QString value, QString separator);
};

}

