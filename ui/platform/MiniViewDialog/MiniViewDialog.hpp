#pragma once

#include <QDialog>

namespace QtNodes
{
class FlowScene;
class MiniView;
class FlowView;
} // namespace QtNodes

namespace Ui
{
class MiniViewDialog;
}

namespace dscontrol
{
/*!
\brief Class representing pan&zoom dialog
 */
class MiniViewDialog : public QDialog
{
Q_OBJECT

public:
    explicit MiniViewDialog(QtNodes::FlowView* parentView, QtNodes::FlowScene* parentScene, QWidget* parent = nullptr);
    ~MiniViewDialog();

    /*!
    \brief setup new view and scene to represent
     */
    void setNewContent(QtNodes::FlowView* parentView, QtNodes::FlowScene* parentScene);

signals:
    void closed();

private slots:
    /*!
    \brief set zoom slider value on origin view scaling
     */
    void setValue(const int &value);

    /*!
    \brief scale origin view on zoom slider changing value
     */
    void scaleOriginView(const int &value);
    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

    void keyPressEvent(QKeyEvent* event);

private:
    void makeConnections();
    void makeDisconnection();

private:
    Ui::MiniViewDialog* ui;
    QtNodes::FlowScene* scene_;
    QtNodes::FlowView* parentView_;

    std::vector<QMetaObject::Connection> metaConnections_;
};

} // namespace dscontrol
