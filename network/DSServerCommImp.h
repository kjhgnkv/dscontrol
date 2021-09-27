#ifndef _DS_SERVER_COMM_IMP
#define _DS_SERVER_COMM_IMP
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
#include <DSServerCommImpBase.h>
using Poco::Timer;
using Poco::TimerCallback;
#define DSSEVER_PORT 6030
#define DSCONTROL_PORT 7030

using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::Timestamp;


/*class DSServerCommImp is used to immplement communincation between DSControl and DSServer*/
class DSServerCommImp : public QObject, public Poco::RefCountedObject, public Poco::Runnable
{
    Q_OBJECT;
public:
    typedef Poco::AutoPtr<DSServerCommImp> Ptr;
    //construct funtion
    DSServerCommImp(std::string address = Poco::Net::IPAddress().toString(), int port = DSCONTROL_PORT);
    bool init(std::vector<std::string>& serverList);
    void run();
    static std::vector<std::string> getLocalIPAddresses();
    int setDeployTask(std::vector<DeployTask::Ptr>& delpoyTasks);
    bool setFtpHome(std::string path);
    std::string getFtpHome();
    DSServerStatus::Ptr fetchDeployDSServer();
    bool startDeploy();
    void setMonitor(bool start);
    bool isRunning();
    bool stop();
    static std::string converseIPStringFromInt32(quint32 ip);
signals:
    void updatedMonitorData(quint32 ip, QString* info);
private:
    bool _running;
    std::string _runningInfo;
    std::map<std::string, DSServerStatus::Ptr> _dsserverList;
    mutable Poco::RWLock _socketLock;

    DatagramSocket _socket;
    std::string _address;
    int _port;

    Poco::Thread _thread;
    TimerHeardBeat* _hearBeatTimer;
    Timer _timer;

    bool _isFTPOn;
    fineftp::FtpServer* _ftpServer;
    std::string _ftpHome;
    int _ftpPort;

    bool  startDeployInHeardBeat;
    bool dispatch(RecieveCommand::Ptr command);
    bool onReHeartBeat(RecieveCommand::Ptr command);
    bool startFTP();
    bool shotDownFTP();
    bool starDeploy(DSServerStatus::Ptr dsServer);
    bool onReDeploy(RecieveCommand::Ptr command);
    bool onReStart(RecieveCommand::Ptr command);
    bool onReStop(RecieveCommand::Ptr command);
    bool onReLeave(RecieveCommand::Ptr command);
    bool checkCommandSuccess(RecieveCommand::Ptr command);
    void sendDelayCommand();
};
#endif
