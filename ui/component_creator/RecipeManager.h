#pragma once
#include "BaseDefine.h"


#include <QDebug>
/*class RecipeManager manage all existing recipes*/
class RecipeManager : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr <RecipeManager> Ptr;
	RecipeManager() {
		;
	};
	~RecipeManager() {
		;
	};
	void init();
	bool loadFromXML(std::string xmlPath);
	bool generate(std::string name, std::string paramters);
private:
	FrameFactroyLibrary::Ptr _frameFactroyLibrary;
	RecipeFactoryLibrary::Ptr _recipeFactroyLibrary;
	std::map<std::string, BasicRecipe::Ptr> _recipes;
};
