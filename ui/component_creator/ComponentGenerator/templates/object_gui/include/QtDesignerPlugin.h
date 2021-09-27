#pragma once

#include <QObject>
#include <QDesignerCustomWidgetInterface>

class %name%QDesignerWidgetInterface : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit %name%QDesignerWidgetInterface(QObject *parent = nullptr);

    void initialize(QDesignerFormEditorInterface *core) override;
    bool isInitialized() const override;

    QString name() const override;
    QString group() const override;

    QIcon icon() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QString includeFile() const override;

    bool isContainer() const override;

    QString domXml() const override;
    QWidget* createWidget(QWidget *parent) override;

private:
    bool initialized = false;
};

inline %name%QDesignerWidgetInterface::%name%QDesignerWidgetInterface(QObject *parent)
    :QObject(parent)
{
}

inline void %name%QDesignerWidgetInterface::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

inline bool %name%QDesignerWidgetInterface::isInitialized() const
{
    return initialized;
}

inline QString %name%QDesignerWidgetInterface::name() const
{
    return QStringLiteral("%name%Widget");
}

inline QString %name%QDesignerWidgetInterface::group() const
{
    // TODO: write appropriate group name
    return QStringLiteral("DSHost Widgets [Test]");
}

inline QIcon %name%QDesignerWidgetInterface::icon() const
{
    return QIcon();
}

inline QString %name%QDesignerWidgetInterface::toolTip() const
{
    // TODO: write appropriate tool tip
    return QStringLiteral("DSHost test widget");
}

inline QString %name%QDesignerWidgetInterface::whatsThis() const
{
    // TODO: write appropriate widget description
    return QStringLiteral("The test widget helps to understand how to create graphical DSHost components.");
}

inline QString %name%QDesignerWidgetInterface::includeFile() const
{
    // TODO: write appropriate header file
    return QStringLiteral("QtWidget.h");
}

inline bool %name%QDesignerWidgetInterface::isContainer() const
{
    return false;
}

