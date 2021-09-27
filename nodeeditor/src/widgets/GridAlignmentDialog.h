#pragma once

#include <QDialog>

namespace Ui
{
class GridAlignmentDialog;
}

class GridAlignmentDialog : public QDialog
{
public:
    GridAlignmentDialog(qint32 totalCount);
    ~GridAlignmentDialog();
    qint32 rowCount() const;
    qint32 columnCount() const;

private:
    void correctRowCount(qint32 columnCount);
    void correctColumnCount(qint32 rowCount);

private:
    Ui::GridAlignmentDialog* ui_;
    qint32 totalCount_;
};
