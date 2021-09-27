#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QDialog>

class QListWidgetItem;

namespace Ui
{
class ImageSelector;
}

class NODEEDITOR_EXPORT ImageSelector : public QDialog
{
Q_OBJECT

public:
    explicit ImageSelector(QString curPicturePath, QWidget* parent = nullptr);
    ~ImageSelector();
    const QString picturePath();

private slots:
    void itemSelected();

    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_resetBtn_clicked();

private:
    void refresh();
    void setPictureForLabel(QString &path);
    void selectItemByFilename(const QString &filename);

private:
    Ui::ImageSelector* ui;

    QStringList m_filenames;
    QString m_picturesDir;
    QString m_picturePath, m_curPicturePath;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* event) override;

    // QWidget interface
protected:
    void changeEvent(QEvent*);
};
