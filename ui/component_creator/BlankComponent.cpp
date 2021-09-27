#include "BlankComponent.h"
#include <QCoreApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

//init function will initial this recipe boject from paramters 
//paramters is xml formation string,
//return true if success, or return false
bool NO_UIRecipe::init(std::string paramters) 
{
	//load the xml content from string
    QDomDocument doc;
    doc.setContent(QString(paramters.c_str()),false);

    QDomElement recipeTemplate=doc.documentElement();

    if (recipeTemplate.isNull())
		return false;
	//get destination path and some cmake files included 

    if(!recipeTemplate.isElement())
        return false;
    else {
        QDomElement recipeElement=recipeTemplate.toElement();
        _destinationPath=recipeTemplate.attribute("destination").toStdString();
        _buildcmakePath=recipeTemplate.attribute("buildcmake").toStdString();
        _macchinacmakePath=recipeTemplate.attribute("macchinacmake").toStdString();
        _case=recipeTemplate.attribute("case").toStdString();
        _isForCase=!recipeTemplate.attribute("case").isEmpty();
    }
    QDomNode node=recipeTemplate.firstChild();

    while (!node.isNull())
    {
        if(!node.isElement())
            continue;
        QDomElement e=node.toElement();
        std::string symbol=e.attribute("symbol").toStdString();
        std::string value=e.attribute("value").toStdString();
        if (_paramtersMap.find(symbol) == _paramtersMap.end())
            _paramtersMap[symbol] = value;
        node=node.nextSibling();
    }
	//check destination path and make it ended with path separator
	if (Poco::Path::separator() != *_destinationPath.rbegin())
	{
		_destinationPath += Poco::Path::separator();
	}
	//for each frames in this recipe, get object instance from factory and 
	//initial the object
	_frameObjects.clear();

	for (std::vector< FrameItem::Ptr>::iterator it = _frameItems.begin();
		it != _frameItems.end(); it++)
	{
        BaseFrameFactory::Ptr factory = _frameFactroyLibrary->getFactoryByName((*it)->getType());
		if (!factory)
			return false;
		AbstractFrame::Ptr pFrame = factory->getObject(_paramtersMap);
		if (!pFrame)
			return false;
        std::string filePath = _destinationPath + (*it)->getName();
        pFrame->setFrameItem(*it);
		pFrame->setDestination(filePath);
        pFrame->setContent((*it)->getContent(), _isForCase, _case);
		_frameObjects.push_back(pFrame);
    }
	return true;
}
//copy static cmake files to destination path
//return true if success, or return false
bool NO_UIRecipe::copyCmakeFile()
{
	Poco::File buildfile(_buildcmakePath);
	if (!buildfile.exists())
        throw std::runtime_error("File does not exists:" + _buildcmakePath);
	buildfile.copyTo(_destinationPath);

	Poco::File macchinafile(_macchinacmakePath);
	if (!macchinafile.exists())
        throw std::runtime_error("File does not exists:" + _macchinacmakePath);
	macchinafile.copyTo(_destinationPath);
	return true;
}
//generate cmake project for all frame files
//return true if success, or return false
bool NO_UIRecipe::generateCMakeProject() 
{
	if (!generateFrame())
		return false;
	if(copyCmakeFile())
		return false;
	return true;
}
