function Component()
{
    component.loaded.connect(this, Component.prototype.loaded);
}

Component.prototype.loaded = function ()
{
    console.log("!!!");
	console.log(installer.value("TargetDir"));
    component.addOperation("execute", "cmd /C", "@TargetDir@\mosquitto-1.6.8-install-windows-x64.exe");
}
