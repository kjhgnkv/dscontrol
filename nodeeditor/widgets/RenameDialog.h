#pragma once

#include <QDialog>

class QLineEdit;

namespace Ui
{
class RenameDialog;
}

class RenameDialog : public QDialog
{
Q_OBJECT

public:
    explicit RenameDialog(const QString &name = QString {}, QWidget* parent = nullptr);
    ~RenameDialog() override;

    QString currentName() const;

protected:
    virtual bool hasNameAlreadyTaken(const QString &name) const;

    void changeEvent(QEvent* event) override;

private:
    void shakeLabel();
    void setLabelText(const QString &newText);
    bool setNewName(const QString &newName);

private slots:
    void lineEditTextEdited(const QString &text);
    void okPushButtonClicked();

private:
    Ui::RenameDialog* ui_;
    QString currentName_;
    const QString takenText_;
    const QString enterNameText_;
    const QString regExpStr_;
};

