//#ifndef COMPONENTGENERATOR_H
//#define COMPONENTGENERATOR_H
#pragma once

#include <QDialog>

namespace Ui {
class ComponentGenerator;
}

class ComponentGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit ComponentGenerator(QWidget *parent = nullptr);
    ~ComponentGenerator();

private slots:
    void on_generateBtn_clicked();

    void on_cancelBtn_clicked();

    void on_codeDirectoryBtn_clicked();

    void on_dshostHeadersBtn_clicked();

private:
    Ui::ComponentGenerator *ui;
};

//#endif // COMPONENTGENERATOR_H
