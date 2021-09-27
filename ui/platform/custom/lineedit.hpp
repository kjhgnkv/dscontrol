#pragma once

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QLineEdit>
#include <QObject>
#include <QScrollBar>

class QCompleter;

class LineEdit : public QLineEdit
{
Q_OBJECT
public:
    explicit LineEdit(QWidget* parent = nullptr);
    ~LineEdit();

    void setCompleter(QCompleter* completer);
    QCompleter* completer() const;

private slots:
    void insertCompletion(const QString &completion);

private:
    QCompleter* c_;
    QMetaObject::Connection connection_;
};

