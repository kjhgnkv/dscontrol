#include "PropertiesDialog.hpp"

#include "ui_PropertiesDialog.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>

namespace dscontrol
{
PropertiesDialog::PropertiesDialog(const QJsonObject &data, QWidget* parent)
: QDialog {parent}
, ui {new Ui::PropertiesDialog}
{
    ui->setupUi(this);

    connect(ui->okBtn, &QPushButton::clicked, this, &PropertiesDialog::accept);

    const std::vector<std::pair<QString, QString>> parsedData = parseJson(data);

    setData(parsedData);
}

void PropertiesDialog::setData(const std::vector<std::pair<QString, QString>> &data)
{
    // Clear content layout
    int count = ui->contentLay->rowCount();
    for (int i = 0; i < count; i++)
    {
        QFormLayout::TakeRowResult row = ui->contentLay->takeRow(i);
        delete row.labelItem;
        delete row.fieldItem;
    }

    // Add records
    for (auto it : data)
    {
        const QString caption = it.first + ":";
        const QString content = it.second;

        QLabel* captionLabel = new QLabel {caption, this};
        QLabel* contentLabel = new QLabel {content, this};

        captionLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
        contentLabel->setStyleSheet("font-size: 14px; color: #ffffff;");

        captionLabel->setAlignment(Qt::AlignTop);
        contentLabel->setAlignment(Qt::AlignTop);

        contentLabel->setWordWrap(true);

        ui->contentLay->addRow(captionLabel, contentLabel);
    }
}

std::vector<std::pair<QString, QString>> PropertiesDialog::parseJson(const QJsonObject &data)
{
    std::vector<std::pair<QString, QString>> vector;

    vector.push_back({"Name", data["moduleName"].toString()});

    QJsonObject info = data["componentInfo"].toObject();

    vector.push_back({tr("GUID"), info["GUID"].toString()});
    vector.push_back({tr("Version"), info["Version"].toString()});
    vector.push_back({tr("Extension"), info["Extension"].toString()});
    vector.push_back({tr("Date of create"), info["CreatedDate"].toString()});
    vector.push_back({tr("Date of update"), info["UpdatedDate"].toString()});
    vector.push_back({tr("Date of upload"), info["UploadDate"].toString()});
    vector.push_back({tr("Last modified"), info["LastmodifiedDate"].toString()});
    vector.push_back({tr("Type"), info["moduleType"].toString()});

    QString sendMessStr {};
    for (const auto item : data["messagesIn"].toArray())
        sendMessStr += QString::number(item.toObject()["id"].toInt()) + ",";
    vector.push_back({tr("Send message ID"), sendMessStr.remove(sendMessStr.length() - 1, 1)});

    QString recvMessStr {};
    for (const auto item : data["messagesOut"].toArray())
        recvMessStr += QString::number(item.toObject()["id"].toInt()) + ",";
    vector.push_back({tr("Recv message ID"), recvMessStr.remove(recvMessStr.length() - 1, 1)});

    vector.push_back({tr("Description"), info["Description"].toString()});

    return vector;
}

void PropertiesDialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui->retranslateUi(this);
                break;
            }
            default:
            {
                break;
            }
        }
        QDialog::changeEvent(event);
    }
}
} // namespace dscontrol
