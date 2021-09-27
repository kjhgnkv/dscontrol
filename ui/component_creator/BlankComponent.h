#pragma once
#include "CppHeaderFrame.h"
#include "CmakeFrame.h"
#include "BndlspecFrame.h"
/*class NO_UIRecipe is a recipe for generation frame code
of NO-UI components*/
class NO_UIRecipe : public BasicRecipe
{
public:
#define NO_UI_COMPONENT_NAME "NO_UI_Component"

	typedef Poco::AutoPtr <NO_UIRecipe> Ptr;
	NO_UIRecipe() {};
	~NO_UIRecipe() {};
	virtual bool init(std::string paramters) override;
	static std::string getName() { return NO_UI_COMPONENT_NAME; }
	virtual bool generateCMakeProject() override;
private:
	bool copyCmakeFile();
	std::string _buildcmakePath;
	std::string _macchinacmakePath;
    std::string _case;
    bool _isForCase;
};
