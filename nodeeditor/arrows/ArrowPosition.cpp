#include "ArrowPosition.h"

using namespace QtNodes::Arrows;

QJsonObject ArrowPosition::save() const
{
    const QJsonObject &currentStartPosObj = {{  "x", currentStartPos.x()}
                                             , {"y", currentStartPos.y()}
    };
    const QJsonObject &currentEndPosObj = {{  "x", currentEndPos.x()}
                                           , {"y", currentEndPos.y()}
    };
    const QJsonObject &startRelativeRatioObj = {{  "x", startRelativeRatio.x()}
                                                , {"y", startRelativeRatio.y()}
    };
    const QJsonObject &endRelativeRatioObj = {{  "x", endRelativeRatio.x()}
                                              , {"y", endRelativeRatio.y()}
    };
    const QJsonObject &lastStartPosObj = {{  "x", lastStartPos.x()}
                                          , {"y", lastStartPos.y()}
    };
    const QJsonObject &lastEndPosObj = {{  "x", lastEndPos.x()}
                                        , {"y", lastEndPos.y()}
    };

    return QJsonObject {
    {  "currentStartPos"   , currentStartPosObj}
    , {"currentEndPos"     , currentEndPosObj}
    , {"startRelativeRatio", startRelativeRatioObj}
    , {"endRelativeRatio"  , endRelativeRatioObj}
    , {"lastStartPos"      , lastStartPosObj}
    , {"lastEndPos"        , lastEndPosObj}
    ,
    };
}

void ArrowPosition::restore(const QJsonObject &restoringObject)
{
    const QJsonObject &currentStartPosObj = restoringObject["currentStartPos"].toObject();
    const QJsonObject &currentEndPosObj = restoringObject["currentEndPos"].toObject();
    const QJsonObject &startRelativeRatioObj = restoringObject["startRelativeRatio"].toObject();
    const QJsonObject &endRelativeRatioObj = restoringObject["endRelativeRatio"].toObject();
    const QJsonObject &lastStartPosObj = restoringObject["lastStartPos"].toObject();
    const QJsonObject &lastEndPosObj = restoringObject["lastEndPos"].toObject();

    currentStartPos = {currentStartPosObj["x"].toDouble(), currentStartPosObj["y"].toDouble()};
    currentEndPos = {currentEndPosObj["x"].toDouble(), currentEndPosObj["y"].toDouble()};
    startRelativeRatio = {startRelativeRatioObj["x"].toDouble(), startRelativeRatioObj["y"].toDouble()};
    endRelativeRatio = {endRelativeRatioObj["x"].toDouble(), endRelativeRatioObj["y"].toDouble()};
    lastStartPos = {lastStartPosObj["x"].toDouble(), lastStartPosObj["y"].toDouble()};
    lastEndPos = {lastEndPosObj["x"].toDouble(), lastEndPosObj["y"].toDouble()};
}
