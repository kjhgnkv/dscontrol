# This is the mother project of CTK-based plugin system

# Setup plugin according to meta-data
include(../setupPlugin.pri)
setupManifest()

# QT modules
QT += {{qt_module}}

# $$OSGI_MAINPATH --- path of OSGi main project
# $$COMMON_INTERFACE_PATH --- path of common interface files
# $$INCLUDE_PATH --- path of include files of third-party or CBB modules
# $$LIB_PATH --- path of library files of third-party or CBB modules

# headers
HEADERS += \
    Activator.h

# sources
SOURCES += \
    Activator.cpp

# forms
FORMS +=

# resource
RESOURCES += 

# Define the files that share with other plugins
SHARED_FILES +=
# Deploy plugin
deployPlugin()
