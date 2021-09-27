#include "AboutDialog.hpp"

#include "ui_AboutDialog.h"

namespace dscontrol
{
AboutDialog::AboutDialog(int type, QWidget* parent)
: QDialog(parent)
, ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->contentLabel->setAlignment(Qt::AlignJustify);
    // layout()->setSizeConstraint(QLayout::SetFixedSize);

    if (type == VIDE)
    {
        ui->stackedWidget->setCurrentWidget(ui->videPage);
        setFixedSize(652, 788);
        setVIDEText();
    }
    else if (type == CETC)
    {
        ui->stackedWidget->setCurrentWidget(ui->cetcPage);
        setFixedSize(652, 376);
        setCETCText();
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::changeEvent(QEvent* event)
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

void AboutDialog::setVIDEText()
{
    ui->contentLabel->setTextFormat(Qt::TextFormat::AutoText);
    ui->contentLabel->setText(tr("Program version: 1.0\n\n"

                                 "VISUAL INTEGRATED DEVELOPMENT ENVIRONMENT "
                                 "(VIDE) is a visual component - based system integrated design and "
                                 "development envoriment to software systems design, development, "
                                 "integration and deployment, and other areas. It is designed to meet "
                                 "the needs of an integrated processing cross-platform software system "
                                 "tool of the full-life cycle development and deployment. In "
                                 "distributed architecture, components are presented on different "
                                 "software and hardware platforms and several components can cooperate "
                                 "with one another over a communication network in order to achieve a "
                                 "specific objective or goal. A Component is a modular part of a "
                                 "system, whose behavior is defined by its provided and required "
                                 "interfaces; the internal workings of the Component should be "
                                 "invisible and its usage environment-independent. Source code files, "
                                 "dynamic library files DLLs, So and other artifacts defining the "
                                 "system can be manifested in the Components. A Component can be "
                                 "composed of multiple Classes, or Components pieced together. By "
                                 "building the system in discrete Components, localization of data and "
                                 "behavior enables decreased dependency between Classes and Objects, "
                                 "providing a more robust and maintainable design. The VIDE is a "
                                 "proven, scalable, effective and affordable full life cycle "
                                 "design,development and deploy of distributed processing component "
                                 "based system tool for:\n"
                                 "- Strategic System Designing and Modeling\n"
                                 "- Component Software Design and Construction\n"
                                 "- Software Developing, Testing and Debugging\n"
                                 "- Process Modeling and Simulation\n"
                                 "- System Code Engineering\n"
                                 "- Database Engineering\n"
                                 "- Development of component software\n"
                                 "- Development of XML Schemas\n"
                                 "- Testing and Maintenance\n"
                                 "- Generation Documentation\n\n"

                                 "Visual Environment is an intuitive, flexible "
                                 "and powerful analysis, design and development environment for "
                                 "building robust and maintainable system. From documents creating, "
                                 "through analysis, designing, developing, implementation and testing "
                                 "to deployment and maintenance, the VIDE is a fast, feature-rich, "
                                 "multi-user software envoriment, driving the long-term success of your "
                                 "software project."));
}

void AboutDialog::setCETCText()
{
    ui->contentLabel->setText(tr("China Electronic Technology Group Corporation (CETC) is a backbone "
                                 "state-owned enterprise in China. It was officially established in "
                                 "March 1st, 2002. It is made up of 66 secondary member units, 8 listed "
                                 "companies,42 subsidiaries and over 40 offices overseas. There are "
                                 "150,000 employees,18 national key laboratories, 10 national research "
                                 "centers and innovation centers. In 2016, CETC realized its main "
                                 "business revenue of 188 billion yuan, profit of 18.31 billion yuan, "
                                 "and entered the Fortune Global 500.\n\n"

                                 "Address ：Hefei China,168\n"
                                 "Telephone：+86-551-62724715\n"
                                 "Support Email: ivy@ecthf.com\n"
                                 "Website: www.en.cetc.com.cn"));
}

void AboutDialog::on_pushButton_clicked()
{
    close();
}
} // namespace dscontrol
