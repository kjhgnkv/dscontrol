set(component
    component/NodeItemComponent.hpp
    component/NodeItemObject.hpp
    )

set(node
    node/NodeItemMonitorApplication.cpp
    node/NodeItemMonitorApplication.hpp

    node/NodeItemMonitorComputer.cpp
    node/NodeItemMonitorComputer.hpp

    node/NodeItemMonitorThread.cpp
    node/NodeItemMonitorThread.hpp

    node/NodeItemMonitorTree.cpp
    node/NodeItemMonitorTree.hpp

    node/Icon/Icon.cpp
    node/Icon/Icon.hpp
    )

set(push
    push/MonitorPush.cpp
    push/MonitorPush.hpp
    push/MonitorPush.ui
    )

set(computer
    node/Computer/Cpu/Cpu.cpp
    node/Computer/Cpu/Cpu.hpp

    node/Computer/Memory/Memory.cpp
    node/Computer/Memory/Memory.hpp

    node/Computer/Network/Network.cpp
    node/Computer/Network/Network.hpp

    node/Computer/Storage/Storage.cpp
    node/Computer/Storage/Storage.hpp

    node/Computer/System/System.cpp
    node/Computer/System/System.hpp

    node/Computer/Computer.cpp
    node/Computer/Computer.hpp
    )

set(application
    node/Application/Application.cpp
    node/Application/Application.hpp
    )

set(threads
    node/Thread/Thread.cpp
    node/Thread/Thread.hpp
    )

set(monitor
    MonitorWidget.cpp
    MonitorWidget.hpp
    MonitorWidget.ui
    )

set(dsmonitor
    ../../network/businessComponents/DSMonitor/src
    )