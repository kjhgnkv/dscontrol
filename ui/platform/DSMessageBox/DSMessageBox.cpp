#include "DSMessageBox.hpp"

#include "ui_DSMessageBox.h"
#include <QPushButton>

namespace dscontrol
{
DSMessageBox::DSMessageBox(DSMessageBox::Icon icon, const QString &title, const QString &text, int buttons,
                           QWidget* parent, Qt::WindowFlags f)
: QDialog(parent)
, ui_(new Ui::DSMessageBox)
{
    ui_->setupUi(this);

    setup();
    setWindowFlags(f);
    setWindowTitle(title);
    addButtons(buttons);
    setIcon(icon);
    setText(text);
}

DSMessageBox::~DSMessageBox()
{
    delete ui_;
}

void DSMessageBox::setIcon(Icon type)
{
    QPixmap image;
    switch (type)
    {
        case Information:
            image = {":/images/resources/images/toolImage.png"};
            break;
        case Warning:
            image = {":/images/resources/images/toolImage.png"};
            break;
        case Critical:
            image = {":/images/resources/images/toolImage.png"};
            break;
        case Question:
            image = {":/images/resources/images/toolImage.png"};
            break;
        default:
            break;
    }
    setIcon(image);
}

void DSMessageBox::setIcon(const QPixmap &image)
{
    ui_->lblImage->setPixmap(image);
}

void DSMessageBox::setText(const QString &text)
{
    ui_->textBox->setText(text);
}

void DSMessageBox::addButtons(int buttons)
{
    if (buttons & StandardButtons::Pdf)
    {
        addButton(new QPushButton(), StandardButtons::Pdf);
    }
    if (buttons & StandardButtons::Odf)
    {
        addButton(new QPushButton(), StandardButtons::Odf);
    }
    if (buttons & StandardButtons::File)
    {
        addButton(new QPushButton(), StandardButtons::File);
    }
    if (buttons & StandardButtons::Clipboard)
    {
        addButton(new QPushButton(), StandardButtons::Clipboard);
    }
    if (buttons & StandardButtons::Ok)
    {
        addButton(new QPushButton(), StandardButtons::Ok);
    }
    if (buttons & StandardButtons::Yes)
    {
        addButton(new QPushButton(), StandardButtons::Yes);
    }
    if (buttons & StandardButtons::Cancel)
    {
        addButton(new QPushButton(), StandardButtons::Cancel);
    }
    if (buttons & StandardButtons::No)
    {
        addButton(new QPushButton(), StandardButtons::No);
    }
}

void DSMessageBox::addButton(QPushButton* button, int type)
{
    if (button == nullptr)
        return;

    button->setParent(this);
    QString color = "#27AE60";
    switch (type)
    {
        case Ok:
        {
            button->setText(tr("OK"));
            color = "#27AE60";
        }
            break;
        case Cancel:
        {
            button->setText(tr("Cancel"));
            color = "#EB5757";
        }
            break;
        case Yes:
        {
            button->setText(tr("Yes"));
            color = "#27AE60";
        }
            break;
        case No:
        {
            button->setText(tr("No"));
            color = "#EB5757";
        }
            break;
        case File:
        {
            button->setText(tr("File"));
            color = "gray";
        }
            break;
        case Clipboard:
        {
            button->setText(tr("Clipboard"));
            color = "gray";
        }
            break;
        case Pdf:
        {
            button->setText(tr("PDF"));
            color = "gray";
        }
            break;
        case Odf:
        {
            button->setText(tr("ODT"));
            color = "gray";
        }
            break;
        default:
        {
            return;
        }
    }
    QString styleSheet = "QPushButton"
                         "{"
                         "background-color: " + color + ";"
                                                        "border: 1px solid #3B3E3D;"
                                                        "border-radius: 5px;"
                                                        "font-size: 14px;"
                                                        "color: #FFFFFF;"
                                                        "}";
    button->setStyleSheet(styleSheet);
    button->setFixedSize(88, 24);
    connect(button, &QPushButton::clicked, this, [this, type]()
    {
        keyPressed(type);
    });
    ui_->buttonBoxLayout->addWidget(button);
}

void DSMessageBox::setup()
{
    setStyleSheet("background-color: #3D3D46;"
                  "color: white;"
                  "font-family: Source Sans Pro;"
                  "font-size: 14px;");
}

void DSMessageBox::keyPressed(int type)
{
    close();
    setResult(type);
}

void DSMessageBox::about(QWidget* parent, const QString &title, const QString &text)
{
    DSMessageBox msg(NoIcon, title, text, NoButton, parent);
    msg.exec();
}

int DSMessageBox::critical(QWidget* parent, const QString &title, const QString &text,
                           DSMessageBox::StandardButtons buttons)
{
    DSMessageBox msg(Critical, title, text, buttons, parent);
    return msg.exec();
}

int DSMessageBox::information(QWidget* parent, const QString &title, const QString &text,
                              DSMessageBox::StandardButtons buttons)
{
    DSMessageBox msg(Information, title, text, buttons, parent);
    return msg.exec();
}

int DSMessageBox::question(QWidget* parent, const QString &title, const QString &text,
                           DSMessageBox::StandardButtons buttons)
{
    DSMessageBox msg(Question, title, text, buttons, parent);
    return msg.exec();
}

int DSMessageBox::warning(QWidget* parent, const QString &title, const QString &text,
                          DSMessageBox::StandardButtons buttons)
{
    DSMessageBox msg(Warning, title, text, buttons, parent);
    return msg.exec();
}
} // namespace dscontrol
