set(COMP_CREATOR_INCLUDE
    component_parser.hpp
    component_creator.hpp
    edit_component_dialog.hpp
    interface_store_model.hpp
    interfaces_select_dialog.hpp
    proxy_interface_model.hpp
    IDEProjectProperties.hpp
    ComponentCreateConfig.h
    RecipeManager.h
    BlankComponent.h
    BndlspecFrame.h
    CmakeFrame.h
    CppHeaderFrame.h
	
	ComponentGenerator/ComponentGenerator.h
	ComponentGenerator/generator.h
	ComponentGenerator/wizard.h
	ComponentGenerator/installer.h
	ComponentGenerator/global.h
	
	ComponentGenerator/custom/generator.h
	ComponentGenerator/custom/wizard.h
	
	ComponentGenerator/object/generator.h
	ComponentGenerator/object/wizard.h
	
	ComponentGenerator/protocol/generator.h
	ComponentGenerator/protocol/wizard.h
	
	ComponentGenerator/transport/generator.h
	ComponentGenerator/transport/wizard.h
    )

set(COMP_CREATOR_SRC
    component_parser.cpp
    component_creator.cpp
    edit_component_dialog.cpp
    interface_store_model.cpp
    interfaces_select_dialog.cpp
    proxy_interface_model.cpp
    IDEProjectProperties.cpp
    ComponentCreateConfig.cpp
    RecipeManager.cpp
    BlankComponent.cpp
	
	ComponentGenerator/ComponentGenerator.cpp
	ComponentGenerator/generator.cpp
	ComponentGenerator/wizard.cpp
	ComponentGenerator/installer.cpp
	ComponentGenerator/global.cpp
	
	ComponentGenerator/custom/generator.cpp
	ComponentGenerator/custom/wizard.cpp
	
	ComponentGenerator/object/generator.cpp
	ComponentGenerator/object/wizard.cpp
	
	ComponentGenerator/protocol/generator.cpp
	ComponentGenerator/protocol/wizard.cpp
	
	ComponentGenerator/transport/generator.cpp
	ComponentGenerator/transport/wizard.cpp
    )

set(COMP_CREATOR_UI
    edit_component_dialog.ui
    interfaces_select_dialog.ui
    IDEProjectProperties.ui
    ComponentCreateConfig.ui
	
	ComponentGenerator/ComponentGenerator.ui
    )
