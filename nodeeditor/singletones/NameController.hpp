#pragma once

#include <QObject>
#include <map>
#include <set>

namespace QtNodes
{
class NodeIndex;
class DataFlowModel;

/**
 * @brief This class provides controll above all names of nodes in our project
 */
class NameController final : public QObject
{
Q_OBJECT
public:
    static NameController* Instance();
    void Init();

    bool changeItemCaption(const QtNodes::NodeIndex &index, const QString &newCaption);
    bool containsCaption(const QString &newCaption) const;

public slots:
    void itemCaptionAddedSlot(const QtNodes::NodeIndex &index, const QString &nodeName);
    void itemCaptionRemovedSlot(const QtNodes::NodeIndex &index);

    void clear();

private:
    NameController();
    NameController(NameController &&) = delete;
    NameController(const NameController &) = delete;
    NameController &operator=(const NameController &) = delete;
    NameController &operator=(NameController &&) = delete;

    int expectedNumber(const std::set<int> &sequence) const;
    std::pair<QString, int> split(const QString &name) const;

private:
    std::map<QString, std::set<int>> allNames_;
    DataFlowModel* model_;
};
} // namespace QtNodes
