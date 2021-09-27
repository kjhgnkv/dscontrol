#pragma once

#include "common/nodeeditor_globals.hpp"
#include <QAbstractTableModel>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QTextCursor>
#include <QTextDocument>

namespace QtNodes
{
class DataFlowModel;
class NodeIndex;
} // namespace QtNodes

namespace dscontrol
{
class ControllManager;

class ProjectDocumentCreator 
{
public:
    ProjectDocumentCreator(ControllManager *manager);
    virtual ~ProjectDocumentCreator();

    void createPdfDocumentation();
    void createOdfDocumentation();

private:
    void addPdfTitle(const QString &title);

    void addPdfByLayers(QtNodes::Item::NodeType type);
    void addPdfByNesting(QtNodes::Item::NodeType type);

    void addOdfByLayers(QtNodes::Item::NodeType type);
    void addOdfByNesting(QtNodes::Item::NodeType type);

    void addPdfChilds(const QtNodes::NodeIndex &parent, int deps);
    void addOdfChilds(const QtNodes::NodeIndex &parent, int deps);

    QPixmap modelTableImage(QAbstractItemModel *model);

    void addOdfImage(const QImage &img, const QString &name);
    void addPdfImage(const QPixmap &pixmap);

private:
    ControllManager *manager_;
    QtNodes::DataFlowModel *model_;
    QPdfWriter *pdfWriter_;
    QPainter *painter_;
    QTextCursor *cursor_;

    QTextDocument document_;
    QTextImageFormat imageFormat_;
    QTextBlockFormat format_;
    QTextBlockFormat newpageformat_;
    QTextCharFormat charformat_;
    QTextCharFormat titlecharformat_;
    QTextCharFormat boldcharformat_;
    int yTextOffset_;
    int pageX_;
    int pageY_;
    int xMargin_;
    int yMargin_;
    int yTextStep_;
    QFont textFont_;
    QFont boldFont_;
    QFont titleFont_;
};
} // namespace dscontrol
