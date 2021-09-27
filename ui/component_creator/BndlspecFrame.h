#pragma once
#include "BaseDefine.h"
#define BNDLSPECFRAME_NAME "BndlspecFrame"
/*class BndlspecFrame is a frame to generate  Bndlspec file*/
class BndlspecFrame : public AbstractFrame
{
public:
	typedef Poco::AutoPtr <BndlspecFrame> Ptr;

	explicit BndlspecFrame(std::map<std::string, std::string>& paramterMap) :
		AbstractFrame(paramterMap), _paramterMap(paramterMap) {
		;
	};
	~BndlspecFrame() { ; };
	//this function will build content with paramter
	//in this case will always reture true
	virtual bool buildContent() override {
		for (std::map<std::string, std::string>::iterator map_it = _paramterMap.begin();
			map_it != _paramterMap.end(); map_it++)
		{
			std::string symbol = (*map_it).first;
			std::string name = (*map_it).second;
            BasicUtil::replaceAllSymbolsWithValue(_content, symbol, name);
		}
		return true;
	}

	static std::string getName() { return BNDLSPECFRAME_NAME; }
private:
	std::map<std::string, std::string>& _paramterMap;
};
