#include "src/Monitor.h"
#include <iostream>

#include "src/DSMonitorHelp.hpp"
#include "test_xml.hpp"

int main()
{
    Poco::SharedPtr<dsmonitor::Monitor> monitor = dsmonitor::Monitor::instance();

    // while (true)
    {
        // if (!timer.isProcessed())
        {
            monitor->parser(xml_deploy);
            monitor->parser(xml_deploy_2);
            monitor->update();
            dsmonitor::Help::logMemory();
        }
    }

    monitor.reset();

    // while (true)
    {
        // for (int i = 0; i < 1000; i++)
        {
            // Monitor m = new dsmonitor::Monitor;
            // m->parserXML(xml_deploy);
            // m->parserXML(xml_deploy_2);
            // m->update();
            // std::cout << m->json();
            // vec.push_back(m);
        }
        // logMemory();

        // std::vector<Monitor>::reverse_iterator it, end;
        // for (it = vec.rbegin(), end = vec.rend(); it != end; it++)
        {
            // const Poco::SharedPtr<dsmonitor::Parser> &ptr = (*it)->parser();
            // ptr->showList();
            // removePtr(*it);
            // vec.erase((it++).base(), vec.end());
        }
        // vec.clear();

        // logMemory();
    }

    std::cout << "end" << std::endl;
    return 0;
}
