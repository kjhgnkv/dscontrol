#include "ImageSelector.hpp"

#include "common/globals.h"
#include "ui_ImageSelector.h"
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>

ImageSelector::ImageSelector(QString curPicturePath, QWidget* parent)
: QDialog(parent)
, ui(new Ui::ImageSelector)
, m_picturesDir {"pictures/"}
, m_picturePath {}
, m_curPicturePath {curPicturePath}
{
    ui->setupUi(this);
    setStyleSheet(_DEF_STYLESHEET);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &ImageSelector::itemSelected);
    refresh();

    if (ui->listWidget->count() > 0)
        ui->listWidget->setCurrentRow(0);

    selectItemByFilename(m_curPicturePath);
}

ImageSelector::~ImageSelector()
{
    delete ui;
}

const QString ImageSelector::picturePath()
{
    return m_curPicturePath;
}

void ImageSelector::itemSelected()
{
    QListWidgetItem* item = ui->listWidget->currentItem();

    if (!item)
    {
        return;
    }

    m_picturePath = item->text();
    if (!QFile::exists(m_picturePath))
        m_picturePath = item->text();

    setPictureForLabel(m_picturePath);

    // Enable buttons
    bool deleteFlag = false;
    if (m_picturePath[0] != ':')
        deleteFlag = true;
    ui->btnDelete->setEnabled(deleteFlag);
}

void ImageSelector::setPictureForLabel(QString &path)
{
    QSize size = ui->lblImage->size();
    size.setWidth(size.width() - 2);
    size.setHeight(size.height() - 2);

    QPixmap pixmap {path};

    pixmap = pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation) /*QPixmap(imagesDir_
                                + item->text()) .scaled({width, height}, Qt::KeepAspectRatio)*/
    ;
    ui->lblImage->setPixmap(pixmap);
}

void ImageSelector::refresh()
{
    ui->listWidget->clear();
    QDir directory(m_picturesDir);
    m_filenames = directory.entryList(QStringList() << "*.*", QDir::Files);

    // Get pictures from resources
    m_filenames.clear();
    QDirIterator it {":/pictures"};
    while (it.hasNext())
    {
        it.next();
        m_filenames.push_back(":/pictures/" + it.fileName());
    }

    // Get pictures from files
    QDirIterator it_ {m_picturesDir};
    while (it_.hasNext())
    {
        it_.next();
        m_filenames.push_back(m_picturesDir + it_.fileName());
    }

    ui->listWidget->addItems(m_filenames);
}

void ImageSelector::on_btnAdd_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);

    if (!QDir(m_picturesDir).exists())
    {
        QDir().mkdir(m_picturesDir);
    }

    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        QString newFile;
        foreach (QString file, fileNames)
            {
                QFile fileName(file);
                QFileInfo finfo(fileName);
                newFile = finfo.fileName();
                QFile::copy(file, m_picturesDir + newFile);
            }
        refresh();

        selectItemByFilename(m_picturesDir + newFile);
    }
}

void ImageSelector::selectItemByFilename(const QString &filename)
{
    int count = ui->listWidget->count();
    for (int i = 0; i < count; i++)
    {
        if (ui->listWidget->item(i)->text() == filename)
        {
            ui->listWidget->setCurrentRow(i);
            break;
        }
    }
}

void ImageSelector::on_btnOk_clicked()
{
    m_curPicturePath = m_picturePath;
    QDialog::accept();
}

void ImageSelector::on_btnCancel_clicked()
{
    QDialog::reject();
}

void ImageSelector::on_resetBtn_clicked()
{
    m_curPicturePath = "";
    QDialog::accept();
}

void ImageSelector::on_btnDelete_clicked()
{
    if (QMessageBox::question(this, tr("Deleting"), tr("Do you realy want to delete image?")) == QMessageBox::Yes)
    {
        QFile::remove(m_picturePath);

        if (m_picturePath == m_curPicturePath)
        {
            on_resetBtn_clicked();
        }

        on_resetBtn_clicked();

        ui->lblImage->setPixmap({});

        int curRowNum = ui->listWidget->currentRow();

        refresh();

        if (curRowNum >= ui->listWidget->count())
            curRowNum = ui->listWidget->count() - 1;

        ui->listWidget->setCurrentRow(curRowNum);
    }
}

void ImageSelector::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
}

void ImageSelector::changeEvent(QEvent* event)
{
    if (event)
    {
        [[]] switch (event->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
        QDialog::changeEvent(event);
    }
}
