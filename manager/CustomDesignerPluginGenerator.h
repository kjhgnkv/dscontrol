#pragma once

class QString;
class QDir;
class QFile;

namespace dscontrol
{
class CustomDesignerPluginGenerator
{
private:
    enum class FileType;

private:
    CustomDesignerPluginGenerator() = delete;
    ~CustomDesignerPluginGenerator() = delete;
    CustomDesignerPluginGenerator(const CustomDesignerPluginGenerator &another) = delete;
    CustomDesignerPluginGenerator(CustomDesignerPluginGenerator &&another) = delete;
    CustomDesignerPluginGenerator &operator=(const CustomDesignerPluginGenerator &another) = delete;
    CustomDesignerPluginGenerator &operator=(CustomDesignerPluginGenerator &&another) = delete;

public:
    static void generate(const QString &widgetName, const QString &widgetUiPath);

private:
    static void generateProjectFiles(const QString &widgetProFilePath, const QString &widgetName,
                                     const QDir &widgetUiFileDir);
    static void generateFile(const QString &filePath, FileType fileType, const QString &widgetName);
    static QString generateFileName(const QString &fileName, FileType fileType);
    static QString generatePath(const QDir &dir, const QString &filename);
    static QString transformStringToLowerFirstLetter(const QString &str);
    static void changeFileSubstring(QFile &file, const QString &previousSubstring, const QString &newSubstring);
};
}
