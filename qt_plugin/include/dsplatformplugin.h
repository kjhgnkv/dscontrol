#pragma once

#include "dsplatform_global.h"
#include <extensionsystem/iplugin.h>

namespace DSPlatform
{
namespace Internal
{
class DSMode;

/**\brief simple plugin which add functionality of DSPlatform in QtCreator
 */
class DSPlatformPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT Q_PLUGIN_METADATA(IID
    "org.qt-project.Qt.QtCreatorPlugin" FILE "DSPlatform.json")

public:
    DSPlatformPlugin();
    ~DSPlatformPlugin();

    bool initialize(const QStringList &arguments, QString* errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private
    slots:
    /**\brief this method open input ptoject in EDIT_MODE
     * \param project_file file which describe project (cmake)
     */
    void open_component_in_ide(
    const QString &project_file
    );

private:
    // this is DS mode for Qt
    DSMode* mode;
};

} // namespace Internal
} // namespace DSPlatform
