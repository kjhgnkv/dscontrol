#include "ComponentCreator.h"

#include <QFile>
#include <QTextStream>
#include <QDir>

static const QString &gitignoreFile {".gitignore"};
static const QString &proFile {"{{plugin_name}}.pro"};
static const QString &activatorSourceFile {"Activator.cpp"};
static const QString &activatorHeaderFile {"Activator.h"};
static const QString &doxyFile {"Doxyfile"};
static const QString &manifestFile {"manifest.pri"};
static const QString &readmeFile {"README.md"};
static const QString &protobufFile {"ProtobufProcessor.h"};
static const QString &wndPluginSourceFile {"src/WindowWidgetPlugin.cpp"};
static const QString &wndPluginHeaderFile {"src/WindowWidgetPlugin.h"};
static const QString &wndWidgetSourceFile {"src/{{wnd_class_name}}.cpp"};
static const QString &wndWidgetHeaderFile {"src/{{wnd_class_name}}.h"};
static const QString &wndWidgetUiFile {"src/{{wnd_class_name}}.ui"};
static const QString &workerSourceFile {"src/{{worker_class_name}}.cpp"};
static const QString &workerHeaderFile {"src/{{worker_class_name}}.h"};
static const QString &modelSourceFile {"src/{{model_class_name}}.cpp"};
static const QString &modelHeaderFile {"src/{{model_class_name}}.h"};

static const QString &path_template {":/manager/components/{{type}}/"};

using namespace dscontrol;

void ComponentCreator::createBlank(const QString &pluginName, const QString &fullName, const QString &shortName,
                                   const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "blank");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
}

void ComponentCreator::createRegisterService(const QString &pluginName, const QString &fullName,
                                             const QString &shortName, const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "register_service");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
}

void ComponentCreator::createInternalBus(const QString &pluginName, const QString &fullName, const QString &shortName,
                                         const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "internal_bus");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
    addInternalBusFiles(directory, path);
}

void ComponentCreator::createCenterWindow(const QString &pluginName, const QString &fullName, const QString &shortName,
                                          const QString &wndName, const QString &wndDispName, const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "center_window");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
    addCentralWindowFiles(pluginName, wndName, wndDispName, directory, path);
}

void ComponentCreator::createCenterWindowInternalBus(const QString &pluginName, const QString &fullName,
                                                     const QString &shortName, const QString &wndName,
                                                     const QString &wndDispName, const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "center_window_internal_bus");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
    addCentralWindowFiles(pluginName, wndName, wndDispName, directory, path);
    addInternalBusFiles(directory, path);
}

void ComponentCreator::createBackstageWorker(const QString &pluginName, const QString &fullName,
                                             const QString &shortName, const QString &workerName,
                                             const QString &workerTitle, const QString &workerDescription,
                                             const QString &threadName, const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "backstage_worker");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
    addWorkerFiles(pluginName, workerName, workerTitle, workerDescription, threadName, directory, path);
    addInternalBusFiles(directory, path);
}

void ComponentCreator::createDataModel(const QString &pluginName, const QString &fullName, const QString &shortName,
                                       const QString &modelName, const QString &modelId, const QString &directory)
{
    QString path = path_template;
    path.replace("{{type}}", "data_model");

    addCommonFiles(pluginName, fullName, shortName, directory, path);
    addModelFiles(pluginName, shortName, modelName, modelId, directory, path);
    addInternalBusFiles(directory, path);
}

void ComponentCreator::addCommonFiles(const QString &pluginName, const QString &fullName, const QString &shortName,
                                      const QString &directory, const QString &path)
{
    QString file = readFile(path + proFile);
    file.replace("{{qt_module}}", "core");
    createFile(directory + pluginName + ".pro", file);

    file = readFile(path + activatorHeaderFile);
    file.replace("{{plugin_name}}", pluginName);
    file.replace("{{full_name}}", fullName);
    file.replace("{{short_name}}", shortName);
    createFile(directory + activatorHeaderFile, file);

    file = readFile(path + activatorSourceFile);
    file.replace("{{plugin_name}}", pluginName);
    file.replace("{{short_name}}", shortName);
    file.replace("{{full_name}}", fullName);
    createFile(directory + activatorSourceFile, file);

    QFile::copy(path + gitignoreFile, directory + gitignoreFile);
    QFile::copy(path + doxyFile, directory + doxyFile);
    // TODO manifest and readme file params?
    QFile::copy(path + manifestFile, directory + manifestFile);
    QFile::copy(path + readmeFile, directory + readmeFile);
}

void ComponentCreator::addInternalBusFiles(const QString &directory, const QString &path)
{
    QFile::copy(path + protobufFile, directory + protobufFile);
}

void ComponentCreator::addCentralWindowFiles(const QString &pluginName, const QString &wndName,
                                             const QString &wndDispName, const QString &directory, const QString &path)
{
    QDir dir {directory};
    if (!dir.exists("src"))
    {
        dir.mkdir("src");
    }

    QString file = readFile(path + proFile);
    file.replace("{{qt_module}}", "core gui");
    file.replace("{{wnd_class_name}}", wndName);
    createFile(directory + pluginName + ".pro", file);

    file = readFile(path + wndPluginSourceFile);
    file.replace("{{wnd_class_name}}", wndName);
    createFile(directory + wndPluginSourceFile, file);

    file = readFile(path + wndWidgetSourceFile);
    file.replace("{{wnd_class_name}}", wndName);
    // TODO make it clear with title, chineese title and icon
    file.replace("{{wnd_name}}", wndDispName);
    file.replace("{{wnd_display_name}}", wndDispName);
    file.replace("{{wnd_icon}}", "");
    createFile(directory + "src/" + wndName + ".cpp", file);

    file = readFile(path + wndWidgetHeaderFile);
    file.replace("{{wnd_class_name}}", wndName);
    file.replace("{{plugin_name}}", pluginName);
    createFile(directory + "src/" + wndName + ".h", file);

    file = readFile(path + wndWidgetUiFile);
    file.replace("{{wnd_class_name}}", wndName);
    file.replace("{{wnd_display_name}}", wndDispName);
    createFile(directory + "src/" + wndName + ".ui", file);

    QFile::copy(path + wndPluginHeaderFile, directory + wndPluginHeaderFile);
}

void ComponentCreator::addWorkerFiles(const QString &pluginName, const QString &workerName, const QString &workerTitle,
                                      const QString &workerDescription, const QString &threadName,
                                      const QString &directory, const QString &path)
{
    QDir dir {directory};
    if (!dir.exists("src"))
    {
        dir.mkdir("src");
    }

    QString file = readFile(path + proFile);
    file.replace("{{qt_module}}", "core");
    file.replace("{{worker_class_name}}", workerName);
    createFile(directory + pluginName + ".pro", file);

    file = readFile(directory + activatorHeaderFile);
    file.replace("{{worker_class_name}}", workerName);
    createFile(directory + activatorHeaderFile, file);

    file = readFile(path + workerSourceFile);
    file.replace("{{worker_class_name}}", workerName);
    file.replace("{{thread_class_name}}", threadName);
    file.replace("{{worker_name}}", workerTitle);
    file.replace("{{worker_description}}", workerDescription);
    createFile(directory + "src/" + workerName + ".cpp", file);

    file = readFile(path + workerHeaderFile);
    file.replace("{{worker_class_name}}", workerName);
    file.replace("{{thread_class_name}}", threadName);
    createFile(directory + "src/" + workerName + ".h", file);

}

void ComponentCreator::addModelFiles(const QString &pluginName, const QString &shortName, const QString &modelName,
                                     const QString &modelId, const QString &directory, const QString &path)
{
    QDir dir {directory};
    if (!dir.exists("src"))
    {
        dir.mkdir("src");
    }

    QString file = readFile(path + proFile);
    file.replace("{{qt_module}}", "core");
    file.replace("{{model_class_name}}", modelName);
    createFile(directory + pluginName + ".pro", file);

    file = readFile(directory + activatorHeaderFile);
    file.replace("{{model_class_name}}", modelName);
    createFile(directory + activatorHeaderFile, file);

    file = readFile(directory + activatorSourceFile);
    file.replace("{{model_class_name}}", modelName);
    file.replace("{{model_id}}", modelId);
    createFile(directory + activatorSourceFile, file);

    file = readFile(path + modelSourceFile);
    file.replace("{{model_class_name}}", modelName);
    file.replace("{{model_id}}", modelId);
    file.replace("{{short_name}}", shortName);
    createFile(directory + "src/" + modelName + ".cpp", file);

    file = readFile(path + modelHeaderFile);
    file.replace("{{model_class_name}}", modelName);
    createFile(directory + "src/" + modelName + ".h", file);
}

QString ComponentCreator::readFile(const QString &file)
{
    if (!QFile::exists(file))
    {
        return {};
    }
    QFile f(file);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        return {};
    }
    QTextStream in(&f);
    QString s;
    s.append(in.readAll());

    f.close();
    return s;
}

void ComponentCreator::createFile(const QString &file, const QString &text)
{
    QFile outputFile(file);
    outputFile.open(QIODevice::WriteOnly);

    QTextStream outStream(&outputFile);
    outStream << text;

    outputFile.close();
}
