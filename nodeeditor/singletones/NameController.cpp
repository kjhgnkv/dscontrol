#include "singletones/NameController.hpp"
#include "nodes/NodeIndex.hpp"
#include "singletones/Facade.hpp"

#include <QApplication>
#include <QDebug>

namespace QtNodes
{
NameController::NameController()
{
}

int NameController::expectedNumber(const std::set<int> &sequence) const
{
    int retval = 0;

    auto it = sequence.begin();
    auto curr = *it;

    if (curr != 0)
        return retval;

    auto end = std::prev(sequence.end(), 1);

    for (; it != end; it++, curr = *it)
    {
        auto tmp = *std::next(it, 1);
        if (curr + 1 != tmp)
        {
            retval = curr + 1;
            return retval;
        }
    }

    retval = (*end) + 1;
    return retval;
}

std::pair<QString, int> NameController::split(const QString &name) const
{
    std::pair<QString, int> retval = {name, 0};

    auto splited = name.split("_");
    if (splited.size() > 1)
    {
        bool isOk = false;
        auto number = splited.takeLast().toInt(&isOk);

        if (isOk)
        {
            retval = {splited.join("_"), number};
        }
    }

    return retval;
}

NameController* NameController::Instance()
{
    static NameController retval;
    return &retval;
}

void NameController::Init()
{
    model_ = Facade::Instance().model();
}

bool NameController::changeItemCaption(const NodeIndex &index, const QString &newCaption)
{
    if (!index.isValid())
    {
        return false;
    }

    if (newCaption.isEmpty())
    {
        return false;
    }

    auto nodeName = index.nodeCaption();

    if (newCaption == nodeName)
    {
        return false;
    }

    itemCaptionRemovedSlot(index);
    itemCaptionAddedSlot(index, newCaption);
    return true;
}

bool NameController::containsCaption(const QString &newCaption) const
{
    std::pair<QString, int> splittedCaption = split(newCaption);
    auto foundedSetIt = allNames_.find(splittedCaption.first);

    if (foundedSetIt == allNames_.end())
    {
        return false;
    }

    auto foundedSet = foundedSetIt->second;
    int captionIdx = splittedCaption.second;
    auto setFound = foundedSet.find(captionIdx);
    if (setFound != foundedSet.end())
    {
        return true;
    }

    captionIdx = expectedNumber(foundedSet);
    setFound = foundedSet.find(captionIdx);
    return setFound != foundedSet.end();
}

void NameController::itemCaptionAddedSlot(const QtNodes::NodeIndex &index, const QString &nodeName)
{
    QString indexName {};

    if (nodeName.isEmpty())
    {
        return;
    }

    std::pair<QString, int> name;

    name = split(nodeName);

    auto found = allNames_.find(name.first);

    if (found != allNames_.end())
    {
        auto setFound = found->second.find(name.second);
        if (setFound != found->second.end())
        {
            name.second = expectedNumber(found->second);
        }

        auto[it, retval] = found->second.insert(name.second);

        if (!retval)
        {
            qCritical() << "Cannot insert new name!";
            return;
        }
    }
    else
    {
        auto[it, retval] = allNames_.insert({name.first, {name.second}});
        if (!retval)
        {
            qCritical() << "Cannot insert new name!";
            return;
        }
    }

    indexName = (name.first + "_" + QString::number(name.second));
    model_->nodeSetCaption(index, indexName);
}

void NameController::itemCaptionRemovedSlot(const QtNodes::NodeIndex &index)
{
    std::pair<QString, int> removingCaption = split(index.nodeCaption());
    if (auto found = allNames_.find(removingCaption.first); found != allNames_.end())
    {
        if (found->second.size() > 1)
        {
            found->second.erase(removingCaption.second);
        }
        else
        {
            allNames_.erase(removingCaption.first);
        }
    }
}

void NameController::clear()
{
    allNames_.clear();
}
} // namespace QtNodes
