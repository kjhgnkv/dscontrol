#pragma once

#include <QDialog>

namespace Ui
{
class FramePropertiesDialog;
}

class FramePropertiesDialog : public QDialog
{
public:
    FramePropertiesDialog(const QString &title, int verSwimlaneCount, int horSwimlaneCount, QWidget* parent = nullptr);
    QString getTitle() const;
    int getVerticalSwimlaneCount() const;
    int getHorizontalSwimlaneCount() const;

private:
    Ui::FramePropertiesDialog* ui_;
};

