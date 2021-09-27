#include "include/dsplatformplugin.h"
#include "manager/ControllManager.hpp"
#include "ui/platform/Platform.hpp"
#include "include/dsplatformconstants.h"
#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <coreplugin/idocumentfactory.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>

namespace DSPlatform
{
namespace Internal
{
using dscontrol::ControllManager;
using dscontrol::Platform;

/**\brief mode for QtCreator
 */
class DSMode : public Core::IMode
{
public:
    explicit DSMode(QObject* parent)
    : Core::IMode(parent)
    {
        // withowt menu
        auto widget = new Platform(true);

        auto manager = new ControllManager {widget};
        widget->setControllManager(manager);
        setWidget(widget);
        setContext(Core::Context("DS.MainView"));
        setDisplayName(tr("DSPlatform"));
        // here I use fromUtf8 because qmake can define macros which disable
        // QString(const char *) konstructor

        setIcon(QIcon(QString::fromUtf8("/home/dancon/Platformv2.0/DSControll/qt_plugin/splash.png")));

        setPriority(0);
        setId(DSPlatform::Constants::DS_MODE);
        //setContextHelpId(QString());
    }
};

/**\brief factory, which open our project file (.dspl)
 */
class DSDocumentFactory : public Core::IDocumentFactory
{
public:
    explicit DSDocumentFactory(DSMode* mode, QObject* parent = nullptr)
    : Core::IDocumentFactory(parent)
    {
        addMimeType(DSPlatform::Constants::DS_MIME_TYPE);
        // this opener return always nullptr, this is neded for open only our
        // mode, and not open EDIT_MODE. And we not need IDocument for work
        setOpener([mode](const QString &filename)
                  {
                      auto manager = reinterpret_cast<Platform*>(mode->widget())->controllManager();

                      manager->openProject(filename);
                      ::Core::ModeManager::activateMode(mode->id());
                      ::Core::ModeManager::setFocusToCurrentMode();
                      return nullptr;
                  });
    }
};

DSPlatformPlugin::DSPlatformPlugin()
: mode {nullptr}
{
}

DSPlatformPlugin::~DSPlatformPlugin()
{
    if (mode)
    {
        delete mode;
    }
}

bool DSPlatformPlugin::initialize(const QStringList &arguments, QString* errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    mode = new DSMode(this);
    mode->setEnabled(true);
    //ExtensionSystem::IPlugin::addObject(mode);

    new DSDocumentFactory(mode, this);

    connect(reinterpret_cast<Platform*>(mode->widget())->controllManager(), &ControllManager::openComponentInIde, this,
            &DSPlatformPlugin::open_component_in_ide);

    return true;
}

void DSPlatformPlugin::open_component_in_ide(const QString &file_name)
{
    Core::ICore::instance()->openFiles(QStringList(file_name), Core::ICore::SwitchMode);
}

void DSPlatformPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag DSPlatformPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

} // namespace Internal
} // namespace DSPlatform
