#include "ProjectDocumentCreator.hpp"
#include "ControllManager.hpp"
#include "models/CommunicationModel.hpp"
#include "models/ConnectionsStaticModel.hpp"
#include "models/DataFlowModel.hpp"
#include "models/MessageModel.hpp"
#include "singletones/Facade.hpp"
#include <QFileDialog>
#include <QHeaderView>
#include <QString>
#include <QTableView>
#include <QTextDocumentWriter>

#define TITLE_FONT_SIZE 14

namespace dscontrol
{
ProjectDocumentCreator::ProjectDocumentCreator(ControllManager* manager)
: manager_ {manager}
, model_ {QtNodes::Facade::Instance().model()}
, pdfWriter_ {nullptr}
, painter_ {nullptr}
, cursor_ {nullptr}
, yTextOffset_ {}
, pageX_ {}
, pageY_ {}
, xMargin_ {}
, yMargin_ {}
, yTextStep_ {}
{
}

ProjectDocumentCreator::~ProjectDocumentCreator()
{
    if (painter_)
    {
        delete painter_;
    }
    if (pdfWriter_)
    {
        delete pdfWriter_;
    }
    if (cursor_)
    {
        delete cursor_;
    }
}

void ProjectDocumentCreator::createPdfDocumentation()
{
    if (!manager_ || !model_)
    {
        return;
    }

    QString fileName {
    QFileDialog::getSaveFileName(nullptr, QObject::tr("Save File"), "", QObject::tr("PDF document (*.pdf)"))
    };

    if (fileName.isEmpty())
    {
        return;
    }

    if (pdfWriter_)
    {
        delete pdfWriter_;
        pdfWriter_ = nullptr;
    }
    pdfWriter_ = new QPdfWriter(fileName);
    pdfWriter_->setPageSize(QPageSize(QPageSize::A4));
    if (painter_)
    {
        delete painter_;
        painter_ = nullptr;
    }
    painter_ = new QPainter(pdfWriter_);

    textFont_ = painter_->font();
    titleFont_ = textFont_;
    boldFont_ = textFont_;
    titleFont_.setBold(true);
    titleFont_.setPointSize(TITLE_FONT_SIZE);
    boldFont_.setBold(true);

    int dpiX {pdfWriter_->logicalDpiX()};
    int dpiY {pdfWriter_->logicalDpiY()};
    xMargin_ = dpiX / 6;
    yMargin_ = dpiX / 6;
    yTextStep_ = dpiY / 6; // 6 standard lines per inch

    yTextOffset_ = yMargin_ + yTextStep_;
    pageX_ = dpiX * 8;
    pageY_ = dpiY * 11;

    addPdfTitle(QObject::tr("Project Configuration Mapping"));

    auto pixmap = manager_->exportToImage();
    addPdfImage(pixmap);

    pdfWriter_->newPage();
    yTextOffset_ = yMargin_ + yTextStep_;

    addPdfTitle(QObject::tr("Elements in a Project"));

    addPdfByLayers(QtNodes::Item::NodeType::ComponentNode);
    addPdfByLayers(QtNodes::Item::NodeType::ApplicationNode);
    addPdfByLayers(QtNodes::Item::NodeType::ComputerNode);
    addPdfByLayers(QtNodes::Item::NodeType::SubSystemNode);
    addPdfByLayers(QtNodes::Item::NodeType::SystemNode);
    // TODO uncomment to add external components type
    // addPdfByLayers(QtNodes::Item::NodeType::ExternalNode);

    yTextOffset_ += yTextStep_;
    addPdfTitle(QObject::tr("Hierarchy of the Elements in the Project"));

    addPdfByNesting(QtNodes::Item::NodeType::SystemNode);
    addPdfByNesting(QtNodes::Item::NodeType::SubSystemNode);
    addPdfByNesting(QtNodes::Item::NodeType::ComputerNode);
    addPdfByNesting(QtNodes::Item::NodeType::ApplicationNode);
    addPdfByNesting(QtNodes::Item::NodeType::ComponentNode);
    // TODO uncomment to add external components type
    // addPdfByNesting(QtNodes::Item::NodeType::ExternalNode);

    pdfWriter_->newPage();
    yTextOffset_ = yMargin_ + yTextStep_;

    addPdfTitle(QObject::tr("All Communications in the Project, Their Properties"));

    pixmap = modelTableImage(manager_->communicationModel());
    addPdfImage(pixmap);

    pdfWriter_->newPage();
    yTextOffset_ = yMargin_ + yTextStep_;

    addPdfTitle(QObject::tr("List of All Messages in the Project"));

    pixmap = modelTableImage(manager_->messageModel());
    addPdfImage(pixmap);

    pdfWriter_->newPage();
    yTextOffset_ = yMargin_ + yTextStep_;

    addPdfTitle(QObject::tr("All Connections in the Project"));

    ConnectionsStaticModel connsModel;

    pixmap = modelTableImage(&connsModel);

    addPdfImage(pixmap);
}

void ProjectDocumentCreator::createOdfDocumentation()
{
    if (!manager_ || !model_)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save File"), "",
                                                    QObject::tr("ODF document (*.odt)"));

    if (fileName.isEmpty())
    {
        return;
    }

    if (cursor_)
    {
        delete cursor_;
        cursor_ = nullptr;
    }
    cursor_ = new QTextCursor(&document_);

    format_ = cursor_->blockFormat();
    newpageformat_ = format_;
    newpageformat_.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);

    charformat_ = cursor_->charFormat();
    titlecharformat_ = charformat_;
    boldcharformat_ = charformat_;
    titlecharformat_.setFontPointSize(TITLE_FONT_SIZE);
    titlecharformat_.setFontWeight(QFont::Bold);
    boldcharformat_.setFontWeight(QFont::Bold);

    cursor_->insertBlock(format_, titlecharformat_);
    cursor_->insertText(QObject::tr("Project Configuration Mapping"));

    addOdfImage(manager_->exportToImage().toImage(), "Mapping");

    cursor_->insertBlock(newpageformat_, titlecharformat_);
    cursor_->insertText(QObject::tr("Elements in a Project"));

    addOdfByLayers(QtNodes::Item::NodeType::ComponentNode);
    addOdfByLayers(QtNodes::Item::NodeType::ApplicationNode);
    addOdfByLayers(QtNodes::Item::NodeType::ComputerNode);
    addOdfByLayers(QtNodes::Item::NodeType::SubSystemNode);
    addOdfByLayers(QtNodes::Item::NodeType::SystemNode);
    // TODO uncomment to add external components type
    // addOdfByLayers(QtNodes::Item::NodeType::ExternalNode);

    cursor_->insertBlock(format_, titlecharformat_);
    cursor_->insertText(QObject::tr("Hierarchy of the Elements in the Project"));

    addOdfByNesting(QtNodes::Item::NodeType::SystemNode);
    addOdfByNesting(QtNodes::Item::NodeType::SubSystemNode);
    addOdfByNesting(QtNodes::Item::NodeType::ComputerNode);
    addOdfByNesting(QtNodes::Item::NodeType::ApplicationNode);
    addOdfByNesting(QtNodes::Item::NodeType::ComponentNode);
    // TODO uncomment to add external components type
    // addOdfByNesting(QtNodes::Item::NodeType::ExternalNode);

    cursor_->insertBlock(newpageformat_, titlecharformat_);
    cursor_->insertText(QObject::tr("All Communications in the Project, Their Properties"));

    addOdfImage(modelTableImage(manager_->communicationModel()).toImage(), "AllCommunications");

    cursor_->insertBlock(newpageformat_, titlecharformat_);
    cursor_->insertText(QObject::tr("List of All Messages in the Project"));

    addOdfImage(modelTableImage(manager_->messageModel()).toImage(), "AllMessages");

    cursor_->insertBlock(newpageformat_, titlecharformat_);
    cursor_->insertText(QObject::tr("All Connections in the Project"));

    ConnectionsStaticModel connsModel;

    addOdfImage(modelTableImage(&connsModel).toImage(), "AllConnections");

    QTextDocumentWriter writer;
    writer.setFileName(fileName);
    writer.setFormat("odf");
    writer.write(&document_);
}

QPixmap ProjectDocumentCreator::modelTableImage(QAbstractItemModel* model)
{
    QTableView table;
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table.verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    table.setModel(model);

    const int rows {table.model()->rowCount()};
    const int columns {table.model()->columnCount()};
    int totalWidth {table.verticalHeader()->width()};

    for (int column {0}; column < columns; ++column)
    {
        totalWidth += table.columnWidth(column);
    }

    int totalHeight {table.horizontalHeader()->height()};

    table.show();
    for (int row {0}; row < rows; ++row)
    {
        totalHeight += table.rowHeight(row);
    }
    table.hide();
    table.setFixedSize(totalWidth, totalHeight);
    table.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // TODO: test it; QPixmap::grabWidget is obsolete
    // return QPixmap::grabWidget(&table);
    return table.grab();
}

void ProjectDocumentCreator::addPdfImage(const QPixmap &pixmap)
{
    if (!painter_)
    {
        return;
    }

    int width {pixmap.width()};
    int height {pixmap.height()};

    int coef {pageX_ / width};
    if (coef * height > pageY_ - yTextOffset_ - yMargin_)
    {
        coef = (pageY_ - yTextOffset_ - yMargin_) / height;
    }

    painter_->drawPixmap(QRect {xMargin_, yTextOffset_, width * coef, height * coef}, pixmap);
}

void ProjectDocumentCreator::addOdfImage(const QImage &image, const QString &name)
{
    if (!cursor_)
    {
        return;
    }

    auto imgXdots = image.dotsPerMeterX() * 0.16; // dots per A4 X
    auto imgYdots = image.dotsPerMeterY() * 0.25; // dots per A4 Y
    double height {double(image.height())};
    double width {double(image.width())};

    double widthCoef {imgXdots / width};
    double heightCoef {imgYdots / height};
    double coef = widthCoef < heightCoef ? widthCoef : heightCoef;

    document_.addResource(QTextDocument::ImageResource, QUrl(name), image);
    QTextImageFormat imageFormat;
    imageFormat.setName(name);
    imageFormat.setQuality(200);
    imageFormat.setWidth(image.width() * coef);
    imageFormat.setHeight(image.height() * coef);

    cursor_->insertBlock(format_, charformat_);
    cursor_->insertImage(imageFormat);
}

void ProjectDocumentCreator::addPdfTitle(const QString &title)
{
    if (yTextOffset_ > pageY_ - yMargin_)
    {
        pdfWriter_->newPage();
        yTextOffset_ = yMargin_ + yTextStep_;
    }
    painter_->setFont(titleFont_);
    painter_->drawText(QPoint {xMargin_, yTextOffset_}, title);
    painter_->setFont(textFont_);
    yTextOffset_ += 2 * yTextStep_;
}

void ProjectDocumentCreator::addPdfByLayers(QtNodes::Item::NodeType type)
{
    QString nodeType;
    switch (type)
    {
        case QtNodes::Item::NodeType::SystemNode:
        {
            nodeType = QObject::tr("Systems:");
            break;
        }
        case QtNodes::Item::NodeType::SubSystemNode:
        {
            nodeType = QObject::tr("SubSystems:");
            break;
        }
        case QtNodes::Item::NodeType::ComputerNode:
        {
            nodeType = QObject::tr("Computers:");
            break;
        }
        case QtNodes::Item::NodeType::ApplicationNode:
        {
            nodeType = QObject::tr("Applications:");
            break;
        }
        case QtNodes::Item::NodeType::ComponentNode:
        {
            nodeType = QObject::tr("Components:");
            break;
        }
        case QtNodes::Item::NodeType::ExternalNode:
        {
            nodeType = QObject::tr("Externals:");
            break;
        }
        default:
        {
            return;
        }
    }

    painter_->setFont(boldFont_);
    if (yTextOffset_ > pageY_ - yMargin_)
    {
        pdfWriter_->newPage();
        yTextOffset_ = yMargin_ + yTextStep_;
    }
    painter_->drawText(QPoint {xMargin_, yTextOffset_}, nodeType);
    painter_->setFont(textFont_);
    for (const auto &index : model_->nodes(type))
    {
        yTextOffset_ += yTextStep_;
        QString deplType;
        if (model_->nodeDeploymentType(index) == QtNodes::Item::DeploymentType::External)
        {
            deplType = QObject::tr("(E)");
        }
        if (model_->nodeDeploymentType(index) == QtNodes::Item::DeploymentType::Internal)
        {
            deplType = QObject::tr("(I)");
        }
        if (yTextOffset_ > pageY_ - yMargin_)
        {
            pdfWriter_->newPage();
            yTextOffset_ = yMargin_ + yTextStep_;
        }
        painter_->drawText(QPoint {xMargin_, yTextOffset_}, "   -   " + index.nodeCaption() + deplType);
    }
    yTextOffset_ += 2 * yTextStep_;
}

void ProjectDocumentCreator::addPdfChilds(const QtNodes::NodeIndex &parent, int deps)
{
    QString offset {deps * 3, ' '};
    for (auto node : model_->nodeChilds(parent))
    {
        yTextOffset_ += yTextStep_;
        if (yTextOffset_ > pageY_ - yMargin_)
        {
            pdfWriter_->newPage();
            yTextOffset_ = yMargin_ + yTextStep_;
        }
        painter_->drawText(QPoint {xMargin_, yTextOffset_}, offset + "- " + node.nodeCaption());
        addPdfChilds(node, deps + 1);
    }
}

void ProjectDocumentCreator::addPdfByNesting(QtNodes::Item::NodeType type)
{
    for (auto node : model_->nodes(type))
    {
        if (model_->nodeParent(node).isValid())
        {
            continue;
        }
        if (yTextOffset_ > pageY_ - yMargin_)
        {
            pdfWriter_->newPage();
            yTextOffset_ = yMargin_ + yTextStep_;
        }
        painter_->drawText(QPoint {xMargin_, yTextOffset_}, "- " + node.nodeCaption());
        addPdfChilds(node, 1);
        yTextOffset_ += 2 * yTextStep_;

    }
}

void ProjectDocumentCreator::addOdfByLayers(QtNodes::Item::NodeType type)
{
    if (!cursor_)
    {
        return;
    }

    QString nodeType;
    switch (type)
    {
        case QtNodes::Item::NodeType::SystemNode:
        {
            nodeType = QObject::tr("Systems:");
            break;
        }
        case QtNodes::Item::NodeType::SubSystemNode:
        {
            nodeType = QObject::tr("SubSystems:");
            break;
        }
        case QtNodes::Item::NodeType::ComputerNode:
        {
            nodeType = QObject::tr("Computers:");
            break;
        }
        case QtNodes::Item::NodeType::ApplicationNode:
        {
            nodeType = QObject::tr("Applications:");
            break;
        }
        case QtNodes::Item::NodeType::ComponentNode:
        {
            nodeType = QObject::tr("Components:");
            break;
        }
        case QtNodes::Item::NodeType::ExternalNode:
        {
            nodeType = QObject::tr("Externals:");
            break;
        }
        default:
        {
            return;
        }
    }

    cursor_->insertBlock(format_, boldcharformat_);
    cursor_->insertText(nodeType);
    for (const auto &index : model_->nodes(type))
    {
        QString deplType {};
        if (model_->nodeDeploymentType(index) == QtNodes::Item::DeploymentType::External)
        {
            deplType = QObject::tr("(E)");
        }
        if (model_->nodeDeploymentType(index) == QtNodes::Item::DeploymentType::Internal)
        {
            deplType = QObject::tr("(I)");
        }
        cursor_->insertBlock(format_, charformat_);
        cursor_->insertText("   -   " + index.nodeCaption() + deplType);
    }
}

void ProjectDocumentCreator::addOdfChilds(const QtNodes::NodeIndex &parent, int deps)
{
    if (!cursor_)
    {
        return;
    }
    QString offset {deps * 3, ' '};
    for (auto node : model_->nodeChilds(parent))
    {
        cursor_->insertBlock(format_, charformat_);
        cursor_->insertText(offset + "- " + node.nodeCaption());
        addOdfChilds(node, deps + 1);
    }
}

void ProjectDocumentCreator::addOdfByNesting(QtNodes::Item::NodeType type)
{
    if (!cursor_)
    {
        return;
    }
    for (auto node : model_->nodes(type))
    {
        if (!model_->nodeParent(node).isValid())
        {
            cursor_->insertBlock(format_, charformat_);
            cursor_->insertText("- " + node.nodeCaption());
            addOdfChilds(node, 1);
        }
    }
}
} // namespace dscontrol
