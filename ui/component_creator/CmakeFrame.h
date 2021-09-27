#pragma once
#include "BaseDefine.h"
#define CMAKEFRAME_NAME "CmakeFrame"
/*class CmakeFrame is a frame to generate cmake file*/
class CmakeFrame : public AbstractFrame
{
public:
	typedef Poco::AutoPtr <CmakeFrame> Ptr;

	explicit CmakeFrame(std::map<std::string, std::string>& paramterMap) :
		AbstractFrame(paramterMap), _paramterMap(paramterMap) {
		;
	};
	~CmakeFrame() { ; };
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
	static std::string getName() { return CMAKEFRAME_NAME; }
private:
	std::map<std::string, std::string>& _paramterMap;
};
