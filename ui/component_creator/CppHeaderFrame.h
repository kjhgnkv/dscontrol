#pragma once
#include "BaseDefine.h"
#include <QDebug>
#define CPPFRAME_NAME "CppHeaderFrame"
/*class CppHeaderFrame is a frame to generate c++ source and head file*/
class CppHeaderFrame : public AbstractFrame
{
public:
	typedef Poco::AutoPtr <CppHeaderFrame> Ptr;
	
	explicit CppHeaderFrame(std::map<std::string, std::string>& paramterMap) :
		AbstractFrame(paramterMap), _paramterMap(paramterMap) {
		;
	};
	~CppHeaderFrame() { ; };
	//buildContent function will build content with paramter
	//return true if success, or return false
	virtual bool buildContent() override {
		if(!fixFileName(_destination))
			return false;
		_content = getContent();
		for (std::map<std::string, std::string>::iterator map_it = _paramterMap.begin();
			map_it != _paramterMap.end(); map_it++)
		{
			std::string symbol = (*map_it).first;
			std::string name = (*map_it).second;
			BasicUtil::replaceAllSymbolsWithValue(_content, symbol, name);
		}
        qDebug() << QString(_content.c_str());
		return true;
	}
	static std::string getName() { return CPPFRAME_NAME; }
private:
	std::map<std::string, std::string>& _paramterMap;
	//fixFileName function will fix file name and path
	//return true if success, or return false
	bool fixFileName(std::string& fileName) {
		if (_paramterMap.find("[#name]") != _paramterMap.end())
		{
			std::string symbol = "[#name]";
			std::string name = _paramterMap["[#name]"];
			BasicUtil::replaceAllSymbolsWithValue(fileName, symbol, name);
			return true;
		}
		else
			throw std::runtime_error("Can not find necessary paramter: [#name]" );
		return false;
	}
};
