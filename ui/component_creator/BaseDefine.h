#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include<fstream>
#include<iostream>
#include <stdexcept>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/AutoPtr.h>
#include <Poco/RefCountedObject.h>
#include <Poco/XML/XML.h>

#include <QCoreApplication>
#include <QFile>
#include <QCoreApplication>
#include <QDomDocument>
#include <QDebug>

class BasicUtil {
public:
	//replcae all symbols with value in string
	//paramters: sourceString, the string that will be dealed with and its content will be changed
	//symbol, the string will be replaced
	//value, the string will be applied to replace symbol
	//no return 
	static void replaceAllSymbolsWithValue(std::string& sourceString,
		std::string symbol, std::string value) {
		size_t position = sourceString.find(symbol);
		while (position != sourceString.npos)
		{
			sourceString.replace(position, symbol.size(), value);
			position = sourceString.find(symbol);
		}
	}
};
/*class FrameItem is a describer of template*/
class FrameItem : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <FrameItem> Ptr;
	typedef std::map<std::string, std::string> CaseContent;
	typedef std::map<std::string, CaseContent*> SelectContent;
	FrameItem() { ; };
	~FrameItem() { ; };
	std::string getType() { return _type; };
	void setType(std::string type) { _type = type; }
	std::string getContent() { return _content; };
	void setContent(std::string content) { 
		_content = content; 
	};
	std::string getName() { return _name; };
	void setName(std::string name) { _name = name; };
    //get corresponding content by given module and case
    //paramters: moduleName, the module name
    //caseName, the case name
    //return corresponding content
	std::string getPartContentByCase(std::string moduleName, std::string caseName) {
		std::string content;
		if (_selectContent.find(moduleName) != _selectContent.end()) {
			CaseContent* caseContent = _selectContent[moduleName];
			if (caseContent->find(caseName) != caseContent->end())
				content = (*caseContent)[caseName];
			else
				throw std::runtime_error("can not find case:" + caseName);
		}
		else
			throw std::runtime_error("can not find module:" + moduleName);
		return content;
	}
    //add selectable content from xml node
    //paramters: selectNode, an XML node whose tag is "SelectItem"
    //return nothing
    void addSelectContent(QDomNode& selectNode) {
        QDomElement selectElement=selectNode.toElement();
        if(selectElement.tagName().compare("SelectItem")==0){
            std::string module=selectElement.attribute("module").toStdString();
            CaseContent* caseContent=NULL;
            if (_selectContent.find(module) == _selectContent.end()){
                caseContent = new CaseContent();
                _selectContent[module] = caseContent;
            }
            else
                throw std::runtime_error("Duplicate value:" + module);
            QDomNode caseItem = selectNode.firstChild();
            while (!caseItem.isNull()){
                if(caseItem.isElement()){
                    QDomElement caseElement=caseItem.toElement();
                    if(caseElement.tagName().compare("content")!=0)
                        continue;
                    std::string caseName=caseElement.attribute("case").toStdString();
                    std::string content=caseElement.text().toStdString();
                    if (caseContent->find(caseName) == caseContent->end())
                        (*caseContent)[caseName] = content;
                    else
                        throw std::runtime_error("Duplicate value:" + caseName);
                 }
                caseItem=caseItem.nextSibling();
            }
        }
	}
	void getContentByCase(std::string& Content, std::string caseName)
	{
		for (SelectContent::iterator it = _selectContent.begin();
			it != _selectContent.end(); it++)
		{
			std::string moduleName =it->first;
			std::string partContent= getPartContentByCase(moduleName, caseName);
			BasicUtil::replaceAllSymbolsWithValue(Content, moduleName, partContent);
		}
    }
private:
	std::string _type;
	std::string _content;
	std::string _name;
    SelectContent _selectContent;
};
/*class AbstractFrame is abstract basic class for any file frame*/
class AbstractFrame : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <AbstractFrame> Ptr;
	AbstractFrame(std::map<std::string, std::string>& paramtersMap)
	:_paramtersMap(paramtersMap){ ; };
	~AbstractFrame() { ; };
	virtual bool buildContent() = 0;
	//write content to Disk, if success return true, or return false. 
	virtual bool writeToDisk() {
		Poco::Path path(_destination);
		Poco::File dirPath = path.parent();

		if (!dirPath.exists())
			dirPath.createDirectories();
		std::ofstream outfile(_destination);
        BasicUtil::replaceAllSymbolsWithValue(_content, "\r\n", "\n");
		outfile << _content;
		outfile.close();
		return true;
	};
	void setContent(const std::string content, bool isForCase=false, std::string caseName="") {
		_content = content;
		_isForCase = isForCase;
		_caseName = caseName;
	};
	std::string getContent() {
		if (_isForCase)
		{
			_frameItem->getContentByCase(_content, _caseName);
			return _content;
		}
		else
			return _content; 
	}
	void setDestination(std::string destination) { _destination = destination; }
    std::string getDestination() { return _destination; }
    void setFrameItem(FrameItem::Ptr frameItem) {_frameItem = frameItem;};
protected:
	FrameItem::Ptr _frameItem;
	bool _isForCase;
    std::string _caseName;
    std::string _content;
	std::string _destination;
	std::map<std::string, std::string>& _paramtersMap;
};

/*class Factory is the basic class for factory */
class Factory : public Poco::RefCountedObject {
public:
	typedef Poco::AutoPtr <Factory> Ptr;
	Factory() {};
	~Factory() { ; };
	virtual std::string getName() = 0;
};
class BasicRecipe;
/*class BaseFrameFactory is basic class of FrameFactory*/
class BaseFrameFactory : public Factory
{
public:
	typedef Poco::AutoPtr <BaseFrameFactory> Ptr;
	virtual AbstractFrame::Ptr getObject(std::map<std::string, std::string>& paramtersMap)=0;
};

/*class FrameFactory is a template class 
which can offer an Frame object*/
template <typename T>
class FrameFactory : public BaseFrameFactory
{
public:
	typedef Poco::AutoPtr <FrameFactory<T>> Ptr;
	FrameFactory() { ; }
	virtual std::string getName() override { return T::getName(); }
	virtual AbstractFrame::Ptr getObject(std::map<std::string, std::string>& paramtersMap) override {
		return AbstractFrame::Ptr(new T(paramtersMap)); }
};

/*class FrameFactroyLibrary is a class
that will manage FrameFactory*/
class FrameFactroyLibrary : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <FrameFactroyLibrary> Ptr;
	FrameFactroyLibrary() {};
	bool registry(BaseFrameFactory::Ptr factory) {
		std::string name = factory->getName();
		if (_factories.find(name) == _factories.end()) {
			_factories[name] = factory;
			return true;
		}
		else
		{
			throw std::runtime_error("Duplicate value:" + name);
		}
		return false;
	}
	BaseFrameFactory::Ptr getFactoryByName(std::string name) {
		if (_factories.find(name) != _factories.end()) {
			return _factories[name];
		}
		else
			return nullptr;
	};
private:
	std::map<std::string, BaseFrameFactory::Ptr> _factories;

};

/*class BasicRecipe is abstract basic class of recipe, 
recipe is a set of frames*/
class BasicRecipe : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <BasicRecipe> Ptr;

	~BasicRecipe() {};
	virtual bool init(std::string paramters) = 0;
	//generate all frames in this recipe
	//return true if success, or return false
	virtual bool generateFrame() {

		for (std::vector<AbstractFrame::Ptr>::iterator it = _frameObjects.begin();
			it != _frameObjects.end(); it++) {
			if (!(*it)->buildContent())
				return false;
			if(!(*it)->writeToDisk())
				return false;
		}
		return true;
	};
	//generate cmake project for all frame
	virtual bool generateCMakeProject() = 0;
	static std::string getName() { return ""; }
	
	//anylise xml element and construct frames
	//paramters: config is the xml element
	//return true if succes, or return false;
    bool loadFramesFromXML(QDomNode& config)
	{
        QDomNode fileNode = config.firstChild();

        while (!fileNode.isNull())
		{
            if(fileNode.isElement())
            {
                QDomElement fileElement=fileNode.toElement();
                if(fileElement.tagName().compare("File")!=0)
                    continue;
                FrameItem* ptr = new FrameItem();
                std::string value=fileElement.attribute("type").toStdString();
                ptr->setType(value);
                value=fileElement.attribute("filename").toStdString();
                ptr->setName(value);

                QDomNode subNode=fileNode.firstChild();
                bool find=false;
                while(!subNode.isNull())
                {
                    if(!subNode.isElement())
                    {
                        subNode=subNode.nextSibling();
                        continue;
                    }
                    QDomElement element=subNode.toElement();
                    if(element.tagName().compare("template")==0)
                    {
                        value=element.text().toStdString();
                        qDebug() << element.text();
                        if(!find)
                        {
                            ptr->setContent(value);
                            find=true;
                        }
                        else
                        {
                            throw std::runtime_error("Duplate template :" + ptr->getName());;
                        }
                    }
                    else if(element.tagName().compare("SelectItem")==0)
                    {
                        ptr->addSelectContent(subNode);
                    }
                    subNode=subNode.nextSibling();
                }
                if(!find)
                    throw std::runtime_error("can not find template :" + ptr->getName());
               // ptr->loadSelectContent(fileNode);
                _frameItems.push_back(ptr);
            }
            fileNode=fileNode.nextSibling();
		}
		return true;
	}
	void setFrameFactroyLibrary(FrameFactroyLibrary::Ptr frameFactroyLibrary) 
	{ _frameFactroyLibrary= frameFactroyLibrary; }
	FrameFactroyLibrary::Ptr  getFrameFactroyLibrary()
	{return _frameFactroyLibrary;}

protected:
	std::vector<AbstractFrame::Ptr> _frameObjects;
	std::map<std::string, std::string> _paramtersMap;
	std::vector<FrameItem::Ptr> _frameItems;

	std::string _destinationPath;
	FrameFactroyLibrary::Ptr _frameFactroyLibrary;

    bool getReplaceItem(QDomNode& item)
    {
        QDomElement element=item.toElement();
        std::string symbol;
        std::string value;
        symbol=element.attribute("type").toStdString();
        value=element.attribute("value").toStdString();
        if(_paramtersMap.find(symbol)!=_paramtersMap.end())
            throw std::runtime_error("Duplicate symbol:" + symbol);
        _paramtersMap[symbol] = value;
        return true;
    }
};

/*class BaseRecipeFactroy is basic class of RecipeFactroy*/
class BaseRecipeFactroy : public Factory
{
public:
	typedef Poco::AutoPtr <BaseRecipeFactroy> Ptr;
	virtual BasicRecipe::Ptr getObject() = 0;
};

/*class RecipeFactroy is a template class
which can offer an recipe object*/
template <typename T>
class RecipeFactroy : public BaseRecipeFactroy
{
public:
	typedef Poco::AutoPtr <RecipeFactroy<T>> Ptr;
	RecipeFactroy() { ; };
	~RecipeFactroy() { ; };
	virtual BasicRecipe::Ptr getObject() override { return BasicRecipe::Ptr(new T()); }
	virtual std::string getName() override { return T::getName(); }
};

/*class RecipeFactoryLibrary is a class
that will manage RecipeFactroy*/
class RecipeFactoryLibrary : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <RecipeFactoryLibrary> Ptr;
	RecipeFactoryLibrary() {};
	bool registry(BaseRecipeFactroy::Ptr factory) {
		std::string name = factory->getName();
		if (_factories.find(name) == _factories.end()) {
			_factories[name] = factory;
			return true;
		}
		else
			return false;
	};
	BaseRecipeFactroy::Ptr getFactoryByName(std::string name) {
		if (_factories.find(name) != _factories.end()) {
			return _factories[name];
		}
		else
			return nullptr;
	};
private:
	std::map<std::string, BaseRecipeFactroy::Ptr> _factories;

};
