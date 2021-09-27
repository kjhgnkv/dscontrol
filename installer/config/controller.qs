function Controller()
{
    installer.installationFinished.connect(this, installOtherProgramms);
}

installOtherProgramms = function()
{
    if (installer.fileExists(installer.value("TargetDir") + "/mosquitto-1.6.8-install-windows-x64.exe"))
    {
        console.log("mosquitto-1.6.8-install-windows-x64.exe found in " + installer.value("TargetDir"));
        installer.execute(installer.value("TargetDir") + "/mosquitto-1.6.8-install-windows-x64.exe");
    }
    else
    {
        console.log("mosquitto-1.6.8-install-windows-x64.exe not found in " + installer.value("TargetDir"));
    }
	
    if (installer.fileExists(installer.value("TargetDir") + "/Win10Pcap-v10.2-5002.msi"))
    {
        console.log("Win10Pcap-v10.2-5002.msi found in " + installer.value("TargetDir"));
        installer.execute(installer.value("TargetDir") + "/Win10Pcap-v10.2-5002.msi");
    }
    else
    {
        console.log("Win10Pcap-v10.2-5002.msi not found in " + installer.value("TargetDir"));
    }
}
