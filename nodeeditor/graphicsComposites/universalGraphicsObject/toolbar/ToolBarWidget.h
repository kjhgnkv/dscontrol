#pragma once

#include "common/nodeeditor_globals.hpp"
#include <QWidget>

namespace Ui
{
class ToolBarWidget;
}

namespace QtNodes
{
class ToolBarWidget : public QWidget
{
Q_OBJECT

public:
    ToolBarWidget(QWidget* parent = nullptr);
    ~ToolBarWidget() override;

public:
    struct Properties;
    enum Mode
    {
        Item,
        Line,
        Frame,
        Text
    };

    void setProperties(const QFont &font, const QColor &color, const QColor &backColor, const QColor &borderColor,
                       qint32 borderWidth);
    void setProperties(const Properties &properties);
    void setMode(Mode mode);

protected:
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void borderWidthChanged(qint32 newWidth);
    void itemPropertyChanged(const Item::GraphicsItemProperties &property, const QVariant &value);
    void reseted();
    void interactionHasStarted();
    void interactionHasFinished();

private:
    enum class ColorType
    {
        Color,
        BackColor,
        BorderColor
    };

    void colorButtonClickedHelper(ColorType colorType);
    void updateStyleSheets();
    const QColor selectColor(const QColor &initColor = "#000000");

private slots:
    void fontButtonClicked();
    void colorButtonClicked();
    void backColorButtonClicked();
    void borderColorButtonClicked();
    void spinBoxValueChanged(qint32 value);

private:
    Ui::ToolBarWidget* ui_;
    Properties* properties_;
};
} // namespace QtNodes
