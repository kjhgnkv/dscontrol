#pragma once

#include <QString>

namespace dscontrol
{

class ComponentCreator
{
public:
    ComponentCreator() = delete;
    ~ComponentCreator() = delete;
    ComponentCreator(const ComponentCreator &another) = delete;
    ComponentCreator(ComponentCreator &&another) = delete;
    ComponentCreator &operator=(const ComponentCreator &another) = delete;
    ComponentCreator &operator=(ComponentCreator &&another) = delete;

public:
    static void createBlank(const QString &pluginName, const QString &fullName, const QString &shortName,
                            const QString &directory);
    static void createRegisterService(const QString &pluginName, const QString &fullName, const QString &shortName,
                                      const QString &directory);
    static void createInternalBus(const QString &pluginName, const QString &fullName, const QString &shortName,
                                  const QString &directory);
    static void createCenterWindow(const QString &pluginName, const QString &fullName, const QString &shortName,
                                   const QString &wndName, const QString &wndDispName, const QString &directory);
    static void createCenterWindowInternalBus(const QString &pluginName, const QString &fullName,
                                              const QString &shortName, const QString &wndName,
                                              const QString &wndDispName, const QString &directory);
    static void createBackstageWorker(const QString &pluginName, const QString &fullName, const QString &shortName,
                                      const QString &workerName, const QString &workerTitle,
                                      const QString &workerDescription, const QString &threadName,
                                      const QString &directory);
    static void createDataModel(const QString &pluginName, const QString &fullName, const QString &shortName,
                                const QString &modelName, const QString &modelId, const QString &directory);

private:
    static QString readFile(const QString &file);
    static void createFile(const QString &file, const QString &text);

    static void addCommonFiles(const QString &pluginName, const QString &fullName, const QString &shortName,
                               const QString &directory, const QString &path);
    static void addInternalBusFiles(const QString &directory, const QString &path);
    static void addCentralWindowFiles(const QString &pluginName, const QString &wndName, const QString &wndDispName,
                                      const QString &directory, const QString &path);
    static void addWorkerFiles(const QString &pluginName, const QString &workerName, const QString &workerTitle,
                               const QString &workerDescription, const QString &threadName, const QString &directory,
                               const QString &path);
    static void addModelFiles(const QString &pluginName, const QString &shortName, const QString &modelName,
                              const QString &modelId, const QString &directory, const QString &path);

};
}

