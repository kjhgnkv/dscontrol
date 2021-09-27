#include "DSServerCommImp.h"
#include <QDebug>
#include <QObject>
#include <QSharedPointer>
DelayCommand::DelayCommand(SendCommand::Ptr command, Poco::RWLock& socketLock, int delay)
	: _command(command), _delay(delay), _isSend(false), _socketLock(socketLock) 
{
	_timeStamp.update();
}
//check if the time is past
//return true if the time is past, or return false
bool DelayCommand::isTimeOut() 
{
	Timestamp::TimeDiff diff = _timeStamp.elapsed();
	long target = _delay * (long)1000;
	return  diff >= target;
}
//send the command
//paramter: address is the target address
//return true if cuccess, or return false
bool DelayCommand::SendDelayCommand(SocketAddress& address) {
	Poco::ScopedWriteRWLock lock(_socketLock);
	bool result = _command->sendCommand(address);
	_isSend = true;
	return result;
}
//check if the command is send
//return if the command is send
bool DelayCommand::isSend() {
	return _isSend;
}

DeployTask::DeployTask(std::string name, std::string file, std::string address, UINT64 size)
    : _name(name), _file(file), _fileSize(size), _address(address){
	_start = false;
	_complete = false;
}


DSServerStatus::DSServerStatus(): _heartBeat(0), _isConnected(false), _currentDeployTask(NULL) {
	resetHeartBeat();
	_deployed = true;
	_deployFinish = true;
}

//this function recode remote server address
void DSServerStatus::assignAddress(SocketAddress address)
{
	_remoteAddress = address;
}

//return remote address
SocketAddress DSServerStatus::getRemoteAddress() 
{
	return _remoteAddress;
}
//set deplay file name
//paramter : file is the file name, without directory
void DSServerStatus::addDeployTask(DeployTask::Ptr task)
{
	_deployTasks.push(task);
}
bool DSServerStatus::hasDeployTask()
{
	return _deployTasks.size() > 0;
}
bool DSServerStatus::fetchDeployTask()
{
	if (!_currentDeployTask
        || (_currentDeployTask && _currentDeployTask->_complete))
	{
		if (!_deployTasks.size())
			return false;
		_currentDeployTask = _deployTasks.front();
		_deployTasks.pop();
		return true;
	}
	return true;
}
std::string DSServerStatus::getDeployFile() {
	if (!_currentDeployTask)
		return "NO this file";
	return _currentDeployTask->_file;
}
std::string DSServerStatus::getDeployName() {
	if (!_currentDeployTask)
		return "NO this name";
	return _currentDeployTask->_name;
}
//add a delay command
void DSServerStatus::addDelayCommand(DelayCommand::Ptr delayCommand)
{
	_delayCommands.push_back(delayCommand);
}
//send all delay command that is timeout
void DSServerStatus::dealDelayCommands() {
	std::vector<DelayCommand::Ptr>::iterator it = _delayCommands.begin();
	while (it != _delayCommands.end())
	{
		if ((*it)->isTimeOut())
		{
            (*it)->SendDelayCommand(_remoteAddress);
			_delayCommands.erase(it);
			break;
		}
		it++;
	}
}

//recode once heart beat without answer
void DSServerStatus::recodeHeartBeatWithoutAnswer() {
	_heartBeat++;
	if (_heartBeat > MAX_HEARD_COUNT)
		_isConnected = false;
}
//clear the heart beat  without answer
void DSServerStatus::resetHeartBeat() {
	_heartBeat = 0;
	_isConnected = true;
}
//check if this DSServer is active
bool DSServerStatus::isActived() {
	return _isConnected;
}

//check if the deploy start
bool DSServerStatus::isDeployStart() {
	if (!_currentDeployTask)
		return false;
	return _currentDeployTask->_start;
}
void DSServerStatus::setDeployStart(bool isDeployStart) {
	if (!_currentDeployTask)
		return;
	_currentDeployTask->_start = isDeployStart;
}
//check if the deploy finish
bool DSServerStatus::isDeployFinish() {
	if (!_currentDeployTask)
		return false;
	return _currentDeployTask->_complete;
}
void DSServerStatus::setDeployFinish(bool finish) {
	if (!_currentDeployTask)
		return;
	_currentDeployTask->_complete = finish;
}

void DSServerStatus::setRuningInformation(std::string info, bool finish) {
	_tempRuningInfo = info;
	if (finish)
		_runningInfo = _tempRuningInfo;
}
void DSServerStatus::appendRuningInformation(std::string info, bool finish)
{
	_tempRuningInfo += info;
	if (finish)
		_runningInfo = _tempRuningInfo;
}
QString* DSServerStatus::getRuningInformation()
{
    _runningInformation=_runningInfo.c_str();
    return &_runningInformation;
}
quint32 DSServerStatus::getRemoteAddressIP()
{
    _remoteAddressIP = inet_pton(AF_INET, getRemoteAddress().host().toString().c_str(), &_remoteAddressIP);
    return _remoteAddressIP;
}
UINT64 DSServerStatus::getFileSize()
{
    return _currentDeployTask->_fileSize;
}
TimerHeardBeat::TimerHeardBeat(DatagramSocket& socket, Poco::RWLock& socketLock,
	std::map<std::string, DSServerStatus::Ptr>& serverList)
    :_socket(socket), _serverList(serverList), _socketLock(socketLock), _needSend(false)
{
    ;
}
void TimerHeardBeat::onTimer(Timer&)
{
    if(!_needSend)
    {
        return;
    }
	SendCommand sendcmd(_socket, CMD_HEARTBEAT, true);
	unsigned char c = 0;
	sendcmd.setData(&c, 0, 0, true);
    //std::cout << "send CMD_HEARTBEAT" << std::endl;
	//send all DSServer heard beat
	for (std::map<std::string, DSServerStatus::Ptr>::iterator it = this->_serverList.begin();
		it != _serverList.end(); it++)
	{
		Poco::ScopedWriteRWLock lock(_socketLock);
		sendcmd.sendCommand(SocketAddress(it->first, DSSEVER_PORT));
		it->second->recodeHeartBeatWithoutAnswer();//before get answer, record once HeartBeat without answer
		if (!it->second->isActived())
		{
            //std::cout << it->first << " no respond" << std::endl;
		}
	}
}
void TimerHeardBeat::setSend(bool needSend)
{
    _needSend=needSend;
}
DSServerCommImp::DSServerCommImp(std::string address, int port) : _running(false),
    _address(address),_port(port),_hearBeatTimer(nullptr),_isFTPOn(false), _ftpServer(nullptr) {
	_ftpHome = "./";
	_ftpPort = 21;
	startDeployInHeardBeat = true;
}
//initialize this DSServerCommImp with DSServer list
//this function also will start thread and timer, this causes this function 
//can be called only once.
//paramters: serverList a list of DSServer, every item is a DSServer
//return true is success, or return false
bool DSServerCommImp::init(std::vector<std::string>& serverList) 
{
	if (_running)//if already initialized , return false;
		return false;
	_socket.bind(SocketAddress(_address, _port));//setup socket
	_dsserverList.clear();
	//build DSServer list by serverList
	for (std::vector<std::string>::iterator it = serverList.begin();
		it != serverList.end(); it++) {
		std::string add = *it;
		if (_dsserverList.find(add) == _dsserverList.end())
			_dsserverList[add] = new DSServerStatus();
		else
			return false;
	}
	if (_dsserverList.size() == 0)
		return false;
	//start HeardBeat timer
	_hearBeatTimer = new TimerHeardBeat(_socket, _socketLock, _dsserverList);
	_timer.setStartInterval(100);
	_timer.setPeriodicInterval(5000);
	_timer.start(TimerCallback<TimerHeardBeat>(*_hearBeatTimer, &TimerHeardBeat::onTimer));

	_running = true;
	//start thread for recieving data
	_thread.start(*this);

	return true;
}
bool DSServerCommImp::isRunning()
{
    return _running;
}
bool DSServerCommImp::stop()
{
    try {
        _timer.stop();
        if(_hearBeatTimer)
        {
            delete _hearBeatTimer;
            _hearBeatTimer=nullptr;
        }
        if(_running)
        {
            _running=false;
            _thread.join();
        }
        _dsserverList.clear();
        _socket.close();
        shotDownFTP();
        return true;
    }  catch (std::exception&) {
        ;
    }
    return false;

}
//run function is used to recive data
void DSServerCommImp::run() {
	while (_running) {
		sendDelayCommand();//check and send all time out delay command
		int available = _socket.available(); //check revice data
		if (available) {
			char buf[MAX_BUF];
			SocketAddress remoteAddress;
			try {
				int receiveCount = _socket.receiveFrom(buf, MAX_BUF, remoteAddress);
				RecieveCommand* precieveCommand = new RecieveCommand(buf, receiveCount, remoteAddress);
				dispatch(precieveCommand);// deal with recieve data
			}
			catch (std::exception e)
			{
				std::string info = e.what();
			}
		}
	}
}
//this function list all IPV4 address of local machine
//return the address list
std::vector<std::string> DSServerCommImp::getLocalIPAddresses() {
	std::vector<std::string> addressList;
	using Poco::Net::NetworkInterface;
	using Poco::Net::IPAddress;
	Poco::Net::NetworkInterface::Map m = Poco::Net::NetworkInterface::map(false, false);
	for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		const Poco::Net::NetworkInterface::AddressList& ipList = it->second.addressList();
		Poco::Net::NetworkInterface::AddressList::const_iterator ipIt;
		for (ipIt = ipList.begin(); ipIt != ipList.end(); ++ipIt)
		{
			if (!ipIt->get<Poco::Net::NetworkInterface::IP_ADDRESS>().isSiteLocal())
			{
				continue;
			}
			addressList.push_back(ipIt->get<NetworkInterface::IP_ADDRESS>().toString());
		}
	}
	return addressList;
}
//set the deploy task
//paramter: delpoyMap is list of pair, first is IP address, second is the file name
// 	   if the first is not in the _dsserverList, it can't be accepted
//return the count can be accpted
int DSServerCommImp::setDeployTask(std::vector<DeployTask::Ptr>& delpoyTasks)
{
	int count = 0;
	std::vector<DeployTask::Ptr>::iterator delpoyTaskIt = delpoyTasks.begin();
	while (delpoyTaskIt != delpoyTasks.end())
	{
		std::string address = (*delpoyTaskIt)->_address;
		if (_dsserverList.find(address) != _dsserverList.end())//if the _dsserverList as this address
		{
			_dsserverList[address]->addDeployTask(*delpoyTaskIt);
		}
		delpoyTaskIt++;
	}
	return count;
}
//set the FTP home directory
//paramter: path is the path where the deployment files locate
//return true if success, or return false
bool DSServerCommImp::setFtpHome(std::string path)
{
	Poco::File home(path);
	if (!home.exists())
		return false;
	if (!home.isDirectory())
		return false;
	_ftpHome = path;
	return true;
}
std::string DSServerCommImp::getFtpHome()
{
    return _ftpHome;
}
//fetch a deploy DSServer, this function would not offer new DSServer before 
//the current is completed
//return the DSServerStatus if find, or return NULL
DSServerStatus::Ptr DSServerCommImp::fetchDeployDSServer()
{
	for (std::map<std::string, DSServerStatus::Ptr>::iterator it = _dsserverList.begin();
		it != _dsserverList.end(); it++)
	{
		if (it->second->isActived())
		{
			//find a running task, return it
			if (it->second->isDeployStart() && !it->second->isDeployFinish())
				return it->second;
			else if (!it->second->isDeployStart())
				return it->second;
			else {
				if (it->second->hasDeployTask())
					return it->second;
			}
		}
	}
	return NULL;
}
//start deploy, this funciton will start a valid deploy task when it is called,
//untill finish all tasks, but this function would not start new task before 
//the last is completed
//return true if success, or return false if there is not any more deploy task
bool DSServerCommImp::startDeploy()
{
	DSServerStatus::Ptr dsServer = fetchDeployDSServer();
	if (dsServer)
	{
		if (!dsServer->isDeployStart()
            ||(dsServer->isDeployStart() && dsServer->isDeployFinish()))
			starDeploy(dsServer);
		return true;
	}
	else
		return false;
}

//this function is used to dispatch recieved command to treating functions
//return true if success, or return false
bool DSServerCommImp::dispatch(RecieveCommand::Ptr command) 
{
	char cmd = command->getCommand();
	switch (cmd)
	{
	case CMD_RE_HEARTBEAT:
		return onReHeartBeat(command);
	case CMD_RE_DEPLOY:
	case CMD_RE_CHECK_DEPLOY:
		return onReDeploy(command);
	case CMD_RE_FILE:
		return false;
	case CMD_RE_START:
		return onReStart(command);
	case CMD_RE_STOP:
		return onReStop(command);
	case CMD_RE_LEAVE:
		return onReLeave(command);
	default:
		return false;
	}
}
//deal with reply of HeartBeat
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::onReHeartBeat(RecieveCommand::Ptr command)
{
	std::string add = command->getAddress().host().toString();
	if (_dsserverList.find(add) == _dsserverList.end())//can't find the dssserver
	{
		return false;
	}
	DSServerStatus* dsServer = _dsserverList[add];
	dsServer->assignAddress(command->getAddress());
	dsServer->resetHeartBeat();

	unsigned char block[MAX_BUF] = { 0 };
	size_t packageSize = command->getMultiPakageContent(block, MAX_BUF);

	if (!command->isMultiComplete())//if the running information is NOT completed
	{
		Poco::ScopedWriteRWLock lock(_socketLock);
		short index = command->getMultiIndex();//get current index
		if (!index)//first package informaiton
		{
			dsServer->setRuningInformation((char*)block);
		}
		else
		{
			dsServer->appendRuningInformation((char*)block);;
		}
		//build a command and send it to fetch next package of runining information
		SendCommand sendcmd(_socket, CMD_HEARTBEAT, true);
		sendcmd.setData(block, 0, index + 1, true);
		sendcmd.sendCommand(command->getAddress());
	}
	else
	{
		short index = command->getMultiIndex();
		if (!index)//first package, means all information is in one packege
		{
			dsServer->setRuningInformation((char*)block, true);
		}
		else
		{
			dsServer->appendRuningInformation((char*)block, true);
		}
        emit updatedMonitorData( dsServer->getRemoteAddressIP(), dsServer->getRuningInformation());
	}
	return true;
}
//start FTP server, ftp will be started only once
//return true if success, or return false 
bool DSServerCommImp::startFTP() {
	if (_isFTPOn)
		return true;
	try 
	{
		_ftpServer = new fineftp::FtpServer(_address, _ftpPort);
        //std::cout << "start ftp!\n";
		_ftpServer->addUserAnonymous(_ftpHome, fineftp::Permission::All);
		_ftpServer->start(4);
		_isFTPOn = true;
	}
	catch (std::exception& ex) {
		std::string info = ex.what();
	}
	return true;
}
//stop FTP server,
//return true if success, or return false 
bool DSServerCommImp::shotDownFTP()
{
	if (_ftpServer && _isFTPOn)
	{
		_ftpServer->stop();
		delete _ftpServer;
		_ftpServer = NULL;
		return true;
	}
	_isFTPOn = false;
	return false;
}
//start a deploy task to DSServer
//paramters dsServer is a DSServerStatus::Ptr
//return true if success, or return false 
bool DSServerCommImp::starDeploy(DSServerStatus::Ptr dsServer)
{
	Poco::ScopedWriteRWLock lock(_socketLock);
	if (!dsServer->fetchDeployTask())
		return false;
	if (!startFTP())
		return false;
	std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><File FtpAddress=\"";
	xml += _address;
	xml += "\" Port=\"";
	xml += std::to_string(_ftpPort);
	xml += "\" user=\"anonymous\" password=\"\" name=\"";
	xml += dsServer->getDeployName() + "\" file=\"";
    xml += dsServer->getDeployFile() + "\" size=\"";
    std::string size=std::to_string(dsServer->getFileSize());
    xml += size +"\" />";
	SendCommand sendcmd(_socket, CMD_DEPLOY);
	sendcmd.setData((unsigned char*)xml.c_str(), xml.length(), 0, true);
	sendcmd.sendCommand(dsServer->getRemoteAddress());
	dsServer->setDeployStart(true);
	return true;
}
//deal with reply of Deploy
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::onReDeploy(RecieveCommand::Ptr command)
{
	std::string add = command->getAddress().host().toString();
	//if can't find address in _dsserverList, return
	if (_dsserverList.find(add) == _dsserverList.end())
		return false;
	DSServerStatus::Ptr server = _dsserverList[add];
	unsigned char result[MAX_BUF] = { 0 };
    command->getData(result, MAX_BUF);
	unsigned char deployStatus = result[0];
	//check result of deploy, if the task is running then add a delay check command
	if (COMMAND_RUNNING == deployStatus)
	{
		SendCommand* sendcmd = new SendCommand(_socket, CMD_CHECK_DEPLOY);
		server->addDelayCommand(new DelayCommand(sendcmd, _socketLock));
        //std::cout << "add a delay check" << std::endl;
	}
	//if the task has been done
	else if (COMMAND_DONE == deployStatus)
	{
        //std::cout << "curent deploy task has bean done, start next..." << std::endl;
		server->setDeployFinish(true);
		if (!startDeploy())//there is no more deploy task
		{
            //std::cout << "all deploy task have bean done" << std::endl;
			shotDownFTP();

			std::string name = "Application_4";
			SendCommand* sendcmd = new SendCommand(_socket, CMD_STOP);
			sendcmd->setData((unsigned char*)name.c_str(), name.size(), 0, true);
			server->addDelayCommand(new DelayCommand(sendcmd, _socketLock));
		}
        return true;
	}
	else if (COMMAND_WAITING == deployStatus)
	{
        //std::cout << "waiting deploy" << std::endl;
		SendCommand* sendcmd = new SendCommand(_socket, CMD_CHECK_DEPLOY);
		server->addDelayCommand(new DelayCommand(sendcmd, _socketLock));
        return true;
        //std::cout << "add a deplay check" << std::endl;
	}
	//if the task fail
	else if (COMMAND_FAIL == deployStatus)
	{
        //std::cout << add << "deploy fail" << std::endl;
		if (!startDeploy())//there is no more deploy task
		{
			shotDownFTP();
            //std::cout << "deploy has done" << std::endl;
		}
        return true;
	}
    return false;
}
//deal with reply of Start command
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::onReStart(RecieveCommand::Ptr command) {
	std::string add = command->getAddress().host().toString();
	bool sucess = checkCommandSuccess(command);
	if (sucess)
	{
		DSServerStatus::Ptr server = _dsserverList[add];
		SendCommand* sendcmd = new SendCommand(_socket, CMD_LEAVE);

		server->addDelayCommand(new DelayCommand(sendcmd, _socketLock));
	}
	return sucess;
}
//deal with reply of Stop command
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::onReStop(RecieveCommand::Ptr command) {
	std::string add = command->getAddress().host().toString();
	bool sucess = checkCommandSuccess(command);
	if (sucess)
	{
		DSServerStatus::Ptr server = _dsserverList[add];
		SendCommand* sendcmd = new SendCommand(_socket, CMD_START);
		std::string name = "Application_4";
		sendcmd->setData((unsigned char*)name.c_str(), name.size(), 0, true);
		server->addDelayCommand(new DelayCommand(sendcmd, _socketLock));
	}
	return sucess;
}
//deal with reply of Leave command
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::onReLeave(RecieveCommand::Ptr command) {
	bool sucess = checkCommandSuccess(command);
	return sucess;
}
//check the result of reply command
//paramter: command the recieve command
//return true if success, or return false 
bool DSServerCommImp::checkCommandSuccess(RecieveCommand::Ptr command)
{
	unsigned char buf[MAX_BUF];
	memset(buf, 0, MAX_BUF);
	int len = command->getData(buf, MAX_BUF);
	return buf[0];
}
//send all delay command that is time out
void DSServerCommImp::sendDelayCommand() {
	for (std::map<std::string, DSServerStatus::Ptr>::iterator it = _dsserverList.begin();
		it != _dsserverList.end(); it++) {
		(*it).second->dealDelayCommands();
	}
}

void DSServerCommImp::setMonitor(bool start)
{
    if(_hearBeatTimer)
        _hearBeatTimer->setSend(start);
}

std::string DSServerCommImp::converseIPStringFromInt32(quint32 ip)
{
    char str[INET_ADDRSTRLEN];
    std::string ipString;
    inet_ntop(AF_INET, &ip, str, INET_ADDRSTRLEN);
    ipString=str;
    return ipString;
}
