#ifndef _DS_SERVER_COMM_IMP_BASE
#define _DS_SERVER_COMM_IMP_BASE
#include "NetDefine.h"
#include <fineftp/server.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/AutoPtr.h>
#include <Poco/RefCountedObject.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/Timestamp.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/File.h>
#include <Poco/ScopedLock.h>
#include <Poco/RWLock.h>
#include <Poco/Timer.h>
#include <Poco/Thread.h>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <QObject>

using Poco::Timer;
using Poco::TimerCallback;

using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::Timestamp;
#define DELAY 5000

/*class DelayCommand is for command that will be send later
*/
class DelayCommand :public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr<DelayCommand> Ptr;
	explicit DelayCommand(SendCommand::Ptr command, Poco::RWLock& socketLock, int delay = DELAY);
	bool isTimeOut();
    bool SendDelayCommand(SocketAddress& address);
	bool isSend();
private:
	SendCommand::Ptr _command;
	long _delay;
	Timestamp::TimeVal _time;
	Timestamp _timeStamp;
	bool _isSend;
	Poco::RWLock& _socketLock;
};
struct DeployTask : public Poco::RefCountedObject
{
public:
	typedef Poco::AutoPtr<DeployTask> Ptr;
    DeployTask(std::string name, std::string file, std::string address, UINT64 size);
	std::string _name;
	std::string _file;
    UINT64 _fileSize;
	std::string _address;
	bool _start;
	bool _complete;
};

/*class DSServerStatus is status of a DSServer*/
class DSServerStatus :public Poco::RefCountedObject
{
public:
	explicit DSServerStatus();
	typedef Poco::AutoPtr< DSServerStatus> Ptr;
	void assignAddress(SocketAddress address);
	SocketAddress getRemoteAddress();
	void addDeployTask(DeployTask::Ptr task);
	bool hasDeployTask();
	bool fetchDeployTask();
	std::string getDeployFile();
	std::string getDeployName();
    UINT64 getFileSize();
	void addDelayCommand(DelayCommand::Ptr delayCommand);
	void dealDelayCommands();
	void recodeHeartBeatWithoutAnswer();
	void resetHeartBeat();
	bool isActived();
	bool isDeployStart();
	void setDeployStart(bool isDeployStart);
	bool isDeployFinish();
	void setDeployFinish(bool finish);
	
	void setRuningInformation(std::string info, bool finish=false);
	void appendRuningInformation(std::string info, bool finish=false);
    QString* getRuningInformation();
    quint32 getRemoteAddressIP();
    static std::string converseIPStringByInt32(qint32 ip);
private:
	int _heartBeat;
    QString _runningInformation;
    unsigned int _remoteAddressIP;
	SocketAddress _remoteAddress;
	bool _isConnected;
	std::string _deployFile;
	std::queue<DeployTask::Ptr> _deployTasks;
	DeployTask::Ptr _currentDeployTask;
	bool _deployed;
	bool _deployFinish;
	std::vector<DelayCommand::Ptr> _delayCommands;
	std::string _runningInfo;
	std::string _tempRuningInfo;
};
/*class TimerHeardBeat is used to send HeardBeat command by periodic interval*/
class TimerHeardBeat
{
public:
	//construct function
	TimerHeardBeat(DatagramSocket& socket, Poco::RWLock& socketLock,
		std::map<std::string, DSServerStatus::Ptr>& serverList);
	//onTimer is the callback function
	void onTimer(Timer& timer);
    void setSend(bool needSend);
private:
	DatagramSocket& _socket;
	std::map<std::string, DSServerStatus::Ptr>& _serverList;
	Poco::RWLock& _socketLock;
    bool _needSend;
};

#endif
