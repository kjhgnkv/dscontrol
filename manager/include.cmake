set(models
    models/AbstractItemModel.cpp
    models/AbstractItemModel.hpp

    models/AbstractItemModelMonitor.cpp
    models/AbstractItemModelMonitor.hpp

    models/AbstractNodeItem.cpp
    models/AbstractNodeItem.hpp

    models/AbstractNodeItemMonitor.cpp
    models/AbstractNodeItemMonitor.hpp

    models/AllConnectionModel.cpp
    models/AllConnectionModel.hpp

    models/CommunicationModel.cpp
    models/CommunicationModel.hpp

    models/ConnectionsStaticModel.cpp
    models/ConnectionsStaticModel.hpp

    models/DSProxyModel.cpp
    models/DSProxyModel.hpp

    models/MessageModel.cpp
    models/MessageModel.hpp

    models/MimeStoreModel.cpp
    models/MimeStoreModel.hpp

    models/NestedItem.cpp
    models/NestedItem.hpp

    models/NodeItem.cpp
    models/NodeItem.hpp

    models/ObjectConnectionModel.cpp
    models/ObjectConnectionModel.hpp

    models/ProjectModel.cpp
    models/ProjectModel.hpp

    models/ProjectModelItem.cpp
    models/ProjectModelItem.hpp

    models/ProjectTreeItem.cpp
    models/ProjectTreeItem.hpp

    models/ProjectTreeModel.cpp
    models/ProjectTreeModel.hpp

    models/PropertiesItemDelegate.cpp
    models/PropertiesItemDelegate.hpp

    models/PropertiesProxyModel.cpp
    models/PropertiesProxyModel.hpp

    models/PropertiesTreeItem.cpp
    models/PropertiesTreeItem.hpp

    models/PropertiesTreeModel.cpp
    models/PropertiesTreeModel.hpp

    models/RecentProjectsModel.cpp
    models/RecentProjectsModel.hpp

    models/RootItem.cpp
    models/RootItem.hpp

    models/SingleTinyMapModel.cpp
    models/SingleTinyMapModel.hpp

    models/StoredItem.cpp
    models/StoredItem.hpp

    models/TinyMapModel.cpp
    models/TinyMapModel.hpp
    )

set(manager
    BaseSettings.cpp
    BaseSettings.h
	
	WindowsSettings.cpp
    WindowsSettings.h
	
	LinuxSettings.cpp
    LinuxSettings.h

    ComponentCreator.cpp
    ComponentCreator.h

    ControllManager.cpp
    ControllManager.hpp

    CustomDesignerPluginGenerator.cpp
    CustomDesignerPluginGenerator.h

    DSConfiguration.cpp
    DSConfiguration.h

    IDEProjectCreator.cpp
    IDEProjectCreator.hpp

    MessagesDeserializer.cpp
    MessagesDeserializer.hpp

    OutXmlGenerator.cpp
    OutXmlGenerator.hpp

    Project.cpp
    Project.hpp

    ProjectDocumentCreator.cpp
    ProjectDocumentCreator.hpp
    )
