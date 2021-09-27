
#include "DSServerComm.h"
#include "NetDefine.h"
#include "DSServerCommImp.h"
#include <QObject>
#include <QSharedPointer>
#include <Poco/File.h>
#include <Poco/Path.h>

DSServerComm::DSServerComm():_side(nullptr)
{
	;
}
DSServerComm::~DSServerComm()
{
    if(_side)
    {
        delete _side;
    }
}
void DSServerComm::Init(std::vector<std::string>& serverList, std::string address)
{
	std::string IP;
	if (address.size() > 0)
		IP = address;
	else
	{
		std::vector<std::string>  addressList = DSServerCommImp::getLocalIPAddresses();
		IP = addressList.size() ? addressList[0] : Poco::Net::IPAddress().toString();
	}
    if(!_side)
    {
        _side=new DSServerCommImp (IP);
        connect(_side, &DSServerCommImp::updatedMonitorData, this, &DSServerComm::UpdatedMonitorData );
        _side->init(serverList);
    }

}
void DSServerComm::Stop()
{
    if(_side)
    {
        _side->stop();
    }
}
void DSServerComm::UpdateRunInfo(std::string add, std::string json)
{
	;
}
bool DSServerComm::AssignDeployTask(std::string add, std::string name, std::string file)
{
	_task[add][name]=file;
	return true;
}
bool DSServerComm::StartDeployTask()
{
	std::vector<DeployTask::Ptr> deployTask;
	for (std::map<std::string, std::map<std::string, std::string>>::iterator serverIt = _task.begin();
		serverIt != _task.end(); serverIt++)
	{
		for (std::map<std::string, std::string>::iterator taskIt = serverIt->second.begin();
			taskIt != serverIt->second.end(); taskIt++)
		{
            std::string ftpPath=_side->getFtpHome();
            if (ftpPath[ftpPath.size()-1] != Poco::Path::separator())
                ftpPath+=Poco::Path::separator();

            Poco::File file(ftpPath+taskIt->second);
            deployTask.push_back(new DeployTask(taskIt->first, taskIt->second, serverIt->first, file.getSize()));
		}
	}
	_side->setDeployTask(deployTask);
    _side->startDeploy();
	return true;
}
bool DSServerComm::GetDeployStatus(std::string add)
{
	return false;
}
void DSServerComm::SetFtpHome(std::string path)
{
	_side->setFtpHome(path);
}
std::vector<std::string> DSServerComm::getLocalIPAddresses()
{
    return DSServerCommImp::getLocalIPAddresses();
}
void DSServerComm::SetMonitor(bool start)
{
    _side->setMonitor(start);
}
std::string DSServerComm::converseIPStringFromInt32(quint32 ip)
{
    return DSServerCommImp::converseIPStringFromInt32(ip);
}
