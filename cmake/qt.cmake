set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Widgets Xml XmlPatterns Network Svg Sql Designer DesignerComponents)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Widgets Qt5::Xml Qt5::XmlPatterns Qt5::Network Qt5::Svg Qt5::Sql Qt5::Designer Qt5::DesignerComponents)

foreach (LIBS ${REQUIRED_LIBS})
    find_package(Qt${QT_VERSION}${LIBS} COMPONENTS REQUIRED)
endforeach ()

find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QTVERSION)
set(QtDesigner ${CMAKE_PREFIX_PATH}/include/QtDesigner/${QTVERSION})
#for Yura to build. Please, live it, I will fix later.
#set(QtDesigner c:/Qt/Qt5.13.2/5.15.2/msvc2019_64/include/QtDesigner/${QTVERSION})
include_directories(${QtDesigner})

