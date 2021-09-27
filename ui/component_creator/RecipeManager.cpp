#include "RecipeManager.h"
#include "BlankComponent.h"

#include "CppHeaderFrame.h"
#include "CmakeFrame.h"
#include "BndlspecFrame.h"
#include <QDomDocument>
//init function registry frame and recipe facotres
void RecipeManager::init()
{
	_frameFactroyLibrary = new FrameFactroyLibrary();
	_frameFactroyLibrary->registry(new FrameFactory<CppHeaderFrame>());
	_frameFactroyLibrary->registry(new FrameFactory<CmakeFrame>());
	_frameFactroyLibrary->registry(new FrameFactory<BndlspecFrame>());

	_recipeFactroyLibrary = new RecipeFactoryLibrary();
	_recipeFactroyLibrary->registry(new RecipeFactroy<NO_UIRecipe>());
}

//loadFromXML will get instance of each recipe by xml
//paramters: xmlPath is the path of xml file
//return true if success, or return false
bool RecipeManager::loadFromXML(std::string xmlPath)
{
    QFile file(xmlPath.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();
    QDomElement root=doc.documentElement();
    QDomNode recipeTemplate=root.firstChild();

    while (!recipeTemplate.isNull())
	{
        QDomElement element=recipeTemplate.toElement();
        if(element.tagName().compare("RecipeTemplate") != 0){
                continue;
        }
        QString recipeType=element.attribute("type");
        QString recipeName=element.attribute("name");
        if (_recipes.find(recipeName.toStdString()) != _recipes.end())
            throw std::runtime_error("Duplicate Recipe name:"+ recipeName.toStdString());
        BaseRecipeFactroy::Ptr factory = _recipeFactroyLibrary->getFactoryByName(recipeType.toStdString());
        if (!factory)
            throw std::runtime_error("Can not get factory for:" + recipeType.toStdString());
        QDomNode fileList = recipeTemplate.firstChild();
		BasicRecipe::Ptr recipe = factory->getObject();
		recipe->setFrameFactroyLibrary(_frameFactroyLibrary);
        recipe->loadFramesFromXML(fileList);
        _recipes[recipeName.toStdString()] = recipe;
        recipeTemplate=recipeTemplate.nextSibling();
	}
	return true;
}
//generate function will generate a recipe with given paramters
//paramters: name is recipe name
//paramters is xml formation string that will be passed to recips
//return true if success, or return false
bool RecipeManager::generate(std::string name, std::string paramters)
{
    if(_recipes.find(name)==_recipes.end())
    {
        QString error="Can not find recipe for:";
        error+=name.c_str();
        throw std::runtime_error(error.toStdString());
    }
    BasicRecipe::Ptr recipe = _recipes[name];
    if(!recipe->init(paramters))
		return false;
	if(!recipe->generateCMakeProject())
		return false;
	return true;
}
