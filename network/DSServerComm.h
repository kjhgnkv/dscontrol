#ifndef _DS_SERVER_COMM
#define _DS_SERVER_COMM
#include <vector>
#include <map>
#include <string>
#include <QObject>

class DSServerCommImp;
class DSServerComm : public QObject
{
    Q_OBJECT
public:
	DSServerComm();
    ~DSServerComm();
	void Init(std::vector<std::string>& serverList, std::string address = "");
    void Stop();
	void UpdateRunInfo(std::string add, std::string json);
	bool AssignDeployTask(std::string add, std::string name, std::string file);
	bool StartDeployTask();
	bool GetDeployStatus(std::string add);
    void SetFtpHome(std::string path);
    void SetMonitor(bool start);
    static std::vector<std::string> getLocalIPAddresses();
    static std::string converseIPStringFromInt32(quint32 ip);
signals:
    void UpdatedMonitorData(quint32 ip, QString* info);

private:
	DSServerCommImp* _side;
	std::map<std::string, std::map<std::string, std::string>> _task;
};
#endif
